#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

const int MAX_CLIENTS = 20;
std::vector<int> Clients;

void HandleClient(int ClientSocket, char ClientIP[INET_ADDRSTRLEN], std::string username){
	char Buffer[1024];
	std::string IncoMsg;
	while (true){
		int BytesRecieved = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (BytesRecieved <= 0){
			std::cout << "Client " << ClientSocket << " " << ClientIP << " Disconnected" << std::endl;
			std::string anoun = username + " has left the chat...";
			broadcast(anoun);
			close(ClientSocket);
			break;
		}
		std::string RecuData(Buffer, BytesRecieved);
		std::string CompMsg = IncoMsg + RecuData;
		std::cout << ClientSocket << " : ";
		std::cout << Buffer << std::endl;
		std:: string message  = username + " : " + Buffer;
		for (int i = 0; i  < Clients.size() ; i++){
			if (Clients[i] != ClientSocket){
			
				std::cout << "Sent to " << Clients[i] << std::endl;
				send(Clients[i], message.c_str(), message.length(), 0);
				}
			}	
		memset(Buffer, 0, sizeof(Buffer));
		IncoMsg = CompMsg;
	}
}

void broadcast(std::string message){
	for (int i = 0; i < Clients.size(); i++){
		send(Clients[i], message.c_str(), message.length(), 0);
	}
}

int main(void){
	int PORT;
	std::cout << "What port: ";
	std::cin >> PORT;
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
		std::string username;
		int ClientSocket = accept(Socket, reinterpret_cast<struct sockaddr*>(&CAddr), &CAddrLength);
		if (ClientSocket == -1){
			std::cerr << "Error accepting client! " << std::endl;
		}
		Clients.push_back(ClientSocket);
		std::cout << "Accepted " << ClientSocket << std::endl;
		if (getpeername(ClientSocket, reinterpret_cast<struct sockaddr*>(&CAddr), &CAddrLength) == 0){
			std::string username;
			char ClientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &CAddr.sin_addr, ClientIP, sizeof(ClientIP));
			char initbuff[1024];
			int bytes = recv(ClientSocket, initbuff, sizeof(initbuff), 0);
			if (bytes <= 0){
				std::cout << "faild attempted connexion with " << ClientIP << std::endl;
				close(ClientSocket);
			}
			if (initbuff[0] == '#' && initbuff[1] == '?' && initbuff[2] == '>'){
				username = initbuff;
			}
			std::string welcome = username + " has joined the chat";
			memset(initbuff, 0, sizeof(initbuff));
			std::cout << "Client " << ClientSocket << " Connected " << ClientIP << " as " << username << std::endl;
			broadcast(welcome);
	        std::thread ClientThread(HandleClient, ClientSocket, ClientIP, username);
        	ClientThread.detach();

		}
	}

	return 0;
}
