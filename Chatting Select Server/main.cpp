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
	//Winsock �ʱ�ȭ

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	//memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	memset((char*)&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = INADDR_ANY;

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
				if (Original.fd_array[i] == ServerSocket)
				{


					//���� ���Ͽ� �̺�Ʈ�� �߻���, ���� ��û
					SOCKADDR_IN ClientSockAddrIn;
					memset((char*)&ClientSockAddrIn, 0, sizeof(SOCKADDR_IN));
					int ClientSockAddrInSize = sizeof(SOCKADDR_IN);
					SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSockAddrIn, &ClientSockAddrInSize);
					
					//�� Ŭ���̾�Ʈ ���� ����
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
						//���� ����
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

						cout << "���� ������ Client �� : " << ClientArray.size() << endl;
						cout << "Client " << ClientSocket << "�� Message : " << RecvMessage << endl;
						
						for (int i = 0; i < ClientArray.size(); i++)
						{

							int SendLegth = send(ClientArray[i], RecvMessage, strlen(RecvMessage), 0);

						}

						
						
						
						//������ �۽�
						//string ASendMessage;
						//getline(cin, ASendMessage);

						

					}




					//closesocket(ClientSocket);



				}
			}
		}
	}



	//����
	//closesocket(ClientSocket);
	closesocket(ServerSocket);


	WSACleanup;


	return 0;
}