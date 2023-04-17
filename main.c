#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"

int
main(argc, argv)
	int   argc;
	char* argv[];
{
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            pid_t pid = fork();

            if (pid != 0) { //Parent
                //Nothing goes here
            }
            else { //Child
                //Open file
                //For each line (repeat for allotted length)
                    //read number of bytes in the line (not counting spaces?) (read until /n)
                    //check if number of bytes is less or more than allotted width (including /n))
                        //if less, just print line and then add # missing number of bytes
                        //if more, print only up to allotted width
                //if bytes_read is 0 or /0 occurs just print # allotted width times for the rest of the allotted length
            }
        }
    else {
        //fork then,
        //execve 
        char* envp[];
        pid_t pid = fork();
        execve("/dev/genmap.sh", argv[], envp[]);
    }
    exit(0);
}

/* EOF */
