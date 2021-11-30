#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/slab.h> // kmalloc
#include <linux/string.h> // strncpy
#include <linux/timekeeping.h>
#define SECONDS_ON_YEAR 31557600
#define SECONDS_ON_DAY 86400

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_MAX_SIZE 2048
#define PROCFS_FILENAME "hello-world-inode"
#define TAG "[hello-world-inode]"

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
    //char *string_date = kmalloc(13, GFP_KERNEL);

    struct timespec64 time;
    u64 year;
    u64 day_of_year;

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
    year = 1970 + time.tv_sec / SECONDS_ON_YEAR;
    day_of_year = time.tv_sec % SECONDS_ON_YEAR / SECONDS_ON_DAY;

    for(current_month=0; current_month<12; current_month++) {
        acum += months[current_month];
        if(acum > day_of_year) {
            day_of_month = months[current_month] - (acum - day_of_year);
            break;
        }
    }

    if (copy_from_user(local_buffer, buffer, len)) {
        return -EFAULT;
    }


    // Concat timestamp
    //snprintf(string_date, 13, "[%llu-%d-%d]", year, current_month, day_of_month);
    //pr_info("%s formatted date: %s", TAG, string_date);

    // Concat local buffer to global buffer
    //strncat(procfs_buffer, string_date, strlen(string_date) - 1);
    strncat(procfs_buffer, local_buffer, strlen(local_buffer) - 1);
    strncat(procfs_buffer, &end_line, 1);
    procfs_buffer_size += strlen(local_buffer);

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
