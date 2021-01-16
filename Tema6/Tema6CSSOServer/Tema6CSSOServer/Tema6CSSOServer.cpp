#pragma comment(lib, "Wininet")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
//#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <wininet.h>
#include <strsafe.h>
#include <tchar.h> 
#include <stdio.h>


#pragma comment(lib, "Ws2_32.lib")

#define PORT 3008

using namespace std;

DWORD WINAPI choosefunction(LPVOID lpParameter);
int createfile();
int appendfile();
int deletefile();
int createkey();
int deletekey();
int run();
int httpdownload();
bool listdir(LPCTSTR lpcszFolder, int nLevel);

char command[100];
char filename[100];
char content[100];
int index;
HANDLE  hThreadArray[10];

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

        index++;

        hThreadArray[index] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            choosefunction,       // thread function name
            NULL,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier 


        // Check the return value for success.
        // If CreateThread fails, terminate execution. 
        // This will automatically clean up threads and memory. 

        if (hThreadArray[index] == NULL)
        {
            cout << "CreateThread() Failure \n";
            return -1;
        }
        else
        {
            cout << "Thread number " << index << " created. \n";
        }
    }
}


// -------------------------------------------


DWORD WINAPI choosefunction(LPVOID lpParameter) {
    
    Sleep(1000);

    if (strcmp(command, "createfile") == 0)
    {
        return createfile();
    }
    else if (strcmp(command, "appendfile") == 0)
    {
        return appendfile();
    }
    else if (strcmp(command, "deletefile") == 0)
    {
        return deletefile();
    }
    else if (strcmp(command, "createkey") == 0)
    {
        return createkey();
    }
    else if (strcmp(command, "deletekey") == 0)
    {
        return deletekey();
    }
    else if (strcmp(command, "run") == 0)
    {
        return run();
    }
    else if (strcmp(command, "httpdownload") == 0)
    {
        return httpdownload();
    }
    else if (strcmp(command, "listdir") == 0)
    {
        return listdir(filename, 0);
    }

}


int createfile() {
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
            CloseHandle(hFile);
            return 1;
        }

    }
    else {
        cout << "Prea multe argumente \n";
        return -1;
    }

}


int appendfile() {
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
        ExitThread(-1);
    }
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
        ExitThread(-1);
    }
    else {
        cout << "appendfile succes \n";
        CloseHandle(hFile);
        ExitThread(1);
    }
}


int deletefile() {

    if (strlen(content) == 0) {
        if (DeleteFile(
            filename
        ) == 0) {
            cout << "DeleteFile() Failure \n";
            ExitThread(-1);
        }
        else {
            cout << "deletefile succes \n";
            ExitThread(-1);
        }
    }
    else {
        cout << "Prea multe argumente \n";
        ExitThread(-1);
    }

}


int createkey() {

    HKEY hKey = NULL;
    string auxiliary = TEXT(filename);
    auxiliary.insert(0, TEXT("SOFTWARE\\CSSO\\"));

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
        cout << "createkey succes \n";
        RegCloseKey(hKey);
        ExitThread(1);
    }
    else {
        cout << "RegCreateKeyEx() Failure \n";
        ExitThread(-1);
    }

}


int deletekey() {

    HKEY hKey = NULL;
    string path;
    string subkey = filename;
    path = TEXT("SOFTWARE\\CSSO\\");
    if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        path.c_str(),
        0,
        KEY_ALL_ACCESS,
        &hKey)) {

        if (!RegDeleteKey(hKey, subkey.c_str())) {
            RegCloseKey(hKey);
            cout << "deletekey succes \n";
            ExitThread(1);
        }
        else {
            RegCloseKey(hKey);
            cout << "RegDeleteKey() Failure \n";
            ExitThread(-1);
        }

    }
    else {
        cout << "RegOpenKeyEx() Failure \n";
        ExitThread(-1);
    }

}


