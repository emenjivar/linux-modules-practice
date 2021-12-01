#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/slab.h> // kmalloc
#include <linux/string.h> // strncpy
#include <linux/timekeeping.h>
#define SECONDS_ON_YEAR 31556952
#define SECONDS_ON_DAY 86400
#define SECONDS_ON_HOUR 3600
#define SECONDS_ON_MINUTE 60
#define MILLISECONDS_ON_SECOND 1000

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_MAX_SIZE 2048
#define PROCFS_FILENAME "hello-world-inode"
#define TAG "[hello-world-inode]"

struct current_time {
    unsigned short int year;
    unsigned char month;
    unsigned char day;
    unsigned short int day_of_year;
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned short int milliseconds;
};

static struct proc_dir_entry *my_proc_file;
static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;
char end_line = '\n';

static ssize_t my_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
    static int finished = 0;

    if (finished) {
        pr_info("%s my_read end\n", TAG);
        finished = 0;
        return 0;
    }

    finished = 1;

    if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size)) {
        return -EFAULT;
    }

    pr_info("%s my_read %lu bytes\n", TAG, procfs_buffer_size);
    return procfs_buffer_size;
}

static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *off) {
    char *local_buffer = kmalloc(len, GFP_KERNEL);
    char *string_date = kmalloc(27, GFP_KERNEL);

    struct timespec64 time;
    struct current_time now;

    int months[12] = {
        31, 28, 31,
        30, 31, 30,
        31, 31, 30,
        31, 30, 31
    };

    int acum = 0;
    int day_of_month = 0;
    int current_month = 0;

    ktime_get_real_ts64(&time);
    now.year = 1970 + time.tv_sec / SECONDS_ON_YEAR;;
    now.day_of_year = time.tv_sec % SECONDS_ON_YEAR / SECONDS_ON_DAY;

    for(current_month=0; current_month<12; current_month++) {
        acum += months[current_month];
        if(acum >= now.day_of_year) {
            day_of_month = months[current_month] - (acum - now.day_of_year);
            break;
        }
    }


    now.month = current_month;
    now.day = day_of_month;
    now.hours = time.tv_sec % SECONDS_ON_DAY / SECONDS_ON_HOUR;
    now.minutes = time.tv_sec % SECONDS_ON_HOUR / SECONDS_ON_MINUTE;
    now.seconds = time.tv_sec % SECONDS_ON_MINUTE;
    now.milliseconds = time.tv_sec % MILLISECONDS_ON_SECOND;

    if (copy_from_user(local_buffer, buffer, len)) {
        return -EFAULT;
    }

    // Concat timestamp
    snprintf(string_date, 27, "[%d-%d-%d %d:%d:%d:%d] ", now.year, now.month, now.day, now.hours, now.minutes, now.seconds, now.milliseconds);

    // Concat local buffer to global buffer
    strncat(procfs_buffer, string_date, strlen(string_date));
    strncat(procfs_buffer, local_buffer, strlen(local_buffer) - 1);
    strncat(procfs_buffer, &end_line, 1);
    procfs_buffer_size += strlen(local_buffer) + strlen(string_date);

    pr_info("%s\n", TAG);
    return len;
}

static int my_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

#ifdef HAVE_PROC_OPS
static struct proc_ops file_ops = {
    .proc_read = my_read,
    .proc_write = my_write,
    .proc_open = my_open,
    .proc_release = my_release
};
#else
static const struct file_operations file_ops = {
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release
};
#endif

static int __init proc_init(void)
{
    my_proc_file = proc_create(PROCFS_FILENAME, 0666, NULL, &file_ops);

    if (my_proc_file == NULL) {
        remove_proc_entry(PROCFS_FILENAME, NULL);
        pr_info("%s error: could not initialize /proc/%s\n", TAG, PROCFS_FILENAME);
        return -ENOMEM;
    }

    proc_set_size(my_proc_file, 80);
    proc_set_user(my_proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

    pr_info("%s /proc/%s created\n", TAG, PROCFS_FILENAME);
    return 0;
}

static void __exit proc_exit(void)
{
    remove_proc_entry(PROCFS_FILENAME, NULL);
    pr_info("%s /proc/%s removed\n\n", TAG, PROCFS_FILENAME);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
