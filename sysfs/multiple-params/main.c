// https://github.com/spotify/linux/blob/master/samples/kobject/kobject-example.c
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

#define TAG "[custom-screen]"
#define SIZE_BACKGROUND_PATH 512

/**
 * Files created under /sys/kernel/custom-screen
 */
static char background[SIZE_BACKGROUND_PATH];
static char style[10] = "none"; // stretched, centered or none
static int icon_size;

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

// attr files
static struct kobj_attribute background_attr = __ATTR(background, 0664, background_show, background_store);
static struct kobj_attribute style_attr = __ATTR(style, 0664, style_show, style_store);

/**
 * Create a group of attributes
 */
static struct attribute *attrs[] = {
	&background_attr.attr,
	&style_attr.attr,
	NULL // Need to terminale the list of attributes
};

static struct attribute_group attr_group = {
	.attrs = attrs
};

static struct kobject *example_kobj;

static int __init example_init(void)
{
	int retval;

	// Create the directory /sys/kernel/custom_screen/
	example_kobj = kobject_create_and_add("custom_screen", kernel_kobj);

	if (!example_kobj)
		return -ENOMEM;

	// Create files under /sys/kernel/custom/screen/
	retval = sysfs_create_group(example_kobj, &attr_group);

	return retval;
}

static void __exit example_exit(void)
{
	// Delete directory /sys/kernel/custom_screen/
	kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
