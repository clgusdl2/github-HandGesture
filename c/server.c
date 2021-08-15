#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI REACTCLIENT(void* arg);//쓰레드 함수
void Sendmessage(char* message,int len);//메시지 보내는 함수
void ErrorHandling(char* message);

int clientNum=0;
SOCKET clientSockets[MAX_CLNT];//클라이언트 소켓 보관용 배열
HANDLE hMutex;//뮤텍스

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

	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0) //윈도우 소켓을 사용하겠다는 사실을 운영체제에 전달
		ErrorHandling("WSAStartup() error!");


	serverSocket =socket(PF_INET,SOCK_STREAM,0); //하나의 소켓을 생성한다.
	// socket(domain, type, protocol)
	// PF_INET ipv4 AF_INET(주소 체계를 설정할 때 유리) PF_INET (프로토콜 체계설정할때 유리) SOCK_STREAM TCP(신뢰성) DGRAM(UDP) 연결 인터넷프로토콜 값0

	memset(&SAddr,0,sizeof(SAddr));
	SAddr.sin_family=AF_INET;
	SAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	SAddr.sin_port=htons(atoi(port));

	if(bind(serverSocket,(SOCKADDR*)&SAddr,sizeof(SAddr))==SOCKET_ERROR) //생성한 소켓을 배치한다 
		ErrorHandling("bind() error");
	//int bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen); 소켓 생성이후 bind 함수는 addr(custom data structure) 지정된 주소와 포트번호 소켓을 반이딩
	if(listen(serverSocket,5)==SOCKET_ERROR)//소켓을 준비상태에 둔다.
		ErrorHandling("listen() error");
	//int listen(int sockfd , int backlog); 서버 소켓을 수동 모드 설정하여 클라이언트가 서버에 접근하여 연결하기 기다림.
	//backlog는 sockfd 에 대해 보류중인 연결 대기열이 증가할수있는 최대길이를 정의. 큐가 가득찼을때 연결요청시 ECONNEREFUSED표시와 오류수신.
	printf("listening...\n");

	hMutex = CreateMutex(NULL, FALSE, NULL);//하나의 뮤텍스를 생성한다.
	while(1){
		clientAddrSize=sizeof(CAddr);
		clientSocket =accept(serverSocket,(SOCKADDR*)&CAddr,&clientAddrSize);//서버에게 전달된 클라이언트 소켓을 clientSocket에 전달 이시점부터 데이터전송준비완료
		WaitForSingleObject(hMutex,INFINITE);//뮤텍스 실행
		clientSockets[clientNum++]= clientSocket;//클라이언트 소켓배열에 방금 가져온 소켓 주소를 전달
		ReleaseMutex(hMutex);//뮤텍스 중지
		hThread=(HANDLE)_beginthreadex(NULL,0, REACTCLIENT,(void*)&clientSocket,0,NULL);//REACTCLIENT 쓰레드 실행, clientSock을 매개변수로 전달
		printf("Connected Client IP : %s\n",inet_ntoa(CAddr.sin_addr));
	}
	closesocket(serverSocket);//생성한 소켓을 끈다.
	WSACleanup();//윈도우 소켓을 종료하겠다는 사실을 운영체제에 전달
	return 0;
}

unsigned WINAPI REACTCLIENT(void* arg){
	SOCKET clientSocket =*((SOCKET*)arg); //매개변수로받은 클라이언트 소켓을 전달
	int strLen=0,i;
	char message[BUF_SIZE];

	while((strLen=recv(clientSocket, message,sizeof(message),0))!=0){ //클라이언트로부터 메시지를 받을때까지 기다린다.
		if(!strcmp(message,"q")){
			send(clientSocket,"q",1,0);
			break;
		}
		Sendmessage(message,strLen);
	}

	printf("client left the chat\n");
	//이 줄을 실행한다는 것은 해당 클라이언트가 나갔다는 사실임 따라서 해당 클라이언트를 배열에서 제거해줘야함

	WaitForSingleObject(hMutex,INFINITE);//뮤텍스 실행

	for(i=0;i< clientNum;i++){//배열의 갯수만큼
		if(clientSocket == clientSockets[i]){//만약 현재 clientSock값이 배열의 값과 같다면
			while(i++< clientNum -1)//클라이언트 개수 만큼
				clientSockets[i]= clientSockets[i+1];//앞으로 땡긴다.
			break;
		}
	}
	clientNum--;//클라이언트 개수 하나 감소
	ReleaseMutex(hMutex);//뮤텍스 중지
	closesocket(clientSocket);//소켓을 종료한다.
	return 0;
}

void Sendmessage(char* message,int len){ //메시지를 모든 클라이언트에게 보낸다.
	int i;
	WaitForSingleObject(hMutex,INFINITE);//뮤텍스 실행
	for(i=0;i< clientNum;i++)//클라이언트 개수만큼
		send(clientSockets[i], message,len,0);//클라이언트들에게 메시지를 전달한다.
	ReleaseMutex(hMutex);//뮤텍스 중지
}
