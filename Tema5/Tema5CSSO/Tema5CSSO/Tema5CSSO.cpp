// Tema5CSSO.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "advapi32.lib")

#include <Windows.h>
#include <iostream>
#include <securitybaseapi.h>
#include <accctrl.h>
#include <AclAPI.h>
#include <stdio.h>
#include <tchar.h>
#include <stdio.h> 
using namespace std;

int main()
{
	PSECURITY_DESCRIPTOR pSecurityDescriptor = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	PSID ownerSid = (PSID)LocalAlloc(LPTR, SECURITY_MAX_SID_SIZE);
	PSID worldSid = (PSID)LocalAlloc(LPTR, SECURITY_MAX_SID_SIZE);
	DWORD cbOwnerSid = SECURITY_MAX_SID_SIZE;
	DWORD cbWorldSid = SECURITY_MAX_SID_SIZE;

	EXPLICIT_ACCESS pListOfExplicitEntries[2];

	PACL NewAcl = NULL;

	SECURITY_ATTRIBUTES securityAttributes;

	HKEY hKey = NULL;
	BOOL createOwnerWellKnownSid;
	BOOL createWorldWellKnownSid;
	BOOL setSecurityDescriptorDacl;
	DWORD setEntriesInAcl;

	if(!InitializeSecurityDescriptor(
		pSecurityDescriptor,
		SECURITY_DESCRIPTOR_REVISION
	)){
		_tprintf(_T("InitializeSecurityDescriptor Error %u\n"),
			GetLastError());
		goto Cleanup;
	}
	
	 createOwnerWellKnownSid = CreateWellKnownSid(
		WinCreatorOwnerSid,
		NULL,
		ownerSid,
		&cbOwnerSid
	);
	if (createOwnerWellKnownSid == FALSE) {
		std::cout << "CreateWellKnownSid() of owner Failure";
		goto Cleanup;

	}

	 createWorldWellKnownSid = CreateWellKnownSid(
		WinWorldSid,
		NULL,
		worldSid,
		&cbWorldSid
	);
	if (createWorldWellKnownSid == FALSE) {
		std::cout << "CreateWellKnownSid() of world Failure";
		goto Cleanup;
	}

		ZeroMemory(&pListOfExplicitEntries, 2 * sizeof(EXPLICIT_ACCESS));

		pListOfExplicitEntries[0].grfAccessPermissions = KEY_ALL_ACCESS;
		pListOfExplicitEntries[0].grfAccessMode = SET_ACCESS;
		pListOfExplicitEntries[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		pListOfExplicitEntries[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
		pListOfExplicitEntries[0].Trustee.TrusteeType = TRUSTEE_IS_USER;
		pListOfExplicitEntries[0].Trustee.ptstrName = (LPTSTR)ownerSid;
	
		pListOfExplicitEntries[1].grfAccessPermissions = KEY_READ;
		pListOfExplicitEntries[1].grfAccessMode = SET_ACCESS;
		pListOfExplicitEntries[1].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		pListOfExplicitEntries[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
		pListOfExplicitEntries[1].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		pListOfExplicitEntries[1].Trustee.ptstrName = (LPTSTR)worldSid;
	
		 setEntriesInAcl = SetEntriesInAcl(
			2,
			pListOfExplicitEntries,
			NULL,
			&NewAcl
		);
		if (setEntriesInAcl != 0) {
			std::cout << " SetEntriesInAcl() Failure";
			goto Cleanup;
		}

		 setSecurityDescriptorDacl = SetSecurityDescriptorDacl(
			pSecurityDescriptor,
			TRUE,
			NewAcl,
			FALSE
		);

		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.lpSecurityDescriptor = pSecurityDescriptor;
		securityAttributes.bInheritHandle = FALSE;
			
		if (RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			"SOFTWARE\\CSSO",
			0,
			0,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			&securityAttributes,
			&hKey,
			NULL
		) != ERROR_SUCCESS) {
			std::cout<<"RegCreateKeyEx() Failure";
		}

	Cleanup:

		if (ownerSid)
			LocalFree(ownerSid);
		if (worldSid)
			LocalFree(worldSid);
		if (NewAcl)
			LocalFree(NewAcl);
		if (pSecurityDescriptor)
			LocalFree(pSecurityDescriptor);
		if (hKey)
			RegCloseKey(hKey);

		return 0;
}
