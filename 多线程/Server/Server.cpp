// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream.h>
#include <WINSOCK2.H>
#pragma comment(lib, "wsock32.lib")
#include <string.h>

#define SERVER_EXIT_OK				0	//服务器正常退出
#define SERVER_DLL_REEOR			1	//调用Windows sockets DLL失败
#define SERVER_API_ERROR			2	//调用Windows sockets API失败
#define	SERVERPORT					5555//服务器TCP端口
#define MAX_NUM_BUF					64	//缓冲区最大尺寸

//变量
char	bufRecv[MAX_NUM_BUF];			//读缓冲区
char	bufSend[MAX_NUM_BUF];			//写缓冲区
SOCKET	sServer;						//服务器监听套接字
SOCKET	sClient;						//接受客户端套接字
BOOL	bConning;						//与客户端的连接状态

//函数
void	InitMember(void);				//初始化成员变量 
int		ExitClient(int nExit);			//客户端退出
BOOL	RecvLine(SOCKET s, char* buf);	//读取一行数据
BOOL	SendLine(SOCKET s, char* buf);	//发送一行数据
int		HandleSocketError(char *str);	//对Windows sockets API调用错误处理
void	ShowSocketMsg(char* str);		//显示错误信息
DWORD WINAPI Fun1Proc( LPVOID lpParameter );
int		ExitClient1(int nExit);
//主函数
int main(int argc, char* argv[])
{

	InitMember();					//初始化变量
	
	WORD	wVersionRequested;		//应用程序需要Windows sockets DLL的版本
	WSADATA	wsaData;				//Windows sockets DLL版本信息
	int		retVal;					//调用Windows sockets API返回值		
	//初始化Windows Sockets DLL
	wVersionRequested = MAKEWORD(1,1);
	retVal = WSAStartup(wVersionRequested, &wsaData);
	if ( 0 != retVal ) 
	{
		ShowSocketMsg("Can not find a usable Windows Sockets dll!");
		return SERVER_DLL_REEOR;
	}	
	//确保WinSock DLL支持1.1
	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1)
	{
		ShowSocketMsg("Can not find a usable Windows Sockets dll!");
		WSACleanup( );
		return SERVER_DLL_REEOR; 
	}
	
	
	//创建套接字
	sServer= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if(INVALID_SOCKET == sServer)
	{
		return HandleSocketError("Failed socket()!");
	}
	
	//服务器套接字地址 
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(SERVERPORT);
	addrServ.sin_addr.s_addr = INADDR_ANY;		
	//绑定套接字
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == retVal)
	{
		closesocket(sServer);						//关闭套接字
		return HandleSocketError("Failed bind()!");	//错误处理
	}
	
	//开始监听 
	retVal = listen(sServer, 1);
	if(SOCKET_ERROR == retVal)
	{
		closesocket(sServer);						//关闭套接字
		return HandleSocketError("Failed listen()!");//错误处理
	}
	

	//等待客户端的连接	
	cout << "Server succeeded!" << endl;
	cout << "Waiting for new clients..." << endl;

		/*if (!RecvLine(sClient, bufRecv))
		{
			return	ExitClient(SERVER_API_ERROR);//退出
		}	
		//显示客户端数据
		cout << bufRecv<<endl;*/


		//向客户端发送数据
		/*strcpy(bufSend, "Hello,Client!\n");
		if (!SendLine(sClient, bufSend))
		{
			return	ExitClient(SERVER_API_ERROR);
		}*/
	while (1)
	{
		//接受客户端请求
		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);
		SOCKET sockConn = accept(sServer,(sockaddr FAR*)&addrClient, &addrClientlen);
		char *pClientIP = inet_ntoa(addrClient.sin_addr);
		u_short  clientPort = ntohs(addrClient.sin_port);	
		cout<<"Accept a client."<<endl;
		cout<<"IP: "<<pClientIP<<endl;
		cout<<"Port: "<<clientPort<<endl;

//		char sendBuf[1024], hostname[100];
		/*if (gethostname(hostname, 100) != 0)
		{
			strcpy_s(hostname, strlen("None") + 1, "None");
		}
		sprintf_s(sendBuf, sizeof(sendBuf), "Welecome %s connected to %s!", inet_ntoa(addrClt.sin_addr), hostname);
		if(INVALID_SOCKET == sClient)
		{
			closesocket(sServer);							//关闭套接字
			return HandleSocketError("Failed accept()!");	//错误处理
		}else{
			bConning = TRUE;								//客户端请求成功
		}*/
		
		//显示客户端的IP和端口

		HANDLE hThread1;
		hThread1 = CreateThread(NULL, 0, Fun1Proc,&sockConn, 0, NULL);
	}
	

	//显示退出信息
	cout << "Server exiting..." << endl;
	
	//退出
	return ExitClient(SERVER_EXIT_OK);
}



/*
 *	初始化成员变量
 */
