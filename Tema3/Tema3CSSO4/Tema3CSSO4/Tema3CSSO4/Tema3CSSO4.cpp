#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <string.h>
#include <iterator>
#include <map>
#include <utility>
using namespace std;
#pragma comment(lib, "user32.lib")
#define BUF_SIZE 256

TCHAR szName[] = TEXT("BFileMappingObject");

int _tmain()
{
    HANDLE hMapFile;
    char* pBuf;
    HANDLE readEvent;
    HANDLE writeEvent;

    readEvent = OpenEvent(
        SYNCHRONIZE | EVENT_MODIFY_STATE,
        FALSE,
        "readEvent"
    );

    if (readEvent == NULL)
    {
        printf("OpenEvent error: %d\n", GetLastError());
        return 1;
    }

    writeEvent = OpenEvent(
        SYNCHRONIZE | EVENT_MODIFY_STATE,
        FALSE,
        "writeEvent"
    );

    if (writeEvent == NULL)
    {
        printf("OpenEvent error: %d\n", GetLastError());
        return 1;
    }

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        szName);               // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not open file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }

    pBuf = (char*)MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
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

    for (int i = 0; i < 250; i++) {
        if (WaitForSingleObject(readEvent, INFINITE) == 0) {
            char* p = strtok(pBuf, "|");
            char token_a[10];  //strtok(pBuf, "|");
            char token_b[10];//strtok(NULL, "|");
            strcpy(token_a, p);
            p = strtok(NULL, "|");
            strcpy(token_b, p);
            cout << "Al doilea proces: " << token_a << " " << token_b << endl;
            if (atoi(token_b) == 2 * atoi(token_a))
            {
                cout << "corect" << endl;
            }
            else { cout << "incorect" << endl; }
            SetEvent(writeEvent);
        }
    }

    CloseHandle(readEvent);

    CloseHandle(writeEvent);

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    return 0;
}