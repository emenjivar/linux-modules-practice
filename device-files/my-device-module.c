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

// Prototypes
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
// static char *get_mood(char *mood);

static int get_mood_index(char *mood);
static int get_random_track_index(void);
static char **get_random_track(char *mood);

#define SUCCESS 0
#define DEVICE_NAME "my-device-module"
#define BUF_LEN 80
#define ROW 4
#define COL 6

#define SAD 0
#define LONELY 1
#define HAPPY 2
#define WEIRD 3

// Global variables are declared as static
static int major_number;
static char msg[BUF_LEN];
static char *mood_matrix[ROW][COL][2] = {
    // sad
    {
        {"Where is my love?", "Cat power"},
        {"Space song", "Beach house"},
        {"Come sweet death", "The end of evangelion"},
        {"Jueves", "La oreja de van gogh"},
        {"Alma mia", "Silvio rodriguez"},
        {"Fireworks", "Stina nordenstam"}
    },
    // lonely
    {
        {"Don't give up", "Peter gabriel"},
        {"Thank you for hearing me", "Sinead o'connor"},
        {"Dimentions", "Her OST"},
        {"The moon song", "Karen O"},
        {"Asleep", "The smiths"},
        {"Eclipse de luna en el cielo", "Natalia lafourcase"}
    },
    // happy
    {
        {"Over the rainbow", "Judy garland"},
        {"Come on eileen", "Dexis midnight runners"},
        {"Sea of love", "Cat power"},
        {"I'll try anything one", "The strokes"},
        {"Starry, starry night", "Don McLean"},
        {"For lovers", "Lamp"}
    },
    // weird
    {
        {"Creep", "Radiohead"},
        {"Weird fished", "Radiohead"},
        {"13 angels standing guard 'round the side of your bed", "Silver Mt Zion"},
        {"Little dark age", "MGMT"},
        {"Pneumonia", "Bjork"},
        {"Mercy street", "Peter gabriel"}
    }
};

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
        pr_alert("%s Registering char device failed with %d\n", DEVICE_NAME, major_number);
        return major_number;
    }

    // Creating a pointer to use on device creation
    cls = class_create(THIS_MODULE, DEVICE_NAME);

    // Creating device automatically, no need to run mknod command
    device_create(cls, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);

    pr_info("%s I was assigned major number %d.\n", DEVICE_NAME, major_number);
    pr_info("%s Create the device using 'mknod /dev/%s c %d 0'\n", DEVICE_NAME, DEVICE_NAME, major_number);
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
    char **track;

    // Checking if device is already open for another user/process
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    track = get_random_track(mood); // get_mood(mood);

    sprintf(msg, "Track: %s - %s\n", track[0], track[1]);

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
    pr_alert("Sorry, this operation isn't supported.\n");
    return -EINVAL;
}

/*
static char *get_mood(char *mood) {
    int i, random, mood_index;
    get_random_bytes(&i, sizeof(i));

    random = i % 3; // Get a random number between 0 and 2
    if(random < 0) {
        random *= -1;
    }

    if(strcmp(mood, "sad") == 0) {
        mood_index = 0;
    } else if(strcmp(mood, "depressed") == 0) {
        mood_index = 1;
    } else {
        mood_index = 0; // default mood sad
    }

    return mood_matrix[mood_index][random];
}
*/

static int get_mood_index(char *mood) {
    int index;
    if(strcmp(mood, "sad") == 0) {
        index = 0;
    } else if(strcmp(mood, "lonely") == 0) {
        index = 1;
    } else if(strcmp(mood, "happy") == 0) {
        index = 2;
    } else if(strcmp(mood, "weird") == 0) {
        index = 3;
    } else {
        index = 0;
    }

    return index;
}

static int get_random_track_index(void) {
    int random_bytes, index;
    get_random_bytes(&random_bytes, sizeof(random_bytes));

    // Get a random number between 0 and 5
    index = random_bytes % 6;

    // Calculate absolute value of index
    if(index < 0) {
        index *= -1;
    }

    return index;
}

static char **get_random_track(char *mood) {
    int local_mood_index, random_track_index;

    local_mood_index = get_mood_index(mood);
    random_track_index = get_random_track_index();
    return mood_matrix[local_mood_index][random_track_index];
}

MODULE_LICENSE("GPL");
