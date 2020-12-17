#pragma comment(lib, "Wininet")
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
#include <wininet.h>
#include <cstring>
#include <string>
std::string global_path;
int global_pos;
int main()
{
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

	hInternetConnect = InternetConnect(
	    hInternetOpen,
		"raw.githubusercontent.com",
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
		"SpantuTheodor/Teme-CSSO/master/ex.txt",
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

	InternetCloseHandle(hHttpOpenRequest);
	InternetCloseHandle(hInternetConnect);

	char* pch;
	pch = strtok(buffer, "\n");
	int number_of_commands = std::stoi(pch);
	pch = strtok(NULL, "\n");
	std::string address = pch;
	pch = strtok(NULL, "\n");
	std::string username = pch;

	pch = strtok(NULL, "\n");
	std::string password = pch;

	int index = 0;
	std::string command;


	hInternetConnect = InternetConnect(
		hInternetOpen,
		address.c_str(),
		21,
		username.c_str(),
		password.c_str(),
		INTERNET_SERVICE_FTP,
		INTERNET_FLAG_PASSIVE,
		NULL
		);

	if (hInternetConnect == NULL)
	{
		std::cout << "InternetConnect() failure " << GetLastError() << "\n";
		return -1;
	}

	while (index < number_of_commands)
	{
		//printf("%s\n", pch);
		pch = strtok(NULL, " ");
		command = pch;

			if (command == "PUT") {
				pch = strtok(NULL, "\n");
				std::string path;
				std::string file_name;
				path = pch;
				global_path = path;
				int pos;
				pos = path.find("\\");
				while (pos != -1) {
					global_pos = global_pos + pos + 1;
					path.erase(0, pos + 1);
					pos = path.find("\\");
				}
				file_name = path;
				std::cout << file_name;
				std::cout << global_path;
				BOOL ftpPutFileA = FtpPutFileA(
					hInternetConnect,
					global_path.c_str(),
					file_name.c_str(),
					FTP_TRANSFER_TYPE_ASCII,
					NULL
				);


				if (ftpPutFileA == FALSE)
				{
					std::cout << "FtpPutFileA() failure " << GetLastError() << "\n";
					return -1;
				}

			}

			if (command == "RUN") {
				global_pos = 0;
				pch = strtok(NULL, "\n");
				std::string path;
				path = pch;


				std::string desktop = "C:\\Users\\Teo\\Desktop\\";
				BOOL ftpGetFileA = FtpGetFileA(
					hInternetConnect,
					path.c_str(),
					(desktop + path).c_str(),
					FALSE,
					FILE_ATTRIBUTE_NORMAL,
					FTP_TRANSFER_TYPE_ASCII,
					NULL
				);

				if (ftpGetFileA == FALSE)
				{
					std::cout << "FtpGetFileA() failure " << GetLastError() << "\n";
					return -1;
				}
				
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				BOOL createProcess = CreateProcess(
					(desktop + path).c_str(),
					NULL,
					NULL,
					NULL,
					FALSE,
					CREATE_NEW_CONSOLE,
					0,
					0,
					&si,
					&pi
				);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				if (createProcess == FALSE)
				{
					std::cout << "CreateProcess() failure " << GetLastError() << "\n";
					return -1;
				}



			}

		std::cout << command<< " ";
		index += 1;

	}

	InternetCloseHandle(hInternetConnect);
	InternetCloseHandle(hInternetOpen);

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
