
#include <winsock2.h>  
#include <iostream>  

#include <string.h>  
#include <time.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")          //add ws2_32.lib  


const int DEFAULT_PORT = 8000;
int main(int argc, char* argv[])
{

	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err, iLen;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		printf("WSAStartup failed with error: %d\n", err);
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return 1;
	}
	else
	{
		printf("The Winsock 2.2 dll was found okay\n");
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	if (sockSrv == INVALID_SOCKET)
	{
		printf("socket() fail:%d\n", WSAGetLastError());
		return -2;
	}

	SOCKADDR_IN addrSrv;
	memset(&addrSrv, 0, sizeof(addrSrv));
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
	addrSrv.sin_port = htons(DEFAULT_PORT);

	err = bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	if (err != 0)
	{
		printf("bind()fail:%d\n", WSAGetLastError());
		return -3;
	}

	err = listen(sockSrv, 5);//listen函数的第一个参数即为要监听的socket描述字，第二个参数为相应socket可以排队的最大连接个数
	if (err != 0)
	{
		printf("listen()fail:%d\n", WSAGetLastError());
		return -4;
	}
	printf("Server waitting...!!!!!\n");
	SOCKADDR_IN addrClt;
	int len = sizeof(SOCKADDR);
	memset(&addrClt, 0, len);

	while (1)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClt, &len);
		char sendBuf[1024], hostname[100];
		if (gethostname(hostname, 100) != 0)
		{
			strcpy_s(hostname, strlen("None") + 1, "None");
		}
		sprintf_s(sendBuf, sizeof(sendBuf), "Welecome %s connected to %s!", inet_ntoa(addrClt.sin_addr), hostname);

		err = send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		time_t curtime;
		time(&curtime);
		sprintf_s(sendBuf,sizeof(sendBuf),"当前时间 = %s", ctime(&curtime));
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		int i = 1;
		char recvBuf[512];
		do
		{
			iLen = recv(sockConn, recvBuf, 512, 0);
			if (iLen > 0)
			{
				printf("the %d time receive %d bytes\n", i, iLen);
				puts(recvBuf);
				//printf("the %d \n", recvBuf[4]);
				memset(recvBuf, sizeof(recvBuf), 0 );
				i++;
			}
		} while (iLen > 0);


		//recvBuf[iLen] = '\0';
		//printf(recvBuf);

		closesocket(sockConn);
	}

	closesocket(sockSrv);

	WSACleanup();
	return 0;
}