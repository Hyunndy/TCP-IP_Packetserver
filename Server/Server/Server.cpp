/*

B477026 ������

��Ƽ������ ���� ����



����ü���� ������������ ���Ը��ؼ� ����ü�� �����͸� ĳ���������� ĳ�����Ͽ�

������ ���������� ĳ���������� ���� �����͸�

�ٽ� ����ü������ ĳ�����ϴ°� �̶�� ����ȴ�

[��ó] ����ü ������ ����|�ۼ��� ������







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



#pragma pack(push, 1) // #pragma pack(pop)�� ��Ʈ��. �� ��ó����� �뷮�� �ٿ��ֱ� ���� ����.



// ��Ŷ������ ����-���-������-oxff�� �Ѵ�.

// ���̳� �ڷ����� �����ϴ�.

// ����� �Ƹ� �α���, �α׾ƿ�, ������ ����?



// �α��� �� ���� PACKET.id = x�̷������� send�� ����ü�� ������ ��Ƽ� ������

// �α׾ƿ� �� ���� ���� message�� /bye�� ������ �����ϴ� �Լ��� ȣ���Ѵ�.

// ������ ���� ���� ��Ŷ ���� ������� �Ѵ�.



typedef struct PACKET

{

	DWORD Type; // ��ŶŸ��

	char Name[64]; // �̸�

	char Data[BUFSIZE]; // ������

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
		printf("%d�⵵ %d�� %d�� %d�� %d�� %s, Disconnected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		sprintf_s(wd, "%d�⵵ %d�� %d�� %d�� %d�� %s, Disconnected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		fputs(wd, fp);
	}
	else if (b==0)
	{
		printf("%d�⵵ %d�� %d�� %d�� %d�� %s, connected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
		sprintf_s(wd, "%d�⵵ %d�� %d�� %d�� %d�� %s, connected\n", (t2->tm_year + 1900), t2->tm_mon, t2->tm_hour, t2->tm_min, t2->tm_sec, a);
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
			printf("������ ������ Ŭ���̾�Ʈ IP ��ȣ:%d, PORT ��ȣ:%d\n", Sock_list[i].sin_addr,Sock_list[i].sin_port);
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

	fp = fopen("�����α�.txt", "w");
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

		printf("Ŭ���̾�Ʈ ����\n");

		printf("IP : %s, Port : %d \n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

		_beginthread(RecvThread, NULL, (void*)client_sock);

	}

	closesocket(serv_sock);
	fclose(fp);

	WSACleanup();

}



