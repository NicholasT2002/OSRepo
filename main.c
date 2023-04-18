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
                if((fd = open(argv[1], O_RDWR)) >= 0) //no idea how to choose which argv[] from the child
	            {
                    int height = 0;
		            //For each line (repeat for allotted length)
                    for (int i = 0; i < WIDTH; i++) {
                        char* buf;
                        char* temp;
                        //read number of bytes in the line (not counting spaces?) (read until /n)
                        int bytes_read = read(fd, buf, WIDTH);
                        int bytes_written = 0;
                        //check if number of bytes is less or more than allotted width (including /n))
                        if (bytes_read == 0) { //if bytes_read is 0 or /0 occurs just print # allotted width times for the rest of the allotted length
                            for (int i = 0; i < WIDTH; i++) {
                                temp[i] = '0';
                            }
                            temp[WIDTH] = '\n';
                            bytes_written = write(fd, temp, WIDTH);
                        }
                        else if (bytes_read == -1) {
                            perror("read failed");
                        }
                        else if (bytes_read < WIDTH) { //if less, just print line and then add # missing number of bytes
                            for (int i = 0; i < bytes_read; i++) {
                                temp[i] = buf[i];
                            }
                            for (int i = bytes_read; i < WIDTH; i++) {
                                temp[i] = '0';
                            }
                        }
                        else { //if more, print only up to allotted width
                            for (int i = 0; i < bytes_read - 1; i++) {
                                temp[i] = buf[i];
                            }
                            temp[WIDTH] = '\n';
                            bytes_written = write(fd, temp, WIDTH);
                        }
                        height++;
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
    } else {
        //fork then,
        //execve
        //pid_t pid = fork();
        execve("/dev/genmap.sh", argv, envp);
    }
    exit(0);
}

/* EOF */
