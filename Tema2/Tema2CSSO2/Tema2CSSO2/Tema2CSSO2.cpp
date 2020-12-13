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
#define BUF_SIZE 10000

TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
int vector_caracteristic[10000];

void TreeOfProcesses(string parent_token, char* pBuf, int number_of_spaces) {

    char* context = 0;
    char aux_pBuf[10000];
    char aux_pBuf2[10000];
    strcpy(aux_pBuf, pBuf);
    char* aux;
    char* child_token = strtok_s(pBuf, "|", &context); 
    int index = 1;
    aux = child_token;

    while (child_token != NULL) {
        
        if (index % 3 == 2) {
        
            if ((child_token == parent_token) && (index != 2)) {

                vector_caracteristic[index - 1] = 1;
                strcpy(aux_pBuf2, aux_pBuf);
                index++;
                child_token = strtok_s(NULL, "|", &context);
                std::string stuff(number_of_spaces, '.');
                cout << stuff << aux << " " << child_token << endl;
                TreeOfProcesses(aux, aux_pBuf2, number_of_spaces + 1);

            }

        }
 
        aux = child_token;
        index++;
        child_token = strtok_s(NULL, "|", &context);
        //cout << child_token << " " << index << " " << parent_token << endl;


    }

}

BOOL EnablePrivilege()
{
    LUID PrivilegeRequired;
    BOOL bRes = FALSE;

    bRes = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &PrivilegeRequired);

    return bRes;
}

int _tmain()
{

    if (EnablePrivilege() == true)
    {
        HANDLE hMapFile;
        char* pBuf;

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

        pBuf = (char* )MapViewOfFile(hMapFile, // handle to map object
            FILE_MAP_ALL_ACCESS,  // read/write permission
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
        char aux_pBuf[10000];
        char aux_pBuf2[10000];
        strcpy(aux_pBuf, pBuf);
        char* context2 = 0;
        char* token = strtok_s(pBuf, "|", &context2);
        char* aux;
        int index = 1;
        aux = token;
        int index_arbore = 1;

        while (token != NULL) {
            strcpy(aux_pBuf2, aux_pBuf);
            if (index % 3 == 1) {
                if (vector_caracteristic[index] != 1) {
                    cout << "[Arbore nr. " << index_arbore << "]" << endl;
                    index_arbore++;
                    vector_caracteristic[index] = 1;
                    token = strtok_s(NULL, "|", &context2);
                    token = strtok_s(NULL, "|", &context2);
                    index += 2;
                    cout << aux << " " << token << endl;
                    TreeOfProcesses(aux, aux_pBuf2, 1);
                    cout << endl;

                }
            }
            index++;
            token = strtok_s(NULL, "|", &context2);
            aux = token;

        }

        DWORD dwProcessId;
        LPDWORD lpExitCode;
        std::cin >> dwProcessId;
        printf("%d", dwProcessId);
     
        DWORD dwDesiredAccess = PROCESS_TERMINATE;
        BOOL  bInheritHandle = FALSE;
        HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
        if (hProcess == NULL)
            return FALSE;
        
        //lpExitCode = GetExitCodeProcess(hProcess, lpExitCode);

        BOOL result = TerminateProcess(hProcess, NULL);

        CloseHandle(hProcess);
        
        cout << result;

        UnmapViewOfFile(pBuf);

        CloseHandle(hMapFile);

        return 0;
    }
    else {
        _tprintf(TEXT("Could not set privilege to SE_DEBUG_NAME.\n"));
    }
}