// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream.h>
#include <WINSOCK2.H>
#pragma comment(lib, "wsock32.lib")

#define SERVER_EXIT_OK				0	//�����������˳�
#define SERVER_DLL_REEOR			1	//����Windows sockets DLLʧ��
#define SERVER_API_ERROR			2	//����Windows sockets APIʧ��
#define	SERVERPORT					5555//������TCP�˿�
#define MAX_NUM_BUF					64	//���������ߴ�

//����
char	bufRecv[MAX_NUM_BUF];			//��������
char	bufSend[MAX_NUM_BUF];			//д������
SOCKET	sServer;						//�����������׽���
SOCKET	sClient;						//���ܿͻ����׽���
BOOL	bConning;						//��ͻ��˵�����״̬

//����
void	InitMember(void);				//��ʼ����Ա���� 
int		ExitClient(int nExit);			//�ͻ����˳�
BOOL	RecvLine(SOCKET s, char* buf);	//��ȡһ������
BOOL	SendLine(SOCKET s, char* buf);	//����һ������
int		HandleSocketError(char *str);	//��Windows sockets API���ô�����
void	ShowSocketMsg(char* str);		//��ʾ������Ϣ

//������
int main(int argc, char* argv[])
{

	InitMember();					//��ʼ������
	
	WORD	wVersionRequested;		//Ӧ�ó�����ҪWindows sockets DLL�İ汾
	WSADATA	wsaData;				//Windows sockets DLL�汾��Ϣ
	int		retVal;					//����Windows sockets API����ֵ		
	//��ʼ��Windows Sockets DLL
	wVersionRequested = MAKEWORD(1,1);
	retVal = WSAStartup(wVersionRequested, &wsaData);
	if ( 0 != retVal ) 
	{
		ShowSocketMsg("Can not find a usable Windows Sockets dll!");
		return SERVER_DLL_REEOR;
	}	
	//ȷ��WinSock DLL֧��1.1
	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1)
	{
		ShowSocketMsg("Can not find a usable Windows Sockets dll!");
		WSACleanup( );
		return SERVER_DLL_REEOR; 
	}
	
	
	//�����׽���
	sServer= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if(INVALID_SOCKET == sServer)
	{
		return HandleSocketError("Failed socket()!");
	}
	
	//�������׽��ֵ�ַ 
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(SERVERPORT);
	addrServ.sin_addr.s_addr = INADDR_ANY;		
	//���׽���
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == retVal)
	{
		closesocket(sServer);						//�ر��׽���
		return HandleSocketError("Failed bind()!");	//������
	}
	
	//��ʼ���� 
	retVal = listen(sServer, 1);
	if(SOCKET_ERROR == retVal)
	{
		closesocket(sServer);						//�ر��׽���
		return HandleSocketError("Failed listen()!");//������
	}
	

	//�ȴ��ͻ��˵�����	
	cout << "Server succeeded!" << endl;
	cout << "Waiting for new clients..." << endl;



	//���ܿͻ�������
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer,(sockaddr FAR*)&addrClient, &addrClientlen);
	if(INVALID_SOCKET == sClient)
	{
		closesocket(sServer);							//�ر��׽���
		return HandleSocketError("Failed accept()!");	//������
	}else{
		bConning = TRUE;								//�ͻ�������ɹ�
	}
	
	//��ʾ�ͻ��˵�IP�Ͷ˿�
	char *pClientIP = inet_ntoa(addrClient.sin_addr);
	u_short  clientPort = ntohs(addrClient.sin_port);	
	cout<<"Accept a client."<<endl;
	cout<<"IP: "<<pClientIP<<endl;
	cout<<"Port: "<<clientPort<<endl;
		/*if (!RecvLine(sClient, bufRecv))
		{
			return	ExitClient(SERVER_API_ERROR);//�˳�
		}	
		//��ʾ�ͻ�������
		cout << bufRecv<<endl;*/


		//��ͻ��˷�������
		/*strcpy(bufSend, "Hello,Client!\n");
		if (!SendLine(sClient, bufSend))
		{
			return	ExitClient(SERVER_API_ERROR);
		}*/
	while(1)
	{
		memset(bufSend, 0, MAX_NUM_BUF);
		memset(bufRecv, 0, MAX_NUM_BUF);
		int cd = recv(sClient, bufRecv,MAX_NUM_BUF, 0);	//���տͻ�������
		if (!cd)
		{
			return	ExitClient(SERVER_API_ERROR);//�˳�
		}	
		//��ʾ�ͻ�������
		cout << bufRecv<<endl;


		//��ͻ��˷�������
		if(bufRecv[cd] == '\n')
		{
			bufRecv[cd] = 'e';
			strcat(bufRecv,"cho\n");
		}
		else
		{
			strcat(bufRecv,"echo\n");
		}
		bufRecv[strlen(bufRecv)]='\0';
		strcpy(bufSend, bufRecv);
		send(sClient, bufSend, strlen(bufSend), 0);
	}
	

	//��ʾ�˳���Ϣ
	cout << "Server exiting..." << endl;
	
	//�˳�
	return ExitClient(SERVER_EXIT_OK);
}



