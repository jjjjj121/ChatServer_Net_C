#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>
#include <conio.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	bool bRunning = true;
	WSAData Winsock;

	//���� �ʱ�ȭ
	WSAStartup(MAKEWORD(2, 2), &Winsock);

	//�ּ� �Է�
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerADDR;
	memset(&ServerADDR, 0, sizeof(SOCKADDR_IN));

	ServerADDR.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerADDR.sin_port = htons(1234);
	ServerADDR.sin_family = PF_INET;

	//��ũ����
	fd_set Original;
	fd_set CopyReads;
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 500;

	//�ʱ�ȭ
	FD_ZERO(&Original);
	//���ϴ� �������� ����
	FD_SET(ServerSocket, &Original);

	if (connect(ServerSocket, (SOCKADDR*)&ServerADDR, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Connet Failed" << endl;
		exit(-1);
	}


	while (bRunning)
	{

		if (_kbhit())
		{
			string ASendMessage;
			getline(cin, ASendMessage);
			system("cls");

			send(ServerSocket, ASendMessage.c_str(), strlen(ASendMessage.c_str()), 0);

		}

		CopyReads = Original;

		//polling
		//�Ǿ� �Ű������� WIndow�� 0�־�� ->�ʿ��� ���� ����?
		int fd_num = select(0, &CopyReads, 0, 0, &Timeout);


		if (fd_num == 0)
		{
			//another process
			continue;
		}

		//���� ���� �� ����
		if (fd_num == SOCKET_ERROR)
		{
			bRunning = false;
			break;
		}


		for (size_t i = 0; i < Original.fd_count; ++i)
		{
			//����� ���� ����Ʈ �߿� �̺�Ʈ�� �߻� ����.
			if (FD_ISSET(Original.fd_array[i], &CopyReads))
			{

				
				char RecvMessage[1024] = {};

				int RecvLength = recv(ServerSocket, RecvMessage, sizeof(RecvMessage), 0);
				cout << RecvMessage << endl;

			}

		}

	}


	//����

	closesocket(ServerSocket);


	WSACleanup;


	return 0;
}