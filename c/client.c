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

	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)// 윈도우 소켓을 사용한다고 운영체제에 알림
		ErrorHandling("WSAStartup() error!");

	sprintf(name,"[%s]: ",inputName);
	sock=socket(PF_INET,SOCK_STREAM,0);//소켓을 하나 생성한다.

	memset(&serverAddr,0,sizeof(serverAddr));
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_addr.s_addr=inet_addr(myIp);
	serverAddr.sin_port=htons(atoi(port));

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)//서버에 접속한다.
	{
		ErrorHandling("connect() error");
	}


	//쓰레드만들기 
	sendThread=(HANDLE)_beginthreadex(NULL,0, SendM,(void*)&sock,0,NULL);//메시지 전송용 쓰레드가 실행된다.
	recvThread=(HANDLE)_beginthreadex(NULL,0, RecvMessage,(void*)&sock,0,NULL);//메시지 수신용 쓰레드가 실행된다.

	WaitForSingleObject(sendThread,INFINITE);//전송용 쓰레드가 중지될때까지 기다린다.
	WaitForSingleObject(recvThread,INFINITE);//수신용 쓰레드가 중지될때까지 기다린다.

	//클라이언트가 종료시
	closesocket(sock);//소켓을 종료한다.
	WSACleanup();//윈도우 소켓 사용중지를 운영체제에 알린다.
	return 0;
}

unsigned WINAPI SendM(void* arg){//전송용 쓰레드함수
	SOCKET sock=*((SOCKET*)arg);//서버용 소켓을 전달한다.
	char ClientMessage[NAMESZ + BUFSZ];
	while(1){//반복
		fgets(message, BUFSZ,stdin);//입력을 받는다.
		if(!strcmp(message,"q\n")){//q를 입력하면 종료한다.
			send(sock,"q",1,0);//nameMsg를 서버에게 전송한다.
		}
		sprintf(ClientMessage,"%s:%s",name, message);
		send(sock, ClientMessage, strlen(ClientMessage), 0);
	}
	return 0;
}

unsigned WINAPI RecvMessage(void* arg){
	SOCKET sock=*((SOCKET*)arg);//서버용 소켓을 전달한다.
	char ClientMessage[NAMESZ + BUFSZ];
	int strLen;
	while(1){//반복
		strLen=recv(sock, ClientMessage, NAMESZ + BUFSZ -1,0);//서버로부터 메시지를 수신한다.
		if(strLen==-1)
			return -1;
		ClientMessage[strLen]=0;//문자열의 끝을 알리기 위해 설정
		if(!strcmp(ClientMessage,"exit")){
			printf("left the chat\n");
			closesocket(sock);
			exit(0);
		}
		fputs(ClientMessage,stdout);//자신의 콘솔에 받은 메시지를 출력한다.
	}
	return 0;
}
