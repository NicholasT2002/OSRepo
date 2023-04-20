#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"

int main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{
    // Add the signal handler for SIGCHLD
    signal(SIGCHLD, SIG_IGN);

    int fd;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            pid_t pid = fork();
            printf("pid: %d\n", (int)pid);
            if (pid != 0)
            { // Parent
                // Nothing goes here
                wait(pid);
            }
            if (pid == 0)
            { // Child
                printf("Child begins\n");
                // Open file
                if ((fd = open(argv[i], O_RDWR)) >= 0)
                {
                    char *buf = malloc(WIDTH * WIDTH);
                    char *temp = malloc(WIDTH * WIDTH);
                    // read number of bytes in the line (not counting spaces?) (read until \n)
                    int bytes_read = read(fd, buf, (WIDTH * WIDTH));
                    printf("test");
                    printf("Bytes Read: %d\n", bytes_read);
                    printf("test2");
                    if (bytes_read == -1)
                    {
                        perror("Read failed");
                    }
                    else
                    {
                        int current = 0;
                        for (int i = 0; i < WIDTH; i++)
                        {
                            for (int j = 0; j < WIDTH; j++)
                            {
                                int slot = (i * WIDTH) + j;
                                if (buf[current] != '\n')
                                {
                                    temp[slot] = buf[current];
                                }
                                else
                                {
                                    for (int k = 0; k < WIDTH - j; k++)
                                        temp[slot + k] = '0';
                                    // temp[slot + WIDTH - j] = '\n';
                                    j = WIDTH;
                                }
                                current++;

                                if (j == WIDTH - 1)
                                { // check if at end of line in file, moves to next line
                                    while (buf[current] != '\n')
                                    {
                                        current++;
                                        //printf("%s\n", &buf[current]);
                                        //printf("Why does this one specifically work?\n");
                                    }
                                    current++;
                                }
                            }
                        }
                        temp[WIDTH * WIDTH] = '\0';
                        fd = open("/dev/mapDriver", O_RDWR);
                        if (fd == -1) {
                            perror("Error");
                            exit(0);        
                        }
                        int bytes_written = write(fd, temp, (WIDTH * WIDTH));
                        printf("Bytes Written: %d\n", bytes_written);
                        if (bytes_written == -1)
                            perror("Write not right");
                    }
                    printf("%s\n", "Close");
                    //close(fd);
                    // Print the new buffer
                    char *toPrint = malloc(WIDTH * WIDTH);
                    bytes_read = read(fd, toPrint, (WIDTH * WIDTH));
                    printf("Bytes Read: %d\n", bytes_read);
                    printf("%s\n", toPrint);
                    close(fd);
                }
                else
                {
                    perror("open(/dev/mapDriver) failed");
                    exit(1);
                }
            }
        }
    }
    else
    {
        // fork then,
        // execve
        pid_t pid = fork();
        if (pid != 0)
        { // Parent
            // Nothing goes here
            wait(pid);
        }
        else
        { // Child
            execve("genmap.sh", argv, envp);
        }
    }
    exit(0);
}
