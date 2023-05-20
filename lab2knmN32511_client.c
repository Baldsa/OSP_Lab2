#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define CHECK_RESULT(res, msg)			\
do {									\
	if (res < 0) {						\
		perror(msg);					\
		exit(EXIT_FAILURE);				\
	}									\
} while (0)

#define BUF_SIZE		1024

const char *program_name;
int main(int argc, char *argv[])
{	
	int clientSocket;
	program_name = argv[0];
	char* debug = getenv("LAB2DEBUG");
	char *ip_val = "127.0.0.1";
    int port = 7777;
    int option;
    bool help = false, version = false;
	char buffer[BUF_SIZE] = {0}; 
	struct sockaddr_in serverAddr  = {0};
	socklen_t addr_size; 
	while ((option = getopt(argc,argv, "hva:p:")) != -1) {
        switch (option) {
            case 'a':
                if (optarg != NULL){
                    ip_val = optarg;
                    if (debug){
                        printf("IP : %s\n", ip_val);
                    }
                }   
                break;
            case 'p':
                if (optarg != NULL) {
                    port = atoi(optarg);
                    if (debug){
                        printf("PORT: %d\n", port);
                    }
            } 
                break;
            case 'h':
                help = true;
                break;
            case 'v':
                version = true;
                break;
            case '?':
                printf("Oops, your guess is as good as mine, what happend ^_^ 0_o ");
                break;
        }
    }
    if (help){
        printf("Usage: %s [OPTION]...\n", program_name);
        printf("Options:\n");
        printf(" -h, display this help and exit\n");
        printf(" -v, output version information and exit\n");
        printf(" -a, set custom IP address\n");
        printf(" -p , set custom port address \n");
        exit(EXIT_SUCCESS);
    }
    if (version){
        printf("%s version 1.0\n", program_name);
        printf("info: A client that sends a domain to the server and receives it's IP addresses in response\n");
        printf("Author: Kopylov Nikita\n");
        printf("Group: N32511\n");
        printf("Lab: 2 variant 19\n");
        exit(EXIT_SUCCESS);
    }


	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	CHECK_RESULT(clientSocket, "socket");
	 if (inet_pton(AF_INET, ip_val, &(serverAddr.sin_addr)) <= 0) {
        printf("Error: Invalid IP address.\n");
        return 1;
    }
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	printf("Data sent: ");
	char* res_f; 
	res_f = fgets(buffer, sizeof(buffer), stdin);
	if (res_f == NULL){
		perror ("fgets");
		exit(EXIT_FAILURE);	
	}
	char *pos = strchr(buffer, '\n');
	if (pos) *pos = '\0';
	int res = sendto(clientSocket, buffer, strlen(buffer), 0, 
		(const struct sockaddr*)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "sendto");
	int size = 0; 
	memset(buffer, 0, sizeof(buffer));
	res = recvfrom(clientSocket, &size, sizeof(size), 0, NULL, NULL);	
	printf("Buffer size [%d]\n",size);
	memset(buffer, 0, sizeof(buffer));
	for (int i = 0; i < size; i++) {
        res = recvfrom(clientSocket, buffer, BUF_SIZE, 0, (struct sockaddr*)&serverAddr, &addr_size);
        CHECK_RESULT(res, "recvfrom");
        printf("Received IP address: %s\n", buffer);
        memset(buffer, 0, BUF_SIZE);
    }
	close(clientSocket);
	return 0;	
}
