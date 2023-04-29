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
	const int MSG_SIZE = 3000, PARAM_INDEX_1 = 4, PARAM_INDEX_2 = 8;

    int listenSocket, clientSocket, c;
    char* ip = DEFAULT_IP;
    struct sockaddr_in server, client;

    if (argc > 1)
	{
		ip = argv[1];
	}

    FILE* logFile = fopen("mapServer.log", "a");

    if(errno == -1)
	{
		perror("Could not open logFile for Client");
		return 1;
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

        pid_t pid = fork();

		// Wait if parent
        if (pid != 0) { wait(pid); }

        else
        {
            char* client_message = malloc(MSG_SIZE), *server_reply = malloc(MSG_SIZE);
			struct smsg_t* msg = malloc(sizeof(struct smsg_t));

			// Try to get message, send error if we don't
            if (recv(clientSocket, client_message, MSG_SIZE, 0) < 0)
			{
				perror("Server Receiving failed");

				char* err_msg = "Failed to receive client message";

				msg->c = 'E';
				msg->width = strlen(err_msg) + 1;				

                for (int i = 0; i < strlen(err_msg); i++)
				{
					msg->message[i] = err_msg[i];
				}

				msg->message[strlen(err_msg)] = '\0';
                    
                if (send(clientSocket, (char*)msg, sizeof(struct smsg_t), 0) < 0) 
				{
                    perror("Server Sending failed");
                }

				exit(-1);
            }

			// Send error if message is of incorrect format
            else if (client_message[0] != 'M') 
			{
				char* err_msg = "Message is in incorrect format";

				msg->c = 'E';
				msg->width = strlen(err_msg) + 1;				

                for (int i = 0; i < strlen(err_msg); i++)
				{
					msg->message[i] = err_msg[i];
				}

				msg->message[strlen(err_msg)] = '\0';
                    
                if (send(clientSocket, (char*)msg, sizeof(struct smsg_t), 0) < 0) 
				{
                    perror("Server Sending failed");
                }

                printf("Server received message in incorrect format\n");
				exit(-1);
            }

            else 
			{
				int requestedWidth, requestedHeight;

                fprintf(logFile, "Received Message from Client: ");
                fprintf(logFile, "%c %d %d\n", client_message[0], client_message[PARAM_INDEX_1], client_message[PARAM_INDEX_2]);

				msg->c = 'M';

				// If client gave no parameters, send default-sized map
                if (client_message[PARAM_INDEX_1] == 0) 
				{
					msg->width = WIDTH;
					msg->height = HEIGHT;
                }

				// Send map of size that client wants
                else 
				{
                    msg->width = client_message[PARAM_INDEX_1];
                    msg->height = client_message[PARAM_INDEX_2];
                }

				printf("%d %d\n", msg->width, msg->height);

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

                if (send(clientSocket, (char*)msg, sizeof(struct smsg_t), 0) < 0) 
				{
                    perror("Server Sending failed");
					close(fd);
                    exit(-1);
                }
                
                fprintf(logFile, "Sent Message to Client: ");
                fprintf(logFile, "%c %d %d\n%s", msg->c, msg->width, msg->height, msg->message);

                close(fd);
				exit(0);
            }
        }
    }

    fclose(logFile);

    return 0;
}
