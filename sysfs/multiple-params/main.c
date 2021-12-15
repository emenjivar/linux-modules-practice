// https://github.com/spotify/linux/blob/master/samples/kobject/kobject-example.c
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/slab.h> // kmalloc
#include <linux/uaccess.h> // copy_from_user, copy_to_user
#include <linux/version.h> // VERSION constants
#include <linux/module.h>
#include <linux/init.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "custom-screen"
#define TAG "[custom-screen]"
#define SIZE_BACKGROUND_PATH 512
#define PROC_BUFFER 2048

/**
 * Proc file variables
 */
static struct proc_dir_entry *proc_file;

/**
 * Files created under /sys/kernel/custom-screen
 */
static char background[SIZE_BACKGROUND_PATH];
static char style[10] = "none"; // stretched, centered or none
static int icon_size = 48;

/**
 * Proc file functions
 */
static ssize_t proc_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
	static int finished = 0;
	
	long unsigned int local_length = 5 + SIZE_BACKGROUND_PATH;
	char *local_buffer = kmalloc(local_length, GFP_KERNEL);

	snprintf(local_buffer, local_length, "background: %s\n", background);

	if (finished) {
		finished = 0;
		return 0;
	}

	finished = 1;

	if (copy_to_user(buffer, local_buffer, local_length))
		return -EFAULT;

	return local_length;
}

static int proc_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);
	return 0;
}

static int proc_release(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;
}

#ifdef HAVE_PROC_OPS
static struct proc_ops file_ops = {
	.proc_read = proc_read,
	.proc_open = proc_open,
	.proc_release = proc_release
};
#else
static const struct file_operations file_ops = {
	.read = proc_read,
	.open = proc_open,
	.release = proc_release
};
#endif

// Background attr functions
static ssize_t background_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", background);
}

static ssize_t background_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	if (count > SIZE_BACKGROUND_PATH) {
		pr_info("%s background to long: %s\n", TAG, buf);
		return -ENOMEM;
	}

	pr_info("%s background {size : %zu, path: %s}\n", TAG, count, buf);
	sscanf(buf, "%s", background);
	return count;
}

// Style attr functions
static ssize_t style_show(struct kobject *kobjs, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", style);	
}

static ssize_t style_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%s", style);
	return count;
}	

// Icon size attr functions
static ssize_t icon_size_show(struct kobject *kobjs, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", icon_size);
}

static ssize_t icon_size_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%d", &icon_size);
	return count;
}

// attr files
static struct kobj_attribute background_attr = __ATTR(background, 0664, background_show, background_store);
static struct kobj_attribute style_attr = __ATTR(style, 0664, style_show, style_store);
static struct kobj_attribute icon_size_attr = __ATTR(icon_size, 0664, icon_size_show, icon_size_store);

/**
 * Create a group of attributes
 */
static struct attribute *attrs[] = {
	&background_attr.attr,
	&style_attr.attr,
	&icon_size_attr.attr,
	NULL // Need to terminale the list of attributes
};

static struct attribute_group attr_group = {
	.attrs = attrs
};

static struct kobject *example_kobj;

static int __init example_init(void)
{
	int retval;

	// Create the file /proc/custom_screen
	proc_file = proc_create(PROC_NAME, 0664, NULL, &file_ops);

	// Create the directory /sys/kernel/custom_screen/
	example_kobj = kobject_create_and_add("custom_screen", kernel_kobj);

	if (proc_file == NULL) {
		remove_proc_entry(PROC_NAME, NULL);
		return -ENOMEM;
	}

	if (!example_kobj)
		return -ENOMEM;

	// Init proc file
	proc_set_size(proc_file, 80);
	proc_set_user(proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);
	
	// Create files under /sys/kernel/custom/screen/
	retval = sysfs_create_group(example_kobj, &attr_group);

	return retval;
}

static void __exit example_exit(void)
{
	// Delete proc file
	remove_proc_entry(PROC_NAME, NULL);

	// Delete directory /sys/kernel/custom_screen/
	kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
