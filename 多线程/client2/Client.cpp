// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <winsock.h>
#include <iostream.h>
#include <string.h>
#pragma comment(lib, "wsock32.lib")


#define CLIENT_EXIT_OK				0	//�ͻ��������˳�
#define CLIENT_DLL_REEOR			1	//����Windows socket dllʧ��
#define CLIENT_API_ERROR			2	//����Windows socket apiʧ��
#define MAX_NUM_BUF					64	//����������󳤶�
#define	SERVERPORT					5555//������TCP�˿�


//����
char	bufRecv[MAX_NUM_BUF];			//��������
char	bufSend[MAX_NUM_BUF];			//д������
char	buff1[MAX_NUM_BUF];
SOCKET	sHost;							//socket
BOOL	bConning;						//���ӷ�����״̬

//����
void	InitMember(void);				//��ʼ������
int		ExitClient(int nExit);			//�˳�
BOOL	RecvLine(SOCKET s, char* buf);	//��ȡһ������
void	ShowErrorMsg(void);				//��ʾ������Ϣ

//������
int main()
{

	//��ʼ������
	InitMember();

	WORD	wVersionRequested;		//Ӧ�ó�����ҪWindows sockets DLL�İ汾
	WSADATA	wsaData;				//Windows sockets DLL�汾��Ϣ
	int		retVal;					//����Windows sockets API����ֵ

	//��ʼ��Windows Sockets DLL
	wVersionRequested = MAKEWORD(1,1);	
	retVal = WSAStartup(wVersionRequested,(LPWSADATA)&wsaData);
	if ( 0 != retVal ) 
	{
		MessageBox(NULL, "Can not find a usable Windows Sockets dll!", "ERROR", MB_OK);
		return CLIENT_DLL_REEOR;
	}


	//����Windows socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == sHost)
	{
		ShowErrorMsg();			//��ʾ������Ϣ
		WSACleanup();			//�ͷ���Դ
		return CLIENT_API_ERROR;//�˳�
	}

	//׼�����ӷ�����
	cout << "Client succeeded!" << endl;
	cout<<"Be ready to connect to server..."<<endl;

	
	
	//��ȡ��������Ϣ
	LPHOSTENT hostEntry;
	char hostname[MAX_NUM_BUF];
	gethostname(hostname,MAX_NUM_BUF);			//��ȡ��������
	hostEntry = gethostbyname(hostname);		//��ȡ������Ϣ
	if(!hostEntry)
	{
		ShowErrorMsg();							//��ʾ������Ϣ
		return ExitClient(CLIENT_API_ERROR);	//�˳�
	}
	//����sockaddr_in
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	addrServ.sin_port = htons(SERVERPORT);
	//���ӷ�����
	retVal=connect(sHost,(LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == retVal)
	{
		ShowErrorMsg();							//��ʾ������Ϣ
		return ExitClient(CLIENT_API_ERROR);	//�˳�
	}else{
		bConning = TRUE;						//���ӷ������ɹ�
	}     
	//���ӷ������ɹ�
	cout<<"Connect successfully!"<<endl;


	//���������������
	//strcpy(bufSend, "Hello, Server!\n");
	
	//retVal = send(sHost, bufSend, strlen(bufSend), 0);
	//strcpy(bufSend, "Hello, Server!");
	if (SOCKET_ERROR == retVal)
	{
		ShowErrorMsg();							//��ʾ������Ϣ
		return ExitClient(CLIENT_API_ERROR);	//�˳�
	}
			/*if (!RecvLine(sHost, bufRecv))
		{
			ShowErrorMsg();							//��ʾ������Ϣ
			return ExitClient(CLIENT_API_ERROR);//�˳�
				//��ʾ��������Ӧ��
		}
		cout<<bufRecv<<endl;*/
	while(1)
	{
		
		puts("������Ҫ���͵��ַ���");
		memset(bufSend, 0, MAX_NUM_BUF);
		memset(bufRecv, 0, MAX_NUM_BUF);
		gets(bufSend);
		if(strlen(bufSend)==1 && bufSend[0] =='q')
			break;
		//cout<<strlen(buf)<<endl;
		//strcpy(bufSend, buff1);
		//cout<<strlen(bufSend)<<endl;
		retVal = send(sHost, bufSend, strlen(bufSend), 0);
		//�ӷ�������������
		if (!RecvLine(sHost, bufRecv))
		{
			ShowErrorMsg();							//��ʾ������Ϣ
			return ExitClient(CLIENT_API_ERROR);//�˳�
				//��ʾ��������Ӧ��
		}
		puts(bufRecv);
	}

	//�˳�
	return ExitClient(CLIENT_EXIT_OK);
}


/*
 *	��ʾ������Ϣ
 */
void	ShowErrorMsg(void)
{
	int nErrCode = WSAGetLastError();//��ȡ�������

	HLOCAL hlocal = NULL;  
	
	//��ȡ������ı��ַ���
	BOOL fOk = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, nErrCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(PTSTR)&hlocal, 0, NULL);
	
	//��ʾ������Ϣ
	if (hlocal != NULL)
	{
		MessageBox(NULL, (char*)LocalLock(hlocal), "CLIENT ERROR", MB_OK);	
		LocalFree(hlocal);
	}
}


/*
 *	��ʼ����Ա����
 */
void	InitMember(void)
{
	//��ʼ������д������
	memset(bufRecv, 0, MAX_NUM_BUF);
	memset(bufSend, 0, MAX_NUM_BUF);
	//memset(buf, 0, MAX_NUM_BUF);
	//��ʼ��
	sHost = INVALID_SOCKET;
	//û������״̬
	bConning = FALSE;
}

/*
 *	�˳�
 */
int		ExitClient(int nExit)
{
	closesocket(sHost);		//�ر��׽���
	WSACleanup();			//ж��Windows sockets DLL �����ڴ�

		//��ʾ�˳���Ϣ
	cout << "Client exiting..." << endl;
	Sleep(20000);
	return nExit;	//�˳�
}


/*
 *	��ȡһ������
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
			retVal= FALSE;	//������ʧ��
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

BOOL	RecvLine1(SOCKET s, char* buf)
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
			retVal= FALSE;	//������ʧ��
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

// 
// struct hostent {
// 	char FAR *			h_name;
// 	char FAR * FAR *	h_aliases;
// 	short				h_addrtype;
// 	short				h_length;
// 	char FAR * FAR *	h_addr_list;
// };
