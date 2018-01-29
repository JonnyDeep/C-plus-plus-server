#define WIN32_LEAN_AND_MEAN	

#include<Windows.h>
#include<WinSock2.h>
#include <ws2tcpip.h>
#include<stdlib.h>
#include<stdio.h>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


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

	WSADATA wsaData;


	SOCKET ConnectSocket= INVALID_SOCKET;
	struct addrinfo *result=NULL;
	struct addrinfo hints;


	const char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		error("WSAStarup failed");
	}

	

	ZeroMemory(&hints,sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	iResult=getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	
	if (iResult != 0)
	{
		printf("getaddrinfo error:%d\n", iResult);
		
		WSACleanup();

		return 1;
	}

	if (iResult==0)
	{
		ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("creat socket fail;%d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			
		}
	}




	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());
	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();
	getchar();
	return 0;


}