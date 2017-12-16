/*

B477026 유현지

멀티스레드 서버 과제



구조체형의 데이터전송은 쉽게말해서 구조체형 데이터를 캐릭터형으로 캐스팅하여

보낸후 서버측에서 캐릭터형으로 받은 데이터를

다시 구조체형으로 캐스팅하는것 이라고 보면된다

[출처] 구조체 데이터 전송|작성자 로지어







Server.cpp

*/



#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS

#pragma comment (lib, "ws2_32.lib")

#include <stdio.h>

#include <WinSock2.h>

#include <process.h>

#include <vector>

#include <time.h>
#include <iostream>
using namespace std;



#define BUFSIZE 256



typedef enum PTYPE

{
	CHAT_LOGIN = 0,

	CHAT_LOGOUT = 1,

	CHAT_DATA = 2
}PTYPE;



#pragma pack(push, 1) // #pragma pack(pop)와 세트임. 이 전처리기는 용량을 줄여주기 위해 쓴다.



// 패킷구조는 길이-헤더-데이터-oxff로 한다.

// 길이나 자료형은 무관하다.

// 헤더는 아마 로그인, 로그아웃, 데이터 전송?



// 로그인 할 때는 PACKET.id = x이런식으로 send에 구조체의 정보를 담아서 보내고

// 로그아웃 할 때는 만약 message에 /bye가 나오면 퇴장하는 함수를 호출한다.

// 데이터 보낼 때는 패킷 설계 과제대로 한다.



typedef struct PACKET

{

	DWORD Type; // 패킷타입

	char Name[64]; // 이름

	char Data[BUFSIZE]; // 데이터

	PACKET()

	{	
		Type = 0;

		memset(Name, 0, sizeof(Name));

		memset(Data, 0, sizeof(BUFSIZE));

	}

}PACKET;



#pragma pack(pop)



vector<SOCKET> Client_list;
vector<SOCKADDR_IN> Sock_list;

FILE *fp;
bool Exit = false;



int recvn(SOCKET s, char *buf, int len, int flags)

{

	int received;

	char *ptr = (buf);

	int left = len;



	//PacketPashing(ptr, pack);



	while (left > 0) {

		received = recv(s, ptr, left, flags);

		if (received == SOCKET_ERROR)

			return SOCKET_ERROR;

		else if (received == 0)

			break;

		left -= received;

		ptr += received;

	}

	return (len - left);

}

void File(char *a, int b)
{
	time_t t1;
	struct tm* t2;

	char wd[256] = "wd";;
	time(&t1);
	t2 = localtime(&t1);
	if (b==1)
	{
		printf("%d년도 %d월 %d시 %d분 %d초 %s, Disconnected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		sprintf_s(wd, "%d년도 %d월 %d시 %d분 %d초 %s, Disconnected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		fputs(wd, fp);
	}
	else if (b==0)
	{
		printf("%d년도 %d월 %d시 %d분 %d초 %s, connected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		sprintf_s(wd, "%d년도 %d월 %d시 %d분 %d초 %s, connected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		fputs(wd, fp);
	}
}

void RecvThread(void *p)

{

	SOCKET client_sock = (SOCKET)p;

	char buf[1024];
	char buf2[256];
	int size;

	PACKET *pack;



	while (1)

	{

		//int recvsize = recvn(client_sock, (char*)&size, sizeof(int), 0);

		int recvsize = recv(client_sock, buf, 1024, 0);

		if (recvsize <= 0)

			break;

		buf[recvsize] = '\0';

		pack = (PACKET *)buf;


		if (pack->Type==2)
		{

			for (int i = 0; i < Client_list.size(); i++)

			{

				if (Client_list[i] != client_sock)

				{
					sprintf_s(buf2, "[ID : %s] %s ", pack->Name, pack->Data);
					int sendsize = send(Client_list[i], buf2, strlen(buf2), 0);
				}

			}

			printf("[ID : %s] %s\n",pack->Name, pack->Data);
		}

		if (pack->Type==1)
		{

			printf("%s, disconnection request succss \n", pack->Name);
			File(pack->Name, 1);
			for (int i = 0; i < Client_list.size(); i++)

			{
				if (Client_list[i] != client_sock)

				{
					sprintf_s(buf2, "%s, disconnected ", pack->Name);
					int sendsize = send(Client_list[i], buf2, strlen(buf2), 0);
				}

				
			}
			break;
		}

		if(pack->Type ==0)
		{					

			printf("%s, connection request succss\n", pack->Name);
			File(pack->Name, 0);

			for (int i = 0; i < Client_list.size(); i++)

			{

				if (Client_list[i] != client_sock)

				{
					sprintf_s(buf2, "%s, connected", pack->Name);
					int sendsize = send(Client_list[i], buf2, strlen(buf2), 0);

				}

			}

		}
	}



	vector<SOCKET>::iterator iter = Client_list.begin();
	vector<SOCKADDR_IN>::iterator iter2 = Sock_list.begin();

	for (int i = 0; i < Client_list.size(); i++)

	{
		if (Client_list[i] == client_sock)

		{
			printf("접속이 끊어진 클라이언트 IP 번호:%d, PORT 번호:%d\n", Sock_list[i].sin_addr,Sock_list[i].sin_port);
			Sock_list.erase(iter2);
			Client_list.erase(iter);


			break;

		}

		iter++;
		iter2++;

	}

	closesocket(client_sock);
}

SOCKADDR_IN client_addr2 = { 0 };

int main()

{

	WSADATA wsaData;

	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (retval != 0)

	{

		printf("WSAStartup() Error\n");

		return 0;

	}


	SOCKET serv_sock;

	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serv_sock == SOCKET_ERROR)

	{

		printf("socket() Error\n");

		return 0;

	}


	SOCKADDR_IN serv_addr = { 0 };

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_port = htons(4000);

	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);


	retval = bind(serv_sock, (SOCKADDR*)&serv_addr, sizeof(SOCKADDR));

	if (retval == SOCKET_ERROR)

	{

		printf("bind() Error\n");

		return 0;

	}

	listen(serv_sock, 5);

	SOCKADDR_IN client_addr = { 0 };

	int size = sizeof(SOCKADDR_IN);

	fp = fopen("서버로그.txt", "w");
	printf("Server Connected......\n");

	while (1)

	{

		SOCKET client_sock = accept(serv_sock, (SOCKADDR*)&client_addr, &size);
		client_addr2 = client_addr;

		if (client_sock == SOCKET_ERROR)

		{

			printf("accept() Error\n");

			continue;

		}

		Client_list.push_back(client_sock);
		Sock_list.push_back(client_addr2);

		printf("클라이언트 접속\n");

		printf("IP : %s, Port : %d \n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

		_beginthread(RecvThread, NULL, (void*)client_sock);

	}

	closesocket(serv_sock);
	fclose(fp);

	WSACleanup();

}



