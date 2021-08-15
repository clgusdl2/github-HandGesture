#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#define BUFSZ 100
#define NAMESZ 20

unsigned WINAPI SendM(void* arg);
unsigned WINAPI RecvMessage(void* arg);
void ErrorHandling(char* message);

char name[NAMESZ]="[]";
char message[BUFSZ];

void ErrorHandling(char* message) {
	WSACleanup();
	fputs(message, stderr);
	exit(1);
}


int main(){
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN serverAddr;
	HANDLE sendThread,recvThread;

	char myIp[100];
	char port[100];
	char inputName[100];
	printf("Input server IP : ");
	gets(myIp);

	printf("Input server port : ");
	gets(port);

	printf("Input your name : ");
	gets(inputName);

	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)// ������ ������ ����Ѵٰ� �ü���� �˸�
		ErrorHandling("WSAStartup() error!");

	sprintf(name,"[%s]: ",inputName);
	sock=socket(PF_INET,SOCK_STREAM,0);//������ �ϳ� �����Ѵ�.

	memset(&serverAddr,0,sizeof(serverAddr));
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_addr.s_addr=inet_addr(myIp);
	serverAddr.sin_port=htons(atoi(port));

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)//������ �����Ѵ�.
	{
		ErrorHandling("connect() error");
	}


	//�����常��� 
	sendThread=(HANDLE)_beginthreadex(NULL,0, SendM,(void*)&sock,0,NULL);//�޽��� ���ۿ� �����尡 ����ȴ�.
	recvThread=(HANDLE)_beginthreadex(NULL,0, RecvMessage,(void*)&sock,0,NULL);//�޽��� ���ſ� �����尡 ����ȴ�.

	WaitForSingleObject(sendThread,INFINITE);//���ۿ� �����尡 �����ɶ����� ��ٸ���.
	WaitForSingleObject(recvThread,INFINITE);//���ſ� �����尡 �����ɶ����� ��ٸ���.

	//Ŭ���̾�Ʈ�� �����
	closesocket(sock);//������ �����Ѵ�.
	WSACleanup();//������ ���� ��������� �ü���� �˸���.
	return 0;
}

unsigned WINAPI SendM(void* arg){//���ۿ� �������Լ�
	SOCKET sock=*((SOCKET*)arg);//������ ������ �����Ѵ�.
	char ClientMessage[NAMESZ + BUFSZ];
	while(1){//�ݺ�
		fgets(message, BUFSZ,stdin);//�Է��� �޴´�.
		if(!strcmp(message,"q\n")){//q�� �Է��ϸ� �����Ѵ�.
			send(sock,"q",1,0);//nameMsg�� �������� �����Ѵ�.
		}
		sprintf(ClientMessage,"%s:%s",name, message);
		send(sock, ClientMessage, strlen(ClientMessage), 0);
	}
	return 0;
}

unsigned WINAPI RecvMessage(void* arg){
	SOCKET sock=*((SOCKET*)arg);//������ ������ �����Ѵ�.
	char ClientMessage[NAMESZ + BUFSZ];
	int strLen;
	while(1){//�ݺ�
		strLen=recv(sock, ClientMessage, NAMESZ + BUFSZ -1,0);//�����κ��� �޽����� �����Ѵ�.
		if(strLen==-1)
			return -1;
		ClientMessage[strLen]=0;//���ڿ��� ���� �˸��� ���� ����
		if(!strcmp(ClientMessage,"exit")){
			printf("left the chat\n");
			closesocket(sock);
			exit(0);
		}
		fputs(ClientMessage,stdout);//�ڽ��� �ֿܼ� ���� �޽����� ����Ѵ�.
	}
	return 0;
}
