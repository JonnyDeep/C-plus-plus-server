//

//#undef UNICODE	

#define WIN32_LEAN_AND_MEAN	

#include<Windows.h>
#include<WinSock2.h>
#include <ws2tcpip.h>
#include<stdlib.h>
#include<stdio.h>


#pragma comment(lib,"Ws2_32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "20000"

void error(const char* msg)
{
	perror(msg);
	exit(0);
}
int main()
{
	int iResult;
	WSADATA	wsaData;

	SOCKET LINSTENSOCKET;
	SOCKET ClientSocket;

	struct addrinfo *result = NULL;
	struct addrinfo hints;


	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		error("WSAStarup failed");
	}


	ZeroMemory(&hints, sizeof(hints));

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);


	if (iResult!=0)
	{
		printf("getaddrinfo failed");
		WSACleanup();
		return 1;
	}
	printf("create socket...\n");
	LINSTENSOCKET =  socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (LINSTENSOCKET == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	printf("bind socket...\n");
	iResult = bind(LINSTENSOCKET, result->ai_addr, result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(LINSTENSOCKET);
		WSACleanup();
		return 1;
	}

	printf("listening...\n");
	iResult = listen(LINSTENSOCKET, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(LINSTENSOCKET);
		WSACleanup();
		return 1;
	}

	printf("get connection\n");
	ClientSocket = accept(LINSTENSOCKET, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(LINSTENSOCKET);
		WSACleanup();
		return 1;
	}

	closesocket(LINSTENSOCKET);
	do {
		iResult = recv(ClientSocket, recvbuf,recvbuflen,0);
		if (iResult > 0)
		{
			printf("received bytes:%d\n", iResult);

			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed:%d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("send bytes:%d\n", iSendResult);
		}
		else if(iResult==0) {
			printf("connection closing...\n");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ClientSocket);
	WSACleanup();


	getchar();
	return 0;
}
