#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h> 

#include "common.h"
#include "mapServer.h"

int main(int argc, char * argv[])
{
    int listenSocket, clientSocket, c;
    char* ip = DEFAULT_IP;
    struct sockaddr_in server, client;

    if (argc > 1)
	{
		ip = argv[1];
	}

    // Create socket
    if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Could not create server socket");
		return 1;
	}
        
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(DEFAULT_PORT);

    // Bind socket
    if (bind(listenSocket, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		perror("Server bind failed");
		exit(-1);
	}

    // Listen
    listen(listenSocket, 3);

    // Accept incoming connections
    c = sizeof(struct sockaddr_in);

    while ((clientSocket = accept(listenSocket, (struct sockaddr*)&client, (socklen_t*)&c)))
	{
		if(clientSocket == -1)
		{
			perror("Server accepting failed");
			continue;
		}

        printf("Received Message\n");
        pid_t pid = fork();

        if (pid != 0)
        { // Parent
            wait(pid);
        }

        else
        { // Child
            char* client_message = malloc(3000);
            char* server_reply = malloc(3000);
            char* err_msg = "Server failed to read client message";

			// Try to get message, send error if we don't
            if (recv(clientSocket, client_message, 3000, 0) < 0) 
			{
                server_reply[0] = 'E';
                server_reply[1] = (char)sizeof(err_msg);

                for (int i = 2; i < sizeof(err_msg) + 3; i++)
				{
					server_reply[i] = err_msg[i];
				}
                    
                if (send(clientSocket, server_reply, sizeof(server_reply), 0) < 0) 
				{
                    perror("Server Sending failed");
                    exit(-1);
                }

                perror("Server Receving failed");
            }

			// Send error if message is of incorrect format
            else if (client_message[0] != 'M') 
			{
                server_reply[0] = 'E';
                server_reply[1] = (char)sizeof(err_msg);

                for (int i = 2; i < sizeof(err_msg) + 3; i++)
				{
					server_reply[i] = err_msg[i];
				}

                if (send(clientSocket, server_reply, sizeof(server_reply), 0) < 0) 
				{
                    perror("Server Sending failed");
                    exit(-1);
                }

                perror("Server Received non-M message");
            }

            else 
			{
				struct smsg_t* msg = malloc(sizeof(struct smsg_t));

				int requestedWidth, requestedHeight;

				// If client gave no parameters, send default-sized map
                if (client_message[1] == 0) 
				{
					msg->width = WIDTH;
					msg->height = HEIGHT;
                }

				// Send map of size that client wants
                else 
				{
                    msg->width = client_message[1];
                    msg->height = client_message[2];
                }
				msg->c = 'M';
                //server_reply[0] = 'M';
                //server_reply[1] = requestedWidth;
                //server_reply[2] = requestedHeight;

                char *buf = malloc(msg->width * msg->height);

                int fd = open("/dev/mapDriver", O_RDWR);

                if (fd == -1)
                {
                    perror("Error opening driver");
                    exit(-1);
                }

                int bytes_read = read(fd, buf, (msg->width * msg->height));

                if (bytes_read == -1)
                {
                    perror("Read failed");
					close(fd);
					exit(-1);
                }

                for (int i = 0; i < bytes_read; i++)
				{
					msg->message[i] = buf[i];
				}

				msg->message[bytes_read] = '\0';

                if (send(clientSocket, (char*)msg, bytes_read + sizeof(char) + (2 * sizeof(int)), 0) < 0) 
				{
                    perror("Server Sending failed");
					close(fd);
                    exit(-1);
                }

                close(fd);
				exit(0);
            }
        }
    }

    return 0;
}
