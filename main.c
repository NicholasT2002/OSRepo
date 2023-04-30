#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "common.h"

int main(int argc, char* argv[], char* envp[])
{
    // Add SIGCHLD Handler
    signal(SIGCHLD, SIG_IGN);

    int fd;

	// If we got a file, turn it into map
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            pid_t pid = fork();

			// Do nothing if parent
            if (pid != 0) { wait(pid); }

            else
            {
                // Open file
                if ((fd = open(argv[i], O_RDWR)) == -1)
                {
                    perror("Error opening file");
                    exit(-1);
                }

                char *buf = malloc(WIDTH * WIDTH), *temp = malloc(WIDTH * WIDTH);
		
                // Read file
                if (read(fd, buf, (WIDTH * WIDTH)) == -1)
                {
                    perror("Read failed\n");
					close(fd);
					exit(-1);
                }

                int currentIndex = 0, fileIndex = 0;
				bool lineEnded = false, fileEnded = false;

                for (int i = 0; i < WIDTH; i++)
                {
					lineEnded = false;

                    for (int j = 0; j < WIDTH; j++)
                    {
						// If line or file ended, just add 0s
						if(fileEnded || lineEnded)
						{
							temp[currentIndex] = '0';
						}

						// When file ends, just add 0s
						else if(buf[fileIndex] == '\0')
						{
							fileEnded = true;
							temp[currentIndex] = '0';
						}

						// When line ends, fill lines with 0s
						else if (buf[fileIndex] == '\n')
						{
							lineEnded = true;
							temp[currentIndex] = '0';
						}

						// Otherwise just copy over
						else
						{
							temp[currentIndex] = buf[fileIndex];
							fileIndex++;
						}

						currentIndex++;
                    }

					// If line ended, move to next line
					if(lineEnded) { fileIndex++; }

					// Add newline
					temp[currentIndex - 1] = '\n';
                }

				// Add null terminator
                temp[WIDTH * WIDTH] = '\0';

                close(fd);

				// Open driver
                fd = open("/dev/mapDriver", O_RDWR);
                if (fd == -1)
                {
                    perror("Error opening driver");
                    exit(-1);
                }

				// Write to driver
                if (write(fd, temp, (WIDTH * WIDTH)) == -1)
				{
					perror("Error writing to driver");
					exit(-1);
				}

                // Print the new map
                char *toPrint = malloc(WIDTH * WIDTH);

				// Read from driver
				if(read(fd, toPrint, (WIDTH * WIDTH)) < 0)
				{
					perror("Error reading driver");
					exit(-1);
				}

                printf("%s\n", toPrint);
                close(fd);
				exit(0);
            }
        }
    }

	// If no given files, just use shell script
    else
    {
        pid_t pid = fork();

		// Parent does nothing
        if (pid != 0) { wait(pid); }

        else
        {
            execve("genmap.sh", argv, envp);
			exit(0);
        }
    }

    exit(0);
}
