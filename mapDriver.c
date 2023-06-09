#include "mapDriver.h"

char static_art[] =
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW-\n"
	"-NT AL GF TW NT AL GF TW NT AL GF TW NT AL GF TW\n";

/* Driver's Status is kept here */
static driver_status_t status =
	{
		'a',   /* Starting ASCII char is '0' */
		false, /* Not busy at the beginning */
		2500,
		{0},  /* buffer */
		NULL, /* buffer's ptr */
		-1,	  /* major */
		-1	  /* minor */
};

/* This function is called whenever a process
 * attempts to open the device file
 */
static int device_open(inode, file)
struct inode *inode;
struct file *file;
{
	static int counter = 0;

#ifdef _DEBUG
	printk("device_open(%p,%p)\n", inode, file);
#endif

	/* This is how you get the minor device number in
	 * case you have more than one physical device using
	 * the driver.
	 */
	status.minor = inode->i_rdev >> 8;
	status.minor = inode->i_rdev & 0xFF;

	printk(
		"Device: %d.%d, busy: %d\n",
		status.major,
		status.minor,
		status.busy);

	/* We don't want to talk to two processes at the
	 * same time
	 */
	if (status.busy)
		return -EBUSY;

	/* If this was a process, we would have had to be
	 * more careful here.
	 *
	 * In the case of processes, the danger would be
	 * that one process might have check busy
	 * and then be replaced by the schedualer by another
	 * process which runs this function. Then, when the
	 * first process was back on the CPU, it would assume
	 * the device is still not open.
	 *
	 * However, Linux guarantees that a process won't be
	 * replaced while it is running in kernel context.
	 *
	 * In the case of SMP, one CPU might increment
	 * busy while another CPU is here, right after
	 * the check. However, in version 2.0 of the
	 * kernel this is not a problem because there's a lock
	 * to guarantee only one CPU will be kernel module at
	 * the same time. This is bad in  terms of
	 * performance, so version 2.2 changed it.
	 */

	status.busy = true;

	/* Initialize the message. */
	sprintf(
		status.buf,
		"%s\n",
        static_art);

	/* The only reason we're allowed to do this sprintf
	 * is because the maximum length of the message
	 * (assuming 32 bit integers - up to 10 digits
	 * with the minus sign) is less than DRV_BUF_SIZE, which
	 * is 80. BE CAREFUL NOT TO OVERFLOW BUFFERS,
	 * ESPECIALLY IN THE KERNEL!!!
	 */

	status.buf_ptr = status.buf;

	return SUCCESS;
}

MODULE_LICENSE("GPL");
/* This function is called when a process closes the
 * device file.
 */
static int device_release(inode, file)
struct inode *inode;
struct file *file;
{
#ifdef _DEBUG
	printk("device_release(%p,%p)\n", inode, file);
#endif

	/* We're now ready for our next caller */
	status.busy = false;

	return SUCCESS;
}

/* This function is called whenever a process which
 * have already opened the device file attempts to
 * read from it.
 */
static ssize_t device_read(file, buffer, length, offset) // TODO
struct file *file;
char *buffer;	/* The buffer to fill with data */
size_t length;	/* The length of the buffer */
loff_t *offset; /* Our offset in the file */
{
	ssize_t bytes_read = 0;
	int i;

	if (length > status.buf_length)
	{
		printk("Reading failed: Requested more bytes than in buffer!");
		return 0;
	}

	for (i = 0; i < length; ++i)
	{
		put_user(status.buf[i], &buffer[i]);
		bytes_read++;
	}

	return bytes_read;
}

/* This function is called when somebody tries to write
 * into our device file.
 */
static ssize_t device_write(file, buffer, length, offset) // TODO
struct file *file;
const char *buffer; /* The buffer */
size_t length;		/* The length of the buffer */
loff_t *offset;		/* Our offset in the file */
{
	ssize_t bytes_written = 0;
	int i;

	if (length > status.buf_length)
	{
		printk("Writing Failed: Too large to fit in map\n");
		return 0;
	}

	for (i = 0; i < length; i++)
	{
		get_user(status.buf[i], &buffer[i]);
		status.buf_ptr = &status.buf[i];
		bytes_written++;
	}
	return bytes_written;
}

static loff_t device_lseek(file, offset, whence) // TODO
struct file *file;
loff_t *offset; /* Our offset in the file */
int whence;
{
	loff_t temp;

	switch (whence)
	{
	case SEEK_SET:
		temp = *offset;
		break;
	case SEEK_CUR:
		temp = *(status.buf_ptr + *offset);
		break;
	case SEEK_END:
		temp = *(DRV_BUF_SIZE + offset);
		break;
	default:
		return -EINVAL;
	};

	if (temp > DRV_BUF_SIZE || temp < 0)
	{
		return -EINVAL;
	}

	status.buf_ptr = (char *)temp;
	return temp;
}

static ssize_t device_ioctl(file, cmd, arg) // TODO
struct file *file;
uint cmd;
unsigned long arg;
{
	char *temp;

	switch (cmd)
	{
	case 1:
		memset(status.buf, 0, sizeof(status.buf));
		break;

	case 2:
		memcpy(status.buf, static_art, sizeof(static_art));
		status.buf_length = 2500;
		status.buf_ptr = NULL;
		break;
	case 3:
		int width = 0;
		int count = 0;
		temp = status.buf;
		while (*temp && *temp != '\n')
		{
			width++;
			temp++;
		}
		temp = status.buf;

		while (*temp)
		{
			count++;
			if (*temp == '\n')
			{
				count--;
				if (count != width)
				{
					printk("Line is shorter than width");
					return -1;
				}
				else
				{
					count = 0;
				}
			}
			else if (*temp < 32)
			{
				printk("A character is outside of the writable ascii");
				return -1;
			}
			temp++;
		}
		break;
	};

	return 0;
}

/* Initialize the module - Register the character device */
int init_module(void)
{
	/* Register the character device (atleast try) */
	status.major = register_chrdev(
		0,
		DEVICE_NAME,
		&Fops);

	/* Negative values signify an error */
	if (status.major < 0)
	{
		printk(
			"Sorry, registering the ASCII device failed with %d\n",
			status.major);

		return status.major;
	}

	printk(
		"Registeration is a success. The major device number is %d.\n",
		status.major);

	printk(
		"If you want to talk to the device driver,\n"
		"you'll have to create a device file. \n"
		"We suggest you use:\n\n"
		"mknod %s c %d <minor>\n\n"
		"You can try different minor numbers and see what happens.\n",
		DEVICE_NAME,
		status.major);

	return SUCCESS;
}

/* Cleanup - unregister the appropriate file from /proc */
void cleanup_module(void)
{
	unregister_chrdev(status.major, DEVICE_NAME);
}
