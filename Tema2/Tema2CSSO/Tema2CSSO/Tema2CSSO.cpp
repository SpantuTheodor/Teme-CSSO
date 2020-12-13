// Tema2CSSO.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
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
#include <sstream>
#include <typeinfo>



#define BUF_SIZE 10000
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

int _tmain()
{

	PROCESSENTRY32 pe32;
	HANDLE hProcessSnap;

	HANDLE hMapFile;
	LPCTSTR pBuf;

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
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}


	//cer un snapshot la procese
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed.err = %d \n", GetLastError());
		return(-1);
	}
	//initializez dwSize cu dimensiunea structurii.
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//obtin informatii despre primul proces
	if (!Process32First(hProcessSnap, &pe32))
	{
		printf("Process32First failed. err = %d \n", GetLastError());
		CloseHandle(hProcessSnap); //inchidem snapshot-ul
		return(-1);
	}
	std::string s;
	do
	{
		//afisez pid-ul si executabilul
		std::stringstream ss;
		ss << pe32.th32ProcessID << '|' << pe32.th32ParentProcessID << '|' << pe32.szExeFile << '|';
		s = s + ss.str();
	} while (Process32Next(hProcessSnap, &pe32)); //trec la urmatorul proces
	//inched handle-ul catre snapshot

	char* szMsg = new char[s.length() + 1];
	strcpy(szMsg, s.c_str());
	std::cout << typeid((szMsg)).name();
	//CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));


	_getch();

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