void	InitMember(void)
{
	//初始化读和写缓冲区
	memset(bufRecv, 0, MAX_NUM_BUF);
	memset(bufSend, 0, MAX_NUM_BUF);

	//初始化
	sServer = INVALID_SOCKET;
	sClient = INVALID_SOCKET;

	//没有连接状态
	bConning = FALSE;
}


/*
 *	退出
 */
int		ExitClient(int nExit)
{
	closesocket(sServer);	//关闭监听套接字
	closesocket(sClient);	//关闭连接客户端套接接
	WSACleanup();			//卸载Windows sockets DLL 清理内存
	return nExit;			//退出
}
int		ExitClient1(int nExit)
{
	closesocket(sServer);	//关闭监听套接字
	closesocket(sClient);	//关闭连接客户端套接接
	WSACleanup();			//卸载Windows sockets DLL 清理内存
	return nExit;			//退出
}
/*
 *	读一行数据
 */
BOOL	RecvLine(SOCKET s, char* buf)
{
	BOOL	retVal = TRUE;			//返回值
	BOOL	bLineEnd = FALSE;		//行结束
	int		nReadLen = 0;			//读入字节数
	int		nDataLen = 0;			//数据长度
	while (!bLineEnd && bConning)	//与客户端连接 没有换行
	{
		nReadLen = recv(s, buf + nDataLen, 1, 0);//每次接收一个字节
		
		//错误处理
		if (SOCKET_ERROR == nReadLen)
		{
			int nErrCode = WSAGetLastError();//错误代码
			if (WSAENOTCONN == nErrCode)
			{
				ShowSocketMsg("The socket is not connected!");
				
			}else if(WSAESHUTDOWN == nErrCode)
			{
				ShowSocketMsg("The socket has been shut down!");
				
			}else if (WSAETIMEDOUT == nErrCode)
			{
				ShowSocketMsg("The connection has been dropped!");							
			}else if (WSAECONNRESET == nErrCode)
			{
				ShowSocketMsg("The virtual circuit was reset by the remote side!");
			}else{}	
			
			retVal = FALSE;	//读数据失败
			break;			//跳出循环						
		}
		
		
		if (0 == nReadLen)//客户端关闭
		{
			retVal = FALSE;	//读数据失败
			break ;			//跳出循环			
		}
		
		//读入数据
		if ('\n' == *(buf + nDataLen))	//换行符
		{
			bLineEnd = TRUE;			//接收数据结束
		}else{
			nDataLen += nReadLen;		//增加数据长度
		}	
	}
	
	return retVal;
}

/*
 *	//发送一行数据
 */
BOOL	SendLine(SOCKET s, char* str)
{
	int retVal;//返回值
	retVal = send(s, str, strlen(str), 0);//一次发送

	//错误处理
	if (SOCKET_ERROR == retVal)
	{
		int nErrCode = WSAGetLastError();//错误代码
		if (WSAENOTCONN == nErrCode)
		{
			ShowSocketMsg("The socket is not connected!");
			
		}else if(WSAESHUTDOWN == nErrCode)
		{
			ShowSocketMsg("The socket has been shut down!");
			
		}else if (WSAETIMEDOUT == nErrCode)
		{
			ShowSocketMsg("The connection has been dropped!");
		}else{}	
		
		return FALSE;	//发送失败
	}
	
	return TRUE;		//发送成功
}
	
/*
 *	错误处理
 */
int		HandleSocketError(char *str)
{
	ShowSocketMsg(str);		//显示错误消息	
	WSACleanup();			//卸载Windows socket DLL	
	return SERVER_API_ERROR;//退出应用程序
}

/*
 *	显示错误
 */
void	ShowSocketMsg(char* str)
{
	MessageBox(NULL, str, "SERVER ERROR", MB_OK);
}
DWORD WINAPI Fun1Proc(
	LPVOID lpParameter   // thread data
)
{

	//int     err, iLen;
	SOCKET sClient = *((SOCKET*)lpParameter);

	char bufRecv1[1024] = "\0";
	char bufSend1[1024] = "\0";
	char anc[1024]="\0";
	char temp[1024]="\0";
	while (1)
	{
		memset(bufSend1, 0, MAX_NUM_BUF);
		memset(bufRecv1, 0, MAX_NUM_BUF);
		int cd = recv(sClient, bufRecv1, MAX_NUM_BUF, 0);	//接收客户端数据
		if (!cd)
		{
			return	closesocket(sClient);;//退出
		}
		//显示客户端数据
		strcpy(anc,bufRecv1);
		//sprintf(temp, "%d", sClient);
		//strcat(anc,temp);
	    //cout << anc << endl;
		printf("get %s from socket  %d\n", anc, sClient);

		//向客户端发送数据
		if (bufRecv1[cd] == '\n')
		{
			bufRecv1[cd] = 'e';
			strcat(bufRecv, "cho\n");
		}
		else
		{
			strcat(bufRecv1, "echo\n");
		}
		bufRecv[strlen(bufRecv1)] = '\0';
		strcpy(bufSend1, bufRecv1);
		send(sClient, bufSend1, strlen(bufSend1), 0);
	}


}
	