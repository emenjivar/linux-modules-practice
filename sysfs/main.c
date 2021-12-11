// https://stackoverflow.com/questions/16245100/how-to-create-a-simple-sysfs-class-attribute-in-linux-kernel-v3-2
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <uapi/linux/stat.h> // S_IRUSR, S_IWURS

#define NAME "hello-world-sysfs"
#define TAG "[hello-world-sysfs]"

static struct kobject *module;

// The variable able to change
static int var = 0;

static ssize_t var_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", var);
}

static ssize_t var_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%d", &var);
	return count;
}

static struct kobj_attribute var_attr = __ATTR(var, 0664, var_show, var_store);

static int __init custom_init(void)
{
	int error = 0;
	pr_info("%s module initialized\n", TAG);

	/**
	 * Creating a directory on /sys/kernel/hello-world-sysfs
	 *
	 * kernel_kobj creates a dir under /sys/kernel/
	 * firmware_kobj creates a dir under /sys/firmware/
	 * fs_kobj creates a dir under /sys/fs/
	 * NULL creates a dir unsed /sys/
	 */
	module = kobject_create_and_add(NAME, kernel_kobj);

	if(!module)
		return -ENOMEM;

	error = sysfs_create_file(module, &var_attr.attr);

	if (error)
		pr_info("%s failed to create var file in /sys/kernel/hello-world-sysfs\n", TAG);

	return error;
}

static void __exit custom_exit(void)
{
	pr_info("%s exit success\n", TAG);
	kobject_put(module);
}

module_init(custom_init);
module_exit(custom_exit);

MODULE_LICENSE("GPL");
