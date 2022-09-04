#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main()
{
	bool bRunning = true;
	vector<SOCKET> ClientArray;
	//Winsock 초기화

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	//memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	memset((char*)&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = INADDR_ANY;

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


	if (bind(ServerSocket, (SOCKADDR*)&ServerAddrIn, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Bind Failed" << endl;
		exit(-1);
	}

	
	if (listen(ServerSocket, 0) == SOCKET_ERROR)
	{
		cout << "Listen Failed" << endl;
		exit(-1);
	}

	while (bRunning)
	{

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
				if (Original.fd_array[i] == ServerSocket)
				{


					//서버 소켓에 이벤트가 발생함, 연결 요청
					SOCKADDR_IN ClientSockAddrIn;
					memset((char*)&ClientSockAddrIn, 0, sizeof(SOCKADDR_IN));
					int ClientSockAddrInSize = sizeof(SOCKADDR_IN);
					SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSockAddrIn, &ClientSockAddrInSize);
					
					//각 클라이언트 소켓 저장
					ClientArray.push_back(ClientSocket);

					FD_SET(ClientSocket, &Original);
					std::cout << "Connected Client : " << ClientSocket << std::endl;
				}

				else
				{
					SOCKET ClientSocket = Original.fd_array[i];

					char RecvMessage[1024] = {};
					int RecvLength = recv(ClientSocket, RecvMessage, sizeof(RecvMessage), 0);
					//cout << RecvMessage << endl;


					if (RecvLength == 0)
					{
						//연결 종료
						std::cout << "Disconnected Client."  << std::endl;
						FD_CLR(ClientSocket, &Original);


						for (int i = 0; i < ClientArray.size(); ++i)
						{

							if (ClientArray[i] == ClientSocket)
							{
								ClientArray.erase(ClientArray.begin()+i);
							}

						}
					
						closesocket(ClientSocket);
						continue;

					}
					else if (RecvLength < 0)
					{
						//Error
						std::cout << "Disconnected Client By Error : " << GetLastError() << std::endl;
						FD_CLR(ClientSocket, &Original);

						for (int i = 0; i < ClientArray.size(); ++i)
						{

							if (ClientArray[i] == ClientSocket)
							{
								ClientArray.erase(ClientArray.begin() + i);
							}

						}


						closesocket(ClientSocket);
						continue;

					}
					else
					{

						cout << "현재 접속한 Client 수 : " << ClientArray.size() << endl;
						cout << "Client " << ClientSocket << "의 Message : " << RecvMessage << endl;
						
						for (int i = 0; i < ClientArray.size(); i++)
						{

							int SendLegth = send(ClientArray[i], RecvMessage, strlen(RecvMessage), 0);

						}

						
						
						
						//데이터 송신
						//string ASendMessage;
						//getline(cin, ASendMessage);

						

					}




					//closesocket(ClientSocket);



				}
			}
		}
	}



	//종료
	//closesocket(ClientSocket);
	closesocket(ServerSocket);


	WSACleanup;


	return 0;
}