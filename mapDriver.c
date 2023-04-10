#include "mapDriver.h"

/* Driver's Status is kept here */
static driver_status_t status =
{
	'a',   /* Starting ASCII char is '0' */
	false, /* Not busy at the beginning */
    2551,
	{0},   /* buffer */
	NULL,  /* buffer's ptr */
	-1,    /* major */
	-1     /* minor */
};

/* This function is called whenever a process
 * attempts to open the device file
 */
static int device_open(inode, file)
	struct inode* inode;
	struct file*  file;
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

	printk
	(
		"Device: %d.%d, busy: %d\n",
		status.major,
		status.minor,
		status.busy
	);

	/* We don't want to talk to two processes at the
	 * same time
	 */
	if(status.busy)
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
	sprintf
	(
		status.buf,
		"If I told you once, I told you %d times - %s",
		counter++,
		"Hello, world\n"
	);

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


/* This function is called when a process closes the
 * device file.
 */
static int device_release(inode, file)
	struct inode* inode;
	struct file*  file;
{
#ifdef _DEBUG
	printk ("device_release(%p,%p)\n", inode, file);
#endif

	/* We're now ready for our next caller */
	status.busy = false;

	return SUCCESS;
}

/* This function is called whenever a process which
 * have already opened the device file attempts to
 * read from it.
 */
static ssize_t device_read(file, buffer, length, offset) //TODO
	struct file* file;
    char*        buffer;  /* The buffer to fill with data */
    size_t       length;  /* The length of the buffer */
    loff_t*      offset;  /* Our offset in the file */
{
	ssize_t bytes_read = 0;

	if(length > buf_length)
	{
		printk(STDERR, "Reading failed: Requested more bytes than in buffer!");
		return 0;
	}

	for(int i = 0; i < length; ++i)
	{
		buffer[i] = buf[i];
		bytes_read++;
	}

	return bytes_read;
}

/* This function is called when somebody tries to write
 * into our device file.
 */
static ssize_t device_write(file, buffer, length, offset) //TODO
	struct file* file;
	const char*  buffer;  /* The buffer */
	size_t       length;  /* The length of the buffer */
	loff_t*      offset;  /* Our offset in the file */
{
	ssize_t bytes_written = 0;

    if (length > buf_length)
    {
        printk(STDERR, "Writing Failed: Too large to fit in map\n")
        return 0;
    }

    for (int i = 0; i < length; i++)
    {
        buf[i] = buffer[i];
        buf_ptr = buf[i];
		bytes_written++;
    }
    return bytes_written;
}

static ssize_t device_lseek(file, offset, whence) //TODO
	struct file* file;
	const char*  buffer;  /* The buffer */
	size_t       length;  /* The length of the buffer */
	loff_t*      offset;  /* Our offset in the file */
{
    loff_t temp;

    switch(whence)
    {
        case SEEK_SET:
            if((offset > DRV_BUF_SIZE)) || ()offset < 0))
                return -1;
            status->buf_ptr = offset; //filp->f_pos
            break;
        case SEEK_CUR:
            temp = status->buf_ptr + offset;
            if((temp > DRV_BUF_SIZE) || (temp < 0))
                return -1;
            status->buf_ptr = temp;
            break;
        case SEEK_END:
            temp = DRV_BUF_SIZE + offset;
            if((temp > DRV_BUD_SIZE) || (temp < 0))
                return -1;
            status->buf_ptr = temp;
            break;
        default:
            return -1;
    }

    return status->buf_ptr;
}

static ssize_t device_ioctl(file, buffer, cmd, arg) //TODO
	struct file* file;
	const char*  buffer;  /* The buffer */
	size_t       length;  /* The length of the buffer */
	loff_t*      offset;  /* Our offset in the file */
{
    switch(cmd)
    {
        case 1:
            buffer = {0};
            break;
        case 2:
            buffer = {0};
            status->length = 2551;
            status->buf_ptr = NULL;
            break;
        case 3:
            
            break;
    }

    return 0;
}
