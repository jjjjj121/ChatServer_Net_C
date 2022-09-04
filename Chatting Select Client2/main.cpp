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

	//윈속 초기화
	WSAStartup(MAKEWORD(2, 2), &Winsock);

	//주소 입력
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerADDR;
	memset(&ServerADDR, 0, sizeof(SOCKADDR_IN));

	ServerADDR.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerADDR.sin_port = htons(1234);
	ServerADDR.sin_family = PF_INET;

	//디스크립터
	fd_set Original;
	fd_set CopyReads;
	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 500;

	//초기화
	FD_ZERO(&Original);
	//원하는 소켓으로 셋팅
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
		//맨앞 매개변수는 WIndow는 0넣어도됨 ->필요한 소켓 갯수?
		int fd_num = select(0, &CopyReads, 0, 0, &Timeout);


		if (fd_num == 0)
		{
			//another process
			continue;
		}

		//소켓 에러 시 종료
		if (fd_num == SOCKET_ERROR)
		{
			bRunning = false;
			break;
		}


		for (size_t i = 0; i < Original.fd_count; ++i)
		{
			//등록한 소켓 리스트 중에 이벤트가 발생 했음.
			if (FD_ISSET(Original.fd_array[i], &CopyReads))
			{

				
				char RecvMessage[1024] = {};

				int RecvLength = recv(ServerSocket, RecvMessage, sizeof(RecvMessage), 0);
				cout << RecvMessage << endl;

			}

		}

	}


	//종료

	closesocket(ServerSocket);


	WSACleanup;


	return 0;
}