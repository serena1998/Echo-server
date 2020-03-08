// Client.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"

#include <windows.h>
#include <winsock.h>
#include <iostream.h>
#include <string.h>
#pragma comment(lib, "wsock32.lib")


#define CLIENT_EXIT_OK				0	//客户端正常退出
#define CLIENT_DLL_REEOR			1	//调用Windows socket dll失败
#define CLIENT_API_ERROR			2	//调用Windows socket api失败
#define MAX_NUM_BUF					64	//缓冲区的最大长度
#define	SERVERPORT					5555//服务器TCP端口


//变量
char	bufRecv[MAX_NUM_BUF];			//读缓冲区
char	bufSend[MAX_NUM_BUF];			//写缓冲区
char	buff1[MAX_NUM_BUF];
SOCKET	sHost;							//socket
BOOL	bConning;						//连接服务器状态

//函数
void	InitMember(void);				//初始化变量
int		ExitClient(int nExit);			//退出
//BOOL	RecvLine(SOCKET s, char* buf);	//读取一行数据
void	ShowErrorMsg(void);				//显示错误信息
#include <stdio.h>
#include <stdlib.h>
#define MAXLINE 100
static int	read_cnt;
static char	*read_ptr;
static char	read_buf[MAXLINE];
#include <windows.h>
#include <winsock.h>
#include <string.h>
#pragma comment(lib, "wsock32.lib")
int my_reada(int fd, char *ptr)
{

	while(read_cnt <= 0)
    {
        if ( (read_cnt = recv(fd, read_buf, sizeof(read_buf),0)) < 0)
		{
			//if (errno == EINTR)goto again;
			continue;
        }
        else if (read_cnt == 0)
			return(0);
        else
        {
            read_ptr = read_buf;
            break;
        }
		
	}
	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

int readlinea(int fd, char *vptr, int maxlen)
{
	int	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++)
    {
		if ( (rc = my_reada(fd, &c)) == 1)
		{*ptr++ = c;
            if (c == '\n') break;	/* newline is stored, like fgets() */}
        else if (rc == 0)
        {
			*ptr = 0;return(n - 1);	/* EOF, n - 1 bytes were read */}
        else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

int readlinebuf(char **vptrptr)
{
	if (read_cnt)
		*vptrptr = read_ptr;
	return(read_cnt);
}

int Readlinea(int fd, char *ptr, int maxlen)
{
	int		n;

	if ( (n = readlinea(fd, ptr, maxlen)) < 0)
		printf("error in readline\n");
	return(n);
}

//主函数
int main()
{

	//初始化变量
	InitMember();

	WORD	wVersionRequested;		//应用程序需要Windows sockets DLL的版本
	WSADATA	wsaData;				//Windows sockets DLL版本信息
	int		retVal;					//调用Windows sockets API返回值

	//初始化Windows Sockets DLL
	wVersionRequested = MAKEWORD(1,1);	
	retVal = WSAStartup(wVersionRequested,(LPWSADATA)&wsaData);
	if ( 0 != retVal ) 
	{
		MessageBox(NULL, "Can not find a usable Windows Sockets dll!", "ERROR", MB_OK);
		return CLIENT_DLL_REEOR;
	}


	//创建Windows socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sHost)
	{
		ShowErrorMsg();			//显示错误信息
		WSACleanup();			//释放资源
		return CLIENT_API_ERROR;//退出
	}

	//准备连接服务器
	cout << "Client succeeded!" << endl;
	cout<<"Be ready to connect to server..."<<endl;

	
	
	//获取主机的信息
	LPHOSTENT hostEntry;
	char hostname[MAX_NUM_BUF];
	gethostname(hostname,MAX_NUM_BUF);			//获取主机名称
	hostEntry = gethostbyname(hostname);		//获取主机信息
	if(!hostEntry)
	{
		ShowErrorMsg();							//显示错误信息
		return ExitClient(CLIENT_API_ERROR);	//退出
	}
	//设置sockaddr_in
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	addrServ.sin_port = htons(SERVERPORT);
	//连接服务器
	retVal=connect(sHost,(LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == retVal)
	{
		ShowErrorMsg();							//显示错误信息
		return ExitClient(CLIENT_API_ERROR);	//退出
	}else{
		bConning = TRUE;						//连接服务器成功
	}     
	//连接服务器成功
	cout<<"Connect successfully!"<<endl;


	//向服务器发送数据
	//strcpy(bufSend, "Hello, Server!\n");
	
	//retVal = send(sHost, bufSend, strlen(bufSend), 0);
	//strcpy(bufSend, "Hello, Server!");
	if (SOCKET_ERROR == retVal)
	{
		ShowErrorMsg();							//显示错误信息
		return ExitClient(CLIENT_API_ERROR);	//退出
	}
			/*if (!RecvLine(sHost, bufRecv))
		{
			ShowErrorMsg();							//显示错误信息
			return ExitClient(CLIENT_API_ERROR);//退出
				//显示服务器的应答
		}
		cout<<bufRecv<<endl;*/
	while(1)
	{
		
		puts("请输入要发送的字符：");
		memset(bufSend, 0, MAX_NUM_BUF);
		memset(bufRecv, 0, MAX_NUM_BUF);
		gets(bufSend);
		if(strlen(bufSend)==1 && bufSend[0] =='q')
			break;
		//cout<<strlen(buf)<<endl;
		//strcpy(bufSend, buff1);
		//cout<<strlen(bufSend)<<endl;
		retVal = send(sHost, bufSend, strlen(bufSend), 0);
		//从服务器接收数据
		if (readlinea(sHost, bufRecv,sizeof(bufRecv))<=0)
		{
			ShowErrorMsg();							//显示错误信息
			return ExitClient(CLIENT_API_ERROR);//退出
				//显示服务器的应答
		}
		puts(bufRecv);
	}

	//退出
	return ExitClient(CLIENT_EXIT_OK);
}


/*
 *	显示错误信息
 */
void	ShowErrorMsg(void)
{
	int nErrCode = WSAGetLastError();//获取错误代码

	HLOCAL hlocal = NULL;  
	
	//获取错误的文本字符串
	BOOL fOk = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, nErrCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(PTSTR)&hlocal, 0, NULL);
	
	//显示错误信息
	if (hlocal != NULL)
	{
		MessageBox(NULL, (char*)LocalLock(hlocal), "CLIENT ERROR", MB_OK);	
		LocalFree(hlocal);
	}
}


/*
 *	初始化成员变量
 */
void	InitMember(void)
{
	//初始化读和写缓冲区
	memset(bufRecv, 0, MAX_NUM_BUF);
	memset(bufSend, 0, MAX_NUM_BUF);
	//memset(buf, 0, MAX_NUM_BUF);
	//初始化
	sHost = INVALID_SOCKET;
	//没有连接状态
	bConning = FALSE;
}

/*
 *	退出
 */
int		ExitClient(int nExit)
{
	closesocket(sHost);		//关闭套接字
	WSACleanup();			//卸载Windows sockets DLL 清理内存

		//显示退出信息
	cout << "Client exiting..." << endl;
	Sleep(20000);
	return nExit;	//退出
}


/*
 *	读取一行数据
 */
/*BOOL	RecvLine(SOCKET s, char* buf)
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
			retVal= FALSE;	//读数据失败
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

BOOL	RecvLine1(SOCKET s, char* buf)
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
			retVal= FALSE;	//读数据失败
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
}*/

// 
// struct hostent {
// 	char FAR *			h_name;
// 	char FAR * FAR *	h_aliases;
// 	short				h_addrtype;
// 	short				h_length;
// 	char FAR * FAR *	h_addr_list;
// };
