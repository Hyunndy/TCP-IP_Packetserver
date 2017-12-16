/*

B477026 ������

��Ƽ������ ���� ����



Client.cpp

*/



#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS



#pragma comment (lib, "ws2_32.lib")





#include <stdio.h>

#include <WinSock2.h>

#include <process.h>

#include <ws2tcpip.h>

#include <iostream>



using namespace std;



#define BUFSIZE 256



typedef enum PTYPE

{

	CHAT_LOGIN = 0,

	CHAT_LOGOUT = 1,

	CHAT_DATA =2



}PTYPE;



#pragma pack(push, 1) // #pragma pack(pop)�� ��Ʈ��. �� ��ó����� �뷮�� �ٿ��ֱ� ���� ����.



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


void login(PACKET* p)
{
	
	cout << "ID :";

	cin >> p->Name;

	p->Type = 0;
	p->Data[BUFSIZE] = { 0x00, };

}


void __cdecl RecvThread(void * p)

{

	SOCKET sock = (SOCKET)p;



	char buf[1024];
	PACKET * pack;

	int size;

	while (1)

	{
		//int recvsize = recv(sock, (char*)&size, sizeof(int), 0);

		int recvsize = recv(sock, buf, 1024, 0);

		if (recvsize <= 0)
		{
			printf("��������\n");
			break;
		}

		buf[recvsize] = '\0';

		//pack = (PACKET*)buf;
		
		printf("%s\n",buf);

	}

}



int main()

{

	WSADATA wsaData;

	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (retval != 0)

	{

		printf("WSAStartup() Error\n");

		return 0;

	}


	SOCKET serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serv_addr;

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_port = htons(4000);

	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");



	// ��Ŷ �ʱ�ȭ

	PACKET pack = {};




	retval = connect(serv_sock, (SOCKADDR*)&serv_addr, sizeof(SOCKADDR));

	if (retval == SOCKET_ERROR)

	{

		printf("connect() Error\n");

		return 0;

	}



	SOCKET cli_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in cli_addr2;





	socklen_t addr_len = sizeof(sockaddr_in);

	int ip = getsockname(cli_sock, (SOCKADDR *)&cli_addr2, &addr_len);



	char myaddr[16];



	strncpy(myaddr, inet_ntoa(cli_addr2.sin_addr), 16);



	//�α���

	login(&pack);


	if (pack.Type==0)

	{

		//sprintf_s(buf, "%s, connection reqeust succss", pack.Name);
		//int sendsize = send(serv_sock, buf, strlen(buf), 0);

		int sendsize = send(serv_sock, (char *)&pack, sizeof(pack), 0);

		pack.Type=2;

		_beginthread(RecvThread, 0, (void*)serv_sock);

		while (1)

		{

			// �α��� �� ���� �޼��� ���

			if (pack.Type==2)

			{
				//cout << "���� �޼��� :";

				scanf("%s", pack.Data);
				//cin >> pack.Data;


				if(strcmp(pack.Data, "/bye")==0)
				{
					pack.Type=1;
					int sendsize2 = send(serv_sock, (char *)&pack, sizeof(pack), 0);
				}
				else
					int sendsize2 = send(serv_sock, (char *)&pack, sizeof(pack), 0);	
			}

		}


	}


	free(pack.Data);



	closesocket(serv_sock);



	WSACleanup();



}