int run() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    string path = filename;

    // Start the child process. 
    if (!CreateProcess(path.c_str(),
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi)
        )
    {
        cout << "CreateProcess() Failure \n";
        ExitThread(-1);
    }

    cout << "run succes \n";
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    ExitThread(1);
}


int httpdownload() {
    HINTERNET hInternetOpen;
    HINTERNET hInternetConnect;
    const char* lplpszAcceptTypes[] = { "text/*", NULL };
    HINTERNET hHttpOpenRequest;
    BOOL httpSendRequest;
    BOOL internetReadFile;

    hInternetOpen = InternetOpen(
        "Client",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        NULL
    );

    if (hInternetOpen == NULL)
    {
        std::cout << "InternetOpen() failure " << GetLastError() << "\n";
        return -1;
    }

    string domain = strtok(filename, "/");
    string path = strtok(NULL, " ");

    hInternetConnect = InternetConnect(
        hInternetOpen,
        domain.c_str(),
        443,
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        NULL,
        NULL
    );


    if (hInternetConnect == NULL)
    {
        std::cout << "InternetConnect() failure " << GetLastError() << "\n";
        return -1;
    }

    hHttpOpenRequest = HttpOpenRequest(
        hInternetConnect,
        "GET",
        path.c_str(),
        "1.1",
        NULL,
        lplpszAcceptTypes,
        INTERNET_FLAG_SECURE,
        NULL
    );


    if (hHttpOpenRequest == NULL)
    {
        std::cout << "HttpOpenRequest() failure " << GetLastError() << "\n";
        return -1;
    }

    httpSendRequest = HttpSendRequest(
        hHttpOpenRequest,
        NULL,
        NULL,
        NULL,
        NULL
    );


    if (httpSendRequest == FALSE)
    {
        std::cout << "HttpSendRequest() failure " << GetLastError() << "\n";
        return -1;
    }

    char buffer[10000] = {};
    DWORD lpdwNumberOfBytesRead;

    internetReadFile = InternetReadFile(
        hHttpOpenRequest,
        buffer,
        10000,
        &lpdwNumberOfBytesRead
    );

    if (httpSendRequest == FALSE)
    {
        std::cout << "InternetReadFile() failure " << GetLastError() << "\n";
        return -1;
    }

    strcpy(filename, "download.txt");
    strcpy(content, buffer);
    appendfile();
    cout << "httpdownload succes \n";
    InternetCloseHandle(hHttpOpenRequest);
    InternetCloseHandle(hInternetConnect);

}

//Reference https://www.codeproject.com/Articles/93141/Iterative-Implementation-of-Recursively-Enumeratin
bool listdir(LPCTSTR lpcszFolder, int nLevel = 0) 
    {
        WIN32_FIND_DATA ffd;
        TCHAR szDir[MAX_PATH];
        HANDLE hFind = INVALID_HANDLE_VALUE;

        StringCchCopy(szDir, MAX_PATH, lpcszFolder);
        StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

        hFind = FindFirstFile(szDir, &ffd);

        if (INVALID_HANDLE_VALUE == hFind)
        {
            return false;
        }

        TCHAR szOutLine[MAX_PATH] = { 0 };
        for (int ii = 0; ii < nLevel; ++ii)
            szOutLine[ii] = _T('\t');

        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (_T('.') != ffd.cFileName[0])
                {
                    _tprintf(TEXT("%s%s   <DIR>\n"),
                        szOutLine, ffd.cFileName);
                    StringCchCopy(szDir + _tcslen(lpcszFolder) + 1,
                        MAX_PATH, ffd.cFileName);
                    listdir(szDir, nLevel + 1);
                }
            }
            else
            {
                _tprintf(TEXT("%s%s\n"), szOutLine, ffd.cFileName);
            }
        } while (FindNextFile(hFind, &ffd) != 0);

        FindClose(hFind);
        cout << "listdir succes \n";
        return true;
    }