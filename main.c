#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdio.h>
#include <unistd.h>

#include "common.h"

int
main(argc, argv, envp)
	int   argc;
	char* argv[];
    char* envp[];
{
    int fd;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            pid_t pid = fork();

            if (pid != 0) { //Parent
                //Nothing goes here
            }
            else { //Child
                //Open file
                if((fd = open(argv[1], O_RDWR)) >= 0)
	            {
                    char* buf;
                    char* temp;
                    //read number of bytes in the line (not counting spaces?) (read until \n)
                    int bytes_read = read(fd, buf, (WIDTH*WIDTH));

                    if (bytes_read == -1) {
                        perror("read failed");
                    }
                    else {
                        int current = 0;
                        for(int i = 0; i < WIDTH; i++) {
                            for (int j = 0; j < WIDTH; j++) {
                                int slot = (i * WIDTH) + j;
                                if(buf[current] != '\n') {
                                    temp[slot] = buf[current];
                                }
                                else {
                                    for (int k = 0; k < WIDTH - j; k++)
                                        temp[slot + k] = '0';
                                    j = WIDTH;
                                }
                                current++;

                                if (j == WIDTH - 1) { //check if at end of line in file, moves to next line
                                    while (buf[current] != '\n')
                                        current++;
                                    current++;
                                }
                            }
                        }
                        //temp[WIDTH*WIDTH] = '\0';
                        write(fd, temp, (WIDTH*WIDTH));
                    }

		            close(fd);
	            }
	            else
	                {
		            perror("open(/dev/CSI230ASCII) failed");
		            exit(1);
	            }
            }
        }

        //Print the new buffer
        char* toPrint;
        int bytes_read = read(fd, toPrint, (WIDTH*WIDTH));
        printf(toPrint);
    } else {
        //fork then,
        //execve
        pid_t pid = fork();
        if (pid != 0) { //Parent
            //Nothing goes here
        }
        else { //Child
            execve("genmap.sh", argv, envp);
        }
    }
    exit(0);
}

/* EOF */
