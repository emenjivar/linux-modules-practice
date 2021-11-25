/**
* create the proc file on init function
* read the file on procfile_read
* and delete the proc file on exit function
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_from_user and copy_to_user
#include <linux/version.h>
#include <linux/string.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "hello-world-proc"
#define TAG "[hello-world-proc]"

static struct proc_dir_entry *our_proc_file;

// The buffer used to store characters for this module
static char procfs_buffer[PROCFS_MAX_SIZE];


// The size of the buffer
static unsigned long procfs_buffer_size = 0;

/**
* Everytime the file is read, this function is called.
* @param buffer is the content returned for the command which read it (i.e. cat, more)
* @param offset is the position in the file, the function is called until offset is zero.
* Be careful about offset param, this can cause an infinite loop
*/
static ssize_t procfile_read(struct file *ptrFile, char __user *buffer, size_t buffer_length, loff_t *offset)
{
    char *message = "Humans cannot create anything out of nothingness.\n";
    size_t length = strlen(message);
    ssize_t ret = length;

    /**
    * copy_to_user returns the number of bytes not copied
    * 0 means success and TRUE inside the IF sentence
    */

    int copied_bytes = copy_to_user(buffer, message, length);

    /**
    * desreference the offset pointer, accessing to offset value
    * check if the offset is still, in the file.
    */
    if(*offset >= length || copied_bytes) {
        pr_info("%s copy_to_user failed\n", TAG);
        ret = 0;
    } else {
        pr_info("%s procfile read: %s\n", TAG, ptrFile->f_path.dentry->d_name.name);
        *offset += length;
    }

    return ret;
}

/**
* This function is called when /proc file is written
*/
static ssize_t procfile_write(struct file *file, const char __user *buff, size_t len, loff_t *off)
{
    pr_info("%s\n", TAG);
    pr_info("%s start writting...", TAG);
    pr_info("%s lenght of writting: %zu\n", TAG, len);

    procfs_buffer_size = len;
    if(procfs_buffer_size > PROCFS_MAX_SIZE) {
        procfs_buffer_size = PROCFS_MAX_SIZE;
    }

    if(copy_from_user(procfs_buffer, buff, procfs_buffer_size)) {
        return -EFAULT;
    }

    /**
    * If the string exceeds the size of the buffer,
    * the string will be empty
    */
    procfs_buffer[procfs_buffer_size & (PROCFS_MAX_SIZE - 1)] = '\0';
    pr_info("%s procfs_buffer_size=%lu\n", TAG, procfs_buffer_size);
    pr_info("%s procfile write %s\n", TAG, procfs_buffer);

    return procfs_buffer_size;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
    .proc_write = procfile_write
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
    .write = procfile_write
};
#endif

static int __init proc_init(void)
{
    our_proc_file = proc_create(
        PROCFS_NAME,
        0666, //  All users can read and write this file
        NULL,
        &proc_file_fops
    );

    if(our_proc_file == NULL) {
        proc_remove(our_proc_file);
        pr_alert("%s Error: could not initialize /proc/%s\n", TAG, PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("%s /proc/%s created", TAG, PROCFS_NAME);
    return 0;
}

static void __exit proc_exit(void)
{
    proc_remove(our_proc_file);
    pr_info("%s /proc/%s removed\n", TAG, PROCFS_NAME);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlos menjivar");
MODULE_DESCRIPTION("simplest proc module");
