#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const char* IP = "127.0.0.1";
int ServerPORT;

void HandComm(int ServerSocket){
	char buffer[1024];
	while (true){
		int BytesRecu = recv(ServerSocket, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;
		if (BytesRecu <= 0){
			std::cout << "Disconnected" << std::endl;
			close(ServerSocket);
			break;
		}
		int Counted = std::stoi(buffer);
		Counted++;
		std::string msg = std::to_string(Counted);
		send(ServerSocket, msg.c_str(), msg.size(), 0);
	}
}

int main(){
	std::cout << "Enter the server port: ";
	std::cin >> ServerPORT;
	int ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerSocket == -1){
		std::cerr << "Error creating socket!" << std::endl;
		return 1;
	}
	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(ServerPORT);
	ServerAddr.sin_addr.s_addr = inet_addr(IP);
	if (connect(ServerSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1){
		std::cerr << "Error connecting to server." << std::endl;
		return 1;
	}
	std::cout << "Connected to server on " << IP << ":" << ServerPORT << std::endl;
	char start;
	std::cout << "Insert s to start or l to listen! ";
	std::cin >> start;
	while (start != 's' && start != 'l'){
		std::cin >> start;
	}
	if (start == 's'){
		int StartCounted = 0;
		std::string msg = std::to_string(StartCounted);
		if (send(ServerSocket, msg.c_str(), msg.size(), 0)){
			std::cerr << "Error initiating communication! " << stderr(errno) << std::endl;
			return 1;
		}
	}
	HandComm(ServerSocket);
	return 0;
}
