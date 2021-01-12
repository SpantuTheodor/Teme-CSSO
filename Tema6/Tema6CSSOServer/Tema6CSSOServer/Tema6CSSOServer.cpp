#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
//#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 3008

using namespace std;

int main()
{
    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    sockaddr_in server;
    ZeroMemory(&server, sizeof(server));

    int wSAStartup = WSAStartup(version, &data);
    if (wSAStartup != 0)
    {
        cout << "WSAStarturp() Failure \n" << wSAStartup << endl;
        return 0;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    
    SOCKET sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd == INVALID_SOCKET) {
        cout << "socket() Failure \n";
        WSACleanup();
        return 0;
    }

    if (bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("bind() Failure \n");
        return errno;
    }

    while (true) {
        char command[100]; 
        char filename[100];
        char content[100];
        char error_message[100];
        sockaddr_in client;
        int clientLength = sizeof(client);
        ZeroMemory(&client, clientLength);
        
        if(recvfrom(sd, command, sizeof(command), 0, (struct sockaddr*)&client, &clientLength) <= 0)
        {
            cout << "recvfrom() Failure \n";
            return -1;
        }

        if (recvfrom(sd, filename, sizeof(filename), 0, (struct sockaddr*)&client, &clientLength) <= 0)
        {
            cout << "recvfrom() Failure \n";
            return -1;
        }

        if (recvfrom(sd, content, sizeof(content), 0, (struct sockaddr*)&client, &clientLength) <= 0)
        {
            cout << "recvfrom() Failure \n";
            return -1;
        }

        cout << "bine";

        if (strcmp(command, "createfile") == 0)
        {
            if (strlen(content) == 0) {
                HANDLE hFile;
                hFile = CreateFile(
                    filename,           
                    GENERIC_WRITE,      
                    0,                  
                    NULL,               
                    CREATE_NEW,             
                    FILE_ATTRIBUTE_NORMAL,  
                    NULL);                  

                if (hFile == INVALID_HANDLE_VALUE)
                {
                    cout << "CreateFile() Failure \n";
                    return -1;
                }
                else {
                    cout << "createfile succes \n";
                }
                CloseHandle(hFile);
            } else {
                cout << "Prea multe argumente \n";
            }
        
        
        } else if (strcmp(command, "appendfile") == 0) {
            HANDLE hFile;
            DWORD lpNumberOfBytesWritten = 0;

            hFile = CreateFile(
                filename,               
                FILE_APPEND_DATA,       
                0,                      
                NULL,                   
                OPEN_ALWAYS,             
                FILE_ATTRIBUTE_NORMAL,  
                NULL);                  

            if (hFile == INVALID_HANDLE_VALUE)
            {
                cout << "CreateFile() Failure \n";
                return -1;
            }
            cout << content;
            string auxiliary;
            auxiliary = content;
            if (WriteFile(
                hFile,
                auxiliary.c_str(),
                auxiliary.size(),
                &lpNumberOfBytesWritten,
                NULL
            ) == FALSE) {
                cout << "WriteFile() Failure \n";
                return -1;
            }
            cout << "appendfile succes \n";
            CloseHandle(hFile);
        
        
        } else if (strcmp(command, "deletefile") == 0) {
            if (strlen(content) == 0) {
                if (DeleteFile(
                    filename
                ) == 0) {
                    cout << "DeleteFile() Failure \n";
                    return -1;
                }
                else
                    cout << "deletefile succes \n";
            } else {
                cout << "Prea multe argumente \n";
            }
        
        
        } else if (strcmp(command, "createkey") == 0) {
            HKEY hKey = NULL;
            string auxiliary = TEXT(filename);
            auxiliary.insert(0, TEXT("SOFTWARE\\CSSO\\"));
            cout << auxiliary;
            
            if (RegCreateKeyEx(
                HKEY_LOCAL_MACHINE,
                auxiliary.c_str(),
                0,
                0,
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS,
                NULL,
                &hKey,
                NULL
            ) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
            } else {
                printf("RegCreateKeyEx error: %d\n", GetLastError());
                return -1;
            }
        }
    }
}

