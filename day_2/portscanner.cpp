

/*
 * HI!
 * TCP port scanner by AC-Armitz
 * Day 1 experimenting with C++
 * 
*/


#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int TCPScanner(std::string TargetIP, unsigned int FirstPort, unsigned int LastPort){	
	int NOpenPorts = 0;
	int count = 0;
	for (int i = FirstPort; i <= LastPort; i++){
		
		int TCPSocket = socket(AF_INET, SOCK_STREAM, 0);
        	if (TCPSocket == -1){
                	std::cerr << "Faild to creat TCP socket for port " << i << std::endl;
		}
		count++;
		sockaddr_in Addr;
		Addr.sin_family = AF_INET;
		Addr.sin_port = htons(i);
		Addr.sin_addr.s_addr = inet_addr(TargetIP.c_str());
		if (connect(TCPSocket, (struct sockaddr*)&Addr, sizeof(Addr)) == 0){
			std::cout << "[V] TCP Port " << i << " is open." << std::endl;
			NOpenPorts++;
		}
		close(TCPSocket);
		if (count == 250){
			usleep(500000);
		}	
	}
	return NOpenPorts;
}

int main(){
	
	std::string TargetIP;
	std::cout << "Insert target IP ";
	std::cin >> TargetIP;	
	unsigned int FirstPort;
	unsigned int LastPort;
	std::cout << "Insert begening of port range: ";
	std::cin >> FirstPort;
	std::cout << "Insert end of port range: ";
	std::cin >> LastPort;
	int OpenPorts = TCPScanner(TargetIP, FirstPort, LastPort);
	std::cout << "There are " << OpenPorts << " open ports. " << std::endl;
	return 0;
}
