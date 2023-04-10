/* The necessary header files */

/* Standard in kernel modules */
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */

/* For character devices */
#include <linux/fs.h>       /* The character device
                             * definitions are here
                             */

//#include <asm/uaccess.h>  /* for put/get_user */

#include <linux/ioctl.h> //??? idk
#define RESET_IOCTL _IOW(1, 1, int)
#define ZERO_IOCTL _IOW(1, 2, int)
#define CHECK_IOCTL _IOR(1, 3, int)

/* Return codes */
#define SUCCESS      0


/* Device Declarations **************************** */

/* The maximum length of the message from the device */
#define DRV_BUF_SIZE 3000

/* The name for our device, as it will appear
 * in /proc/devices
 */
#define DEVICE_NAME  "/dev/asciimap"

/*
 * Driver status structure
 */
typedef struct _driver_status
{
	/* An ASCII character to be outputted in the next read
	 */
	char  curr_char;

	/* Is the device open right now? Used to prevent
	 * concurent access into the same device
	 */
	bool  busy;

    /* The length for the current actual map
     */
    int buf_length;

	/* The message the device will give when asked */
	char  buf[DRV_BUF_SIZE];

	/* How far did the process reading the message
	 * get? Useful if the message is larger than the size
	 * of the buffer we get to fill in device_read.
	 */
	char* buf_ptr;

	/* The major device number for the device.
	 */
	int   major;

	/* The minor device number for the device.
	 */
	int   minor;
} driver_status_t;

/*
 * Driver funcitons' prototypes
 */
static int device_open(struct inode*, struct file*);
static int  device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);
static ssize_t device_lseek(struct file*, loff_t*, int whence);
static ssize_t device_ioctl(struct file*, const char*, uint, unsigned long);

/* This structure will hold the functions to be
 * called when a process does something to the device
 * we created. Since a pointer to this structure is
 * kept in the devices table, it can't be local to
 * init_module. NULL is for unimplemented functions.
 */
struct file_operations Fops =
{
	.open = device_open, /* open */
	.read = device_read, /* read */
	.write = device_write, /* write */
	.release = device_release,  /* a.k.a. close */
    .llseek = device_lseek,   /* seek */
    .unlocked_ioctl = device_ioctl   /* ioctl */
#if 0
	.owner = NULL,   /* owner */
	.readdir = NULL,   /* readdir */
	.poll = NULL,   /* poll/select */
	.mmap = NULL,   /* mmap */
	.flush = NULL,   /* flush */
#endif
};