/*
 *	��ʼ����Ա����
 */
void	InitMember(void)
{
	//��ʼ������д������
	memset(bufRecv, 0, MAX_NUM_BUF);
	memset(bufSend, 0, MAX_NUM_BUF);

	//��ʼ��
	sServer = INVALID_SOCKET;
	sClient = INVALID_SOCKET;

	//û������״̬
	bConning = FALSE;
}


/*
 *	�˳�
 */
int		ExitClient(int nExit)
{
	closesocket(sServer);	//�رռ����׽���
	closesocket(sClient);	//�ر����ӿͻ����׽ӽ�
	WSACleanup();			//ж��Windows sockets DLL �����ڴ�
	return nExit;			//�˳�
}
/*
 *	��һ������
 */
BOOL	RecvLine(SOCKET s, char* buf)
{
	BOOL	retVal = TRUE;			//����ֵ
	BOOL	bLineEnd = FALSE;		//�н���
	int		nReadLen = 0;			//�����ֽ���
	int		nDataLen = 0;			//���ݳ���
	while (!bLineEnd && bConning)	//��ͻ������� û�л���
	{
		nReadLen = recv(s, buf + nDataLen, 1, 0);//ÿ�ν���һ���ֽ�
		
		//������
		if (SOCKET_ERROR == nReadLen)
		{
			int nErrCode = WSAGetLastError();//�������
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
			
			retVal = FALSE;	//������ʧ��
			break;			//����ѭ��						
		}
		
		
		if (0 == nReadLen)//�ͻ��˹ر�
		{
			retVal = FALSE;	//������ʧ��
			break ;			//����ѭ��			
		}
		
		//��������
		if ('\n' == *(buf + nDataLen))	//���з�
		{
			bLineEnd = TRUE;			//�������ݽ���
		}else{
			nDataLen += nReadLen;		//�������ݳ���
		}	
	}
	
	return retVal;
}

/*
 *	//����һ������
 */
BOOL	SendLine(SOCKET s, char* str)
{
	int retVal;//����ֵ
	retVal = send(s, str, strlen(str), 0);//һ�η���

	//������
	if (SOCKET_ERROR == retVal)
	{
		int nErrCode = WSAGetLastError();//�������
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
		
		return FALSE;	//����ʧ��
	}
	
	return TRUE;		//���ͳɹ�
}
	
/*
 *	������
 */
int		HandleSocketError(char *str)
{
	ShowSocketMsg(str);		//��ʾ������Ϣ	
	WSACleanup();			//ж��Windows socket DLL	
	return SERVER_API_ERROR;//�˳�Ӧ�ó���
}

/*
 *	��ʾ����
 */
void	ShowSocketMsg(char* str)
{
	MessageBox(NULL, str, "SERVER ERROR", MB_OK);
}

int recvn(SOCKET s, char * recvbuf, unsigned int fixedlen)

{

	int iResult;//�洢����recv�����ķ���ֵ

	int cnt;//����ͳ������ڹ̶����ȣ�ʣ������ֽ���δ����

	cnt = fixedlen;

	while (cnt > 0)

	{

		iResult = recv(s, recvbuf, cnt, 0);

		if (iResult < 0)

		{

			//���ݽ��ճ��ִ��󣬷���ʧ��

			printf("���շ�������: %d\n", WSAGetLastError());

			return -1;

		}

		if (iResult == 0)

		{

			//�Է��ر����ӣ������ѽ��յ���С��fixedlen���ֽ���

			printf("���ӹر�\n");

			return fixedlen - cnt;

		}

		//printf("���յ����ֽ���: %d\n", iResult);

		//���ջ���ָ������ƶ�

		recvbuf += iResult;

		//����cntֵ

		cnt -= iResult;

	}

	return fixedlen;

}

	