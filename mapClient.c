#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h> 

#include "common.h"
#include "mapClient.h"

int main(int argc, char* argv[])
{
    int sock_fd;
    char* ip = DEFAULT_IP;
    struct sockaddr_in server;

    if (argc > 1)
	{
		ip = argv[1];
	}

    //Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Client Socket could not be created");
	}
        

    //Connect to server
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) 
	{
        perror("Client Connection Error");
        return 1;
    }

    //Send Messages
    char* message = malloc(sizeof(char)+sizeof(int));
    //char* mMessage = malloc(sizeof(char)+sizeof(int)*2); //For if Specifying width/height

    message = 'M' + (char*)0;
    if (send(sock_fd, message, sizeof(message), 0) < 0) 
	{
        perror("Client Sending failed");
        return 1;
    }

    //Receive Messages
    char* server_reply[3000];

    if (recv(sock_fd, server_reply, 3000, 0) < 0)
	{
		perror("Client receiving failed");
	}

    printf("%s\n", *server_reply);

    //Close Socket
    close(sock_fd);

    exit(0);
}
