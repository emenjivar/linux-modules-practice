#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlos menjivar");
MODULE_DESCRIPTION("Hello world on different files");

int init_module(void)
{
    printk(KERN_INFO "Hello world - this is the kernel speaking\n");
    return 0;
}
