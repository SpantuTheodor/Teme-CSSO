#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <string.h>
#include <wchar.h>
#include <strsafe.h>
#include<tlhelp32.h>
#include <string>
#include <conio.h>
#include <typeinfo>

#define BUF_SIZE 256
TCHAR szName[] = TEXT("AFileMappingObject");
using namespace std;
int _tmain()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	HANDLE hMapFile;
	LPCTSTR pBuf;
	HANDLE ghMutex;

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		"muxet");             // unnamed mutex

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		0);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}


	if (!CreateProcess("D:\\[Faculty]\\III1\\[CSSO]\\Tema3\\Tema3CSSO2\\Debug\\Tema3CSSO2.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		printf("Cannot create process.\n");
		return 0;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	int a,b;
	std::string a_string;
	std::string b_string;
	char* szMsg;
	srand(time(NULL));
	for (int i = 0; i < 250; i++) {

		if (WaitForSingleObject(ghMutex, INFINITE) == 0) {
			a = rand();
			b = a * 2;
			a_string = std::to_string(a);
			b_string = std::to_string(b);
			szMsg = new char[a_string.length() + b_string.length() + 1];
			strcpy(szMsg, a_string.c_str());
			strcat(szMsg, "|");
			strcat(szMsg, b_string.c_str());
			strcat(szMsg, "|");
			std::cout << "Primul proces: " << szMsg << " " << endl;
			CopyMemory((PVOID)pBuf, szMsg, BUF_SIZE);
			ReleaseMutex(ghMutex);
		}
	}
	_getch();

	CloseHandle(ghMutex);

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}