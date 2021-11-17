#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/poll.h>
#include <linux/random.h>
#include <linux/string.h>
#include "tracks.h"

// Prototypes
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "my-device-module"
#define TAG "[my-device-module]"
#define BUF_LEN 80

// Global variables are declared as static
static int major_number;
static char msg[BUF_LEN];

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1
};

// Prevent multiple access to device driver
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static struct class *cls;

// Parameters
static char *mood = "sad";
module_param(mood, charp, 0000);
MODULE_PARM_DESC(mood, "sad (default) | lonely | happy | weird.");

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

int init_module(void)
{
    /**
    * Adding the driver file to linux.
    * 0 on the first parameter means that we are
    * requesting the major number available on the SO
    */
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if(major_number < 0) {
        pr_alert("%s Registering char device failed with %d\n", TAG, major_number);
        return major_number;
    }

    // Creating a pointer to use on device creation
    cls = class_create(THIS_MODULE, DEVICE_NAME);

    // Creating device automatically, no need to run mknod command
    device_create(cls, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);

    // Loading soundtrack on memory
    load_songs();

    pr_info("%s I was assigned major number %d.\n", TAG, major_number);
    return SUCCESS;
}

void cleanup_module(void)
{
    device_destroy(cls, MKDEV(major_number, 0));
    class_destroy(cls);
    unregister_chrdev(major_number, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *filp)
{
    static int counter = 0;
    struct song track;

    // Checking if device is already open for another user/process
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    track = get_random_track(mood);
    sprintf(msg, "Track: %s - %s\n", track.name, track.author);
    counter++;

    /**
    * Increment the reference count of the module.
    * Use <<cat /proc/modules | grep 'my_device_module'>>
    * To see the value of the count
    */
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *filp)
{
    // Release the device for the next caller
    atomic_set(&already_open, CDEV_NOT_USED);

    /**
    * Decrement the reference count of the module.
    * Use <<cat /proc/modules | grep 'my_device_module'>>
    * To see the value of the count
    */
    module_put(THIS_MODULE);

    pr_info("%s device released.\n", TAG);
    return SUCCESS;
}

static ssize_t device_read(
    struct file *filp,
    char *buffer,
    size_t length,
    loff_t *offset
)
{
    int bytes_read = 0;
    const char *msg_ptr = msg;

    /**
    * Check if we are at the end of message
    * Reminder: *(...) is the memory address to the end of the msg
    */
    if(!*(msg_ptr + *offset)) {
        *offset = 0; // Reset the offset
        return 0; // End of the file
    }

    msg_ptr += *offset;

    // Put the data into the buffer
    while (length && *msg_ptr) {
        /**
        * Copying data from kernel data segment
        * to user data segment
        */
        put_user(*(msg_ptr++), buffer++);

        length--;
        bytes_read++;
    }

    *offset += bytes_read;

    return bytes_read;
}

static ssize_t device_write(
    struct file *filp,
    const char *buff,
    size_t len,
    loff_t * off
)
{
    pr_info("%s Sorry, read operation isn't supported.\n", TAG);
    return -EINVAL;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlos menjivar");
