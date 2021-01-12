#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 3008

using namespace std;

int main(int argc, char* argv[])
{

	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	sockaddr_in server;

	if (argc > 4)
	{
		cout << "Numar argumente prea mare \n";
		return -1;
	}

	int wSAStartup = WSAStartup(version, &data);
	if (wSAStartup != 0)
	{
		cout << "WSAStarturp() Failure \n" << wSAStartup << endl;
		return 0;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

	SOCKET sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET) {
		cout << "socket() Failure \n";
		WSACleanup();
		return 0;
	}
	
	char command[100];
	char filename[100];
	char content[100];
	strcpy(command, argv[1]);
	strcpy(filename, argv[2]);
	if (argc == 4) {
		strcpy(content, argv[3]);
	}
	else {
		strcpy(content, "");
	}

	if(sendto(sd, command, sizeof(command), 0, (struct sockaddr*)&server, sizeof(server)) <= 0)
	{
		cout << "sendto() Failure \n";
		return 0;
	}

	if (sendto(sd, filename, sizeof(filename), 0, (struct sockaddr*)&server, sizeof(server)) <= 0)
	{
		cout << "sendto() Failure \n";
		return 0;
	}

	if (sendto(sd, content, sizeof(content), 0, (struct sockaddr*)&server, sizeof(server)) <= 0)
	{
		cout << "sendto() Failure \n";
		return 0;
	}
	closesocket(sd);

	WSACleanup();

}