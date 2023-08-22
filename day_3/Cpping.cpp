#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef struct {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint32_t data;
} ICMP_Header_t;

void Help(char * arg){
	std::cout << "Usage:\n\t- Ping:\t\t " << arg << " <distination ip>\n\t- Show help:\t\t " << arg << " --help" << std::endl;
}
int Ping(int Socket, ICMP_Header_t Packet, sockaddr_in Addr, char DistIP[16]){
	if (sendto(Socket, &Packet, sizeof(Packet), 0, (struct sockaddr *)&Addr, sizeof(Addr)) == -1){
		std::cerr << "Could not send echo request" << std::endl;
		exit(1);
	}
	std::cout << "ICMP echo request sent to " << DistIP << std::endl;
	socklen_t Addrlen = sizeof(Addr);
	char ResponseBuffer[1024];
	ssize_t ResponseSize = recvfrom(Socket, ResponseBuffer, sizeof(ResponseBuffer), 0, (struct sockaddr *)&Addr, &Addrlen);
	if (ResponseSize == -1){
		std::cout << "No response recieved " << std::endl;
	} else {
		std::cout << "Recieved " << ResponseSize << " bytes from " << DistIP << "\nSuccess"<< std::endl;
	}
	return 0;
}
int main(int argc, char* argv[]){
	if (argc > 2 || strcmp(argv[1], "--help") == 0){
		Help(argv[0]);
		exit(1);
	}
	int Socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (Socket == -1){
		std::cerr << "Error creating socket" << std::endl;
		exit(1);
	}
	ICMP_Header_t Packet;
	Packet.type = 8;
	Packet.code = 0;
	Packet.checksum = 0xfff7;
	Packet.data = 0;
	sockaddr_in Addr;
	Addr.sin_family = AF_INET;
	Addr.sin_port = 0;
	Addr.sin_addr.s_addr = inet_addr(argv[1]);
	std::cout << "Ping " << argv[1] << " with 8 bytes of data." << std::endl;
	Ping(Socket, Packet, Addr, argv[1]);
	return 0;
}
