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

struct msg_t
{
	char c;
	int width, height;
};

int main(int argc, char* argv[])
{
    int sock_fd;
    char* ip = DEFAULT_IP;
    struct sockaddr_in server;

    if (argc > 1)
	{
		ip = argv[1];
	}

    FILE* logFile = fopen("mapClient.log", "a");

    if(errno == -1)
	{
		perror("Could not open logFile for Client");
		return 1;
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

	// Create message
	struct msg_t* message = malloc(sizeof(struct msg_t));
	message->c = 'M';
	message->width = 0;
	message->height = 0;

	// Send message
    if (send(sock_fd, (char*)message, sizeof(struct msg_t), 0) < 0) 
	{
        perror("Client Sending failed");
        return 1;
    }
    else 
    {
        fprintf(logFile, "Sent Message to Server: ");
        fprintf(logFile, "%c %d %d\n", message->c, message->width, message->height);
    }

    //Receive Messages
	const int REPLY_SIZE = 3000;
	ssize_t bytesReceived;
    char server_reply[REPLY_SIZE];
	memset(server_reply, 0, sizeof(server_reply));

    while((bytesReceived = recv(sock_fd, server_reply, REPLY_SIZE, 0)) < 0)
	{
		if(errno == -1)
		{
			perror("Client receiving failed");
			return 1;
		}
	}

	// Copy message from server
	struct smsg_t* reply = malloc(sizeof(struct smsg_t));
	memcpy(reply, server_reply, bytesReceived);

	if(server_reply[0] == 'M')
	{
		printf("%c %d %d %s", reply->c, reply->width, reply->height, reply->message);

        fprintf(logFile, "Received Message from Server: ");
        fprintf(logFile, "%c %d %d %s", reply->c, reply->width, reply->height, reply->message);
	}

	else if (server_reply[0] == 'E')
	{
		printf("%c %d\n%s\n", reply->c, reply->width, reply->message);

        fprintf(logFile, "Received error from Server: ");
        fprintf(logFile, "%c %d\n%s\n", reply->c, reply->width, reply->message);
	}

	else
	{
		printf("Received message from server in incorrect format:\n%c %d %d\n%s", reply->c, reply->width, reply->height, reply->message);

        fprintf(logFile, "Received message from server in incorrect format:\n");
        fprintf(logFile, "%c %d %d\n%s", reply->c, reply->width, reply->height, reply->message);
	}

    // Close Socket and file
    close(sock_fd);
    fclose(logFile);

    return 0;
}
