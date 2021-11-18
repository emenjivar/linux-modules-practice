/**
* create the proc file on init function
* read the file on procfile_read
* and delete the proc file on exit function
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROCFS_NAME "helloworld"
#define TAG "[helloworld]"

static struct proc_dir_entry *our_proc_file;

/**
* Everytime the file is read, this function is called.
* @param buffer is the content returned for the command which read it (i.e. cat, more)
* @param offset is the position in the file, the function is called until offset is zero.
* Be careful about offset param, this can cause an infinite loop
*/
static ssize_t procfile_read(struct file *ptrFile, char __user *buffer, size_t buffer_length, loff_t *offset)
{
    char s[13] = "HelloWorld!\n";
    int len = sizeof(s);
    ssize_t ret = len;

    /**
    * copy_to_user returns the number of bytes not copied
    * 0 means success and TRUE inside the IF sentence
    */
    int copied_bytes = copy_to_user(buffer, s, len);

    /**
    * desreference the offset pointer, accessing to offset value
    * check if the offset is still, in the file.
    */
    if(*offset >= len || copied_bytes) {
        pr_info("%s copy_to_user failed\n", TAG);
        ret = 0;
    } else {
        pr_info("%s procfile read: %s\n", TAG, ptrFile->f_path.dentry->d_name.name);
        *offset += len;
    }

    return ret;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read
};
#else
static const struct file_operations = {
    .read = procfile_read
};
#endif

static int __init proc_init(void)
{
    our_proc_file = proc_create(
        PROCFS_NAME,
        0644,
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
