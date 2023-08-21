#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
const int PORT = 5591;
const int MAX_CLIENTS = 2;
std::vector<int> clients;

void handleClient(int CSocket){
	std::cout << "Handling" << std::endl;
	char buffer[1024];
	while(true){
		int BRecu = recv(CSocket, buffer, sizeof(buffer), 0);
		if (BRecu <= 0){
			std::cout << "Client" << CSocket << "disconnected";
			close(CSocket);
			break;
		}
		std::cout << "sending" << std::endl;
		send(CSocket, buffer, BRecu, 0);
		std::cout << "sent" << std::endl;
		//for (int i = 0; i < clients.size(); ++i){
		//	if (clients[i] != CSocket){
		//	std::cout << "Sent" << std::endl;
		//	send(clients[i], buffer, BRecu, 0);
		//	}
		//}
	}
}
int main(){

	int Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == -1){
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	sockaddr_in Addr;
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(PORT);
	Addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(Socket, (struct sockaddr*)&Addr, sizeof(Addr)) == -1){
		std::cerr << "Error binding !" << std::endl;
		return 1;
	}
	if (listen(Socket, MAX_CLIENTS) == -1){
		std::cerr << "Error listening !" << std::endl;
		return 1;
	}
       	std::cout << "Server listening on port " << PORT << "..." << std::endl;
	while (true){
		int CSocket = accept(Socket, NULL, NULL);
		if (CSocket == -1){
			std::cerr << "Error accepting client! " << std::endl;
			continue;
		}
		std::thread Cthread(handleClient, CSocket);
		Cthread.detach();
	}
	close(Socket);	
	return 0;
}
