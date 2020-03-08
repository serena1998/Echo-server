// client.cpp : 定义控制台应用程序的入口点。
//

#include <winsock2.h>  
#include <iostream>  
#include <string.h>  
using namespace std;

#pragma comment(lib, "ws2_32.lib")          


const int DEFAULT_PORT = 8000;
int main(int argc, char* argv[])
{

	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err, iLen;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData); //load win socket  
	if (err != 0)
	{
		printf("WSAStartup failed with error: %d\n", err);
		return -1;
	}

	SOCKET sockClt = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClt == INVALID_SOCKET)
	{
		printf("socket() fail:%d\n", WSAGetLastError());
		return -2;
	}

	SOCKADDR_IN addrSrv;
	memset(&addrSrv, 0, sizeof(addrSrv));
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_port = htons(DEFAULT_PORT);

	err = connect(sockClt, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	if (err == INVALID_SOCKET)
	{
		printf("connect()fail:%d\n", WSAGetLastError());
		return -1;
	}

	char sendBuf[1024], hostname[1024];
	
	//err = send(sockClt, sendBuf, 1024, 0);
	char recvBuf[1024] = "\0";
	while(1){
		iLen = recv(sockClt, recvBuf, 1024, 0);

		if (iLen == 0)
		{
			return -3;
		}
		else if (iLen == SOCKET_ERROR)
		{
			printf("recv() fail:%d\n", WSAGetLastError());
			return -4;
		}
		else
		{
			recvBuf[iLen] = '\0';
			printf(recvBuf);
			printf("\n");
		}
	}
	closesocket(sockClt);

	WSACleanup();
	system("PAUSE");
	return 0;
}