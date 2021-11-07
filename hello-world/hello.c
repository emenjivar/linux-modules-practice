#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define TAG "hello-module"

static int __init hello_init(void)
{
    pr_info("%s Hello world\n", TAG);
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("%s Bye Hello world\n", TAG);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlos menjivar");
MODULE_DESCRIPTION("Simple hello world module");
