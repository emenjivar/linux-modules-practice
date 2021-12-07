#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "hello-world-seq-file"
#define TAG "[hello-world-seq-file]"

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

#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops = {
	.proc_open = my_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release
};
#else
static const struct file_operations my_file_ops = {
	.open = my_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = req_release
};
#endif

static int __init procfs_init(void)
{
	struct proc_dir_entry *entry;

	entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);
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

