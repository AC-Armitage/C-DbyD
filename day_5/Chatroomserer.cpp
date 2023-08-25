#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

const int PORT = 5999;
const int MAX_CLIENTS = 20;
std::vector<int> Clients;

void HandleClient(int ClientSocket, char ClientIP[INET_ADDRSTRLEN]){
	char Buffer[1024];
	while (true){
		int BytesRecieved = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (BytesRecieved <= 0){
			std::cout << "Client " << ClientSocket << " " << ClientIP << std::endl;
			close(ClientSocket);
			break;
		}
		for (int i; i < Clients.size(); i++){
			if (Clients[i] != ClientSocket){
				std::cout << "Sent" << std::endl;
				send(Clients[i], Buffer, BytesRecieved, 0);
			}
		}
	}
}

int main(void){

	int Socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (Socket == -1){
		std::cerr << "Error Creating Socket " << std::endl;
		return 1;
	}
	std::cout << "Created server socket" << std::endl;
	sockaddr_in Addr;
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(PORT);
	Addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(Socket, (struct sockaddr*)&Addr, sizeof(Addr)) == -1){
		std::cerr << "Enable to bind socket " << std::endl;
	}
	std::cout << "Binded socket" << std::endl;
	if (listen(Socket, MAX_CLIENTS) == -1){
		std::cerr  << "Could not listen on port: " << PORT << std::endl;
		return 1;
	}
	std::cout << "Listening on port " << PORT << std::endl;
	while(true){
		sockaddr_in CAddr;
		socklen_t CAddrLength = sizeof(CAddr);
		int ClientSocket = accept(Socket, reinterpret_cast<struct sockaddr*>(&CAddr), &CAddrLength);
		if (ClientSocket == -1){
			std::cerr << "Error accepting client! " << std::endl;
		}
		std::cout << "Accepted " << ClientSocket << std::endl;
		if (getpeername(ClientSocket, reinterpret_cast<struct sockaddr*>(&CAddr), &CAddrLength) == 0){
			char ClientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &CAddr.sin_addr, ClientIP, sizeof(ClientIP));
			std::cout << "Client " << ClientSocket << "Connected" << ClientIP << std::endl;
	                std::thread ClientThread(HandleClient, ClientSocket, ClientIP);
                	ClientThread.detach();

		}
	}

	return 0;
}
