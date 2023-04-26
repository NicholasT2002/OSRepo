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

int main(argc, argv)
int argc;
char *argv[];
{
    int sock_fd, new_socket, c;
    char* ip = DEFAULT_IP;
    struct sockaddr_in server, client;

    if (argc > 1)
        ip = argv[1];

    //Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0)) < 0)
        perror("Could not create server socket");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(DEFAULT_PORT);

    //Bind socket
    if (bind(sock_fd, (struct sockaddr*)&server, sizeof(server)) < 0)
        perror("Server bind failed");

    //Listen
    listen(sock_fd, 3);

    //Accept incoming connections
    c = sizeof(struct sockaddr_in);

    while (new_socket = accept(sock_fd, (struct sockaddr*)&client, (socklen_t*)&c))
	{
		// Try again if no connections
		if(errno == EAGAIN || errno == EWOULDBLOCK) { continue; }

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
            if (recv(sock_fd, client_message, 3000, 0) < 0) 
			{
                server_reply[0] = 'E';
                server_reply[1] = (char)sizeof(err_msg);
                for (int i = 2; i < sizeof(err_msg) + 3; i++)
                    server_reply[i] = err_msg[i];
                if (send(sock_fd, server_reply, strlen(server_reply), 0) < 0) {
                    perror("Server Sending failed");
                    return 1;
                }
                perror("Server Receving failed");
            }

			// Send error if message is of incorrect format
            else if (client_message[0] != 'M') 
			{
                server_reply[0] = 'E';
                server_reply[1] = (char)sizeof(err_msg);
                for (int i = 2; i < sizeof(err_msg) + 3; i++)
                    server_reply[i] = err_msg[i];
                if (send(sock_fd, server_reply, strlen(server_reply), 0) < 0) {
                    perror("Server Sending failed");
                    return 1;
                }
                perror("Server Received non-M message\n");
            }

            else 
			{
				int requestedWidth, requestedHeight;

				// If client gave no parameters, send default-sized map
                if (client_message[1] == 0) 
				{
					requestedWidth = WIDTH;
					requestedHeight = HEIGHT;
                }

				// Send map of size that client wants
                else 
				{
                    requestedWidth = client_message[3];
                    requestedHeight = client_message[4];
                }

                server_reply[0] = 'M';
                server_reply[1] = requestedWidth;
                server_reply[2] = requestedHeight;

                char *buf = malloc(requestedWidth * requestedHeight);
                int fd = open("/dev/mapDriver", O_RDWR);

                if (fd == -1)
                {
                    perror("Error opening driver");
                    exit(0);
                }
                
                int bytes_read = read(fd, buf, (requestedWidth * requestedHeight));

                if (bytes_read == -1)
                {
                    perror("Read failed");
                }

                for (int i = 3; i < bytes_read + 3; i++)
                    server_reply[i] = buf[i - 3];

                if (send(sock_fd, server_reply, strlen(server_reply), 0) < 0) 
				{
                    perror("Server Sending failed");
                    return 1;
                }

                close(fd);
            }
        }
    }

    return 0;
}
