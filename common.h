/****
 * common.h
 *
 * Contains generic common global defines.
 * Serguei Mokhov
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/* Genereic Buffer Size */
/* may be made up from the air */
#define BSIZE             1024

/* All these are to be treated as blanks or delimiters */
#define BLANK_CHARS       " \n\t\r'"

/* Every time period (in seconds) interrogate the server */
#define BUG_SERVER_PERIOD 10

/* Timeout (in sec) */
#define TIMEOUT_RETRIES   3
#define TIMEOUT           30


/* Debugging / Logging Macros */

//#include <stdio.h>
//#include <stdlib.h> /* exit() */
//#include <time.h>   /* time(), ctime() */

#include <linux/ioctl.h>
#define RESET_IOCTL _IOW(1, 1, int)
#define ZERO_IOCTL _IOW(1, 2, int)
#define CHECK_IOCTL _IOR(1, 3, int)

#define WIDTH   50
#define HEIGHT  50

#define DEFAULT_PORT 23032
#define DEFAULT_IP "127.0.0.1"

struct smsg_t
{
	char c;
	int width, height;
	char message[3000];
};

/* Debugging/error/logging functions */
/* TODO: implement them */
void print(char* format, ...);
void _log(int fd, char* format, ...);
//void slog(FILE* stream, char* format, ...);
void elog(int fd, char* format, ...);
void perrexit(char* format, ...);

/*
 * Macros act either as wrappers around functions or
 * as real macros. The real macros assume stdin, stdout, and stderr, which is not
 * always desirable. Functions require explcite file to work with. Not finished.
 */

#ifdef _PRINT
#	define PRINT(format, args...)    {fprintf(stdout, format, ## args); fflush(stdout);}
#else
#	define PRINT(format, args...) ;
#endif /* _PRINT */

#if defined(_LOG) || defined (_ELOG) || defined(_OLOG)
#	define LOG(fd, format, args...)  {time_t t = time(NULL); char b[25]; strcpy(b, ctime(&t)); b[24]=0; fprintf(fd, "[%s]: ", b); fprintf(fd, format, ## args); fflush(fd);}
#else
#	define LOG(fd, format, args...) ;
#endif /* _LOG */

#ifdef _ELOG
#	define ELOG(format, args...)     LOG(stderr, format, ## args)
#else
#	define ELOG(format, args...) /**/
#endif /* _ELOG */

#ifdef _OLOG
#	define OLOG(format, args...)     LOG(stdout, format, ## args)
#else
#	define OLOG(format, args...) /**/
#endif /* _OLOG */

#ifdef _PERREXIT
#	define PERREXIT(format, args...) {fprintf(stderr, format, ## args); perror(""); exit(1);}
#else
#	define PERREXIT(format, args...) ;
#endif /* _PERREXIT */


#ifdef _DEBUG
#	define DEBUG(format, args...) {fprintf(stderr, format, ## args); fflush(stderr);}
#else
#	define DEBUG(format, args...) /**/
#endif /* _DEBUG */

#endif /* _COMMON_H_ */
