#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/slab.h> // kmalloc
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "hello-world-seq-file"
#define TAG "[hello-world-seq-file]"

static int read_counter = 0;
static int write_counter = 0;

static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long counter = 0;
	pr_info("%s my_seq_start\n", TAG);

	if(*pos == 0) {
		return &counter;
	}

	*pos = 0;
	return NULL;
}

static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *) v;
	(*tmp_v)++;
	(*pos)++;
	pr_info("%s my_sec_next\n", TAG);

	return NULL;
}

static void my_seq_stop(struct seq_file *s, void *v)
{
	// Nothing to do here
	pr_info("%s my_seq_stop\n", TAG);
}

static int my_seq_show(struct seq_file *s, void *v)
{
	loff_t *spot = (loff_t *) v;
	seq_printf(s, "%Ld\n", *spot);
	pr_info("%s my_seq_show\n", TAG);

	return 0;
}

static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next = my_seq_next,
	.stop = my_seq_stop,
	.show = my_seq_show,
};

static int my_open(struct inode *inode, struct file *file)
{
	pr_info("%s my_open\n", TAG);
	return seq_open(file, &my_seq_ops);
}

static ssize_t my_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
	static int finished = 0;
	static size_t buffer_len = 36;
	char *local_buffer = kmalloc(buffer_len, GFP_KERNEL);

	if (finished) {
		finished = 0;
		return 0;
	}

	finished = 1;

	// Checking read_counter limit
	if (read_counter <= 98)
		read_counter++;

	// Formatting string
	snprintf(local_buffer, buffer_len, "reading %d times\nwritting %d times\n", read_counter, write_counter);

	if(copy_to_user(buffer, local_buffer, buffer_len))
		return -EFAULT;

	return buffer_len;
}

static ssize_t my_write(struct file *file, const char __user *buffer, size_t len, loff_t *off)
{
	pr_info("%s my_write executing...", TAG);

	// Checkint write_counter limit
	if (write_counter <= 98)
		write_counter++;
	
	return len;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops = {
	.proc_open = my_open,
	.proc_read = my_read,
	.proc_write = my_write,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release
};
#else
static const struct file_operations my_file_ops = {
	.open = my_open,
	.read = my_read,
	.write = my_write,
	.llseek = seq_lseek,
	.release = req_release
};
#endif

static int __init procfs_init(void)
{
	struct proc_dir_entry *entry;

	entry = proc_create(PROC_NAME, 0666, NULL, &my_file_ops);
	if(entry == NULL) {
		remove_proc_entry(PROC_NAME, NULL);

		pr_info("%s Error: could not initialize /proc/%s\n", TAG, PROC_NAME);
		return -ENOMEM;
	}
	
	pr_info("%s /proc/%s created\n", TAG, PROC_NAME);
	return 0;
}

static void __exit procfs_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("%s /proc/%s removed\n", TAG, PROC_NAME);
	pr_info("%s\n", TAG);
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");

