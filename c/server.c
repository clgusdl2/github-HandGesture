#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI REACTCLIENT(void* arg);//������ �Լ�
void Sendmessage(char* message,int len);//�޽��� ������ �Լ�
void ErrorHandling(char* message);

int clientNum=0;
SOCKET clientSockets[MAX_CLNT];//Ŭ���̾�Ʈ ���� ������ �迭
HANDLE hMutex;//���ؽ�

void ErrorHandling(char* message) {
	WSACleanup();
	fputs(message, stderr);
	exit(1);
}

int main(){
	WSADATA wsaData;
	SOCKET serverSocket,clientSocket;
	SOCKADDR_IN SAddr,CAddr;
	int clientAddrSize;
	HANDLE hThread;

	char port[100];

	printf("Input port number : ");
	gets(port);

	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0) //������ ������ ����ϰڴٴ� ����� �ü���� ����
		ErrorHandling("WSAStartup() error!");


	serverSocket =socket(PF_INET,SOCK_STREAM,0); //�ϳ��� ������ �����Ѵ�.
	// socket(domain, type, protocol)
	// PF_INET ipv4 AF_INET(�ּ� ü�踦 ������ �� ����) PF_INET (�������� ü�輳���Ҷ� ����) SOCK_STREAM TCP(�ŷڼ�) DGRAM(UDP) ���� ���ͳ��������� ��0

	memset(&SAddr,0,sizeof(SAddr));
	SAddr.sin_family=AF_INET;
	SAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	SAddr.sin_port=htons(atoi(port));

	if(bind(serverSocket,(SOCKADDR*)&SAddr,sizeof(SAddr))==SOCKET_ERROR) //������ ������ ��ġ�Ѵ� 
		ErrorHandling("bind() error");
	//int bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen); ���� �������� bind �Լ��� addr(custom data structure) ������ �ּҿ� ��Ʈ��ȣ ������ ���̵�
	if(listen(serverSocket,5)==SOCKET_ERROR)//������ �غ���¿� �д�.
		ErrorHandling("listen() error");
	//int listen(int sockfd , int backlog); ���� ������ ���� ��� �����Ͽ� Ŭ���̾�Ʈ�� ������ �����Ͽ� �����ϱ� ��ٸ�.
	//backlog�� sockfd �� ���� �������� ���� ��⿭�� �����Ҽ��ִ� �ִ���̸� ����. ť�� ����á���� �����û�� ECONNEREFUSEDǥ�ÿ� ��������.
	printf("listening...\n");

	hMutex = CreateMutex(NULL, FALSE, NULL);//�ϳ��� ���ؽ��� �����Ѵ�.
	while(1){
		clientAddrSize=sizeof(CAddr);
		clientSocket =accept(serverSocket,(SOCKADDR*)&CAddr,&clientAddrSize);//�������� ���޵� Ŭ���̾�Ʈ ������ clientSocket�� ���� �̽������� �����������غ�Ϸ�
		WaitForSingleObject(hMutex,INFINITE);//���ؽ� ����
		clientSockets[clientNum++]= clientSocket;//Ŭ���̾�Ʈ ���Ϲ迭�� ��� ������ ���� �ּҸ� ����
		ReleaseMutex(hMutex);//���ؽ� ����
		hThread=(HANDLE)_beginthreadex(NULL,0, REACTCLIENT,(void*)&clientSocket,0,NULL);//REACTCLIENT ������ ����, clientSock�� �Ű������� ����
		printf("Connected Client IP : %s\n",inet_ntoa(CAddr.sin_addr));
	}
	closesocket(serverSocket);//������ ������ ����.
	WSACleanup();//������ ������ �����ϰڴٴ� ����� �ü���� ����
	return 0;
}

unsigned WINAPI REACTCLIENT(void* arg){
	SOCKET clientSocket =*((SOCKET*)arg); //�Ű������ι��� Ŭ���̾�Ʈ ������ ����
	int strLen=0,i;
	char message[BUF_SIZE];

	while((strLen=recv(clientSocket, message,sizeof(message),0))!=0){ //Ŭ���̾�Ʈ�κ��� �޽����� ���������� ��ٸ���.
		if(!strcmp(message,"q")){
			send(clientSocket,"q",1,0);
			break;
		}
		Sendmessage(message,strLen);
	}

	printf("client left the chat\n");
	//�� ���� �����Ѵٴ� ���� �ش� Ŭ���̾�Ʈ�� �����ٴ� ����� ���� �ش� Ŭ���̾�Ʈ�� �迭���� �����������

	WaitForSingleObject(hMutex,INFINITE);//���ؽ� ����

	for(i=0;i< clientNum;i++){//�迭�� ������ŭ
		if(clientSocket == clientSockets[i]){//���� ���� clientSock���� �迭�� ���� ���ٸ�
			while(i++< clientNum -1)//Ŭ���̾�Ʈ ���� ��ŭ
				clientSockets[i]= clientSockets[i+1];//������ �����.
			break;
		}
	}
	clientNum--;//Ŭ���̾�Ʈ ���� �ϳ� ����
	ReleaseMutex(hMutex);//���ؽ� ����
	closesocket(clientSocket);//������ �����Ѵ�.
	return 0;
}

void Sendmessage(char* message,int len){ //�޽����� ��� Ŭ���̾�Ʈ���� ������.
	int i;
	WaitForSingleObject(hMutex,INFINITE);//���ؽ� ����
	for(i=0;i< clientNum;i++)//Ŭ���̾�Ʈ ������ŭ
		send(clientSockets[i], message,len,0);//Ŭ���̾�Ʈ�鿡�� �޽����� �����Ѵ�.
	ReleaseMutex(hMutex);//���ؽ� ����
}
