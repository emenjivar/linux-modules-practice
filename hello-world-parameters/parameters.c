#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int port=81;
static char *ip="192.168.0.1";
static int excluded_ports[5]={0};
static int number_ports=0;

/**
* Third param is the bit permission
* It means, if the user can modify the value
* when the kernel is already mounted.
* The params are generated on /sys/module/my-module/parameters/my_param
* This file could be editable if has the correct permissions

* https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
*/
module_param(port, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(port, "Port number of application.");

/**
* When bit permission is 0, the parameter will not
* appear in the module directory
*/
module_param(ip, charp, 0000);
MODULE_PARM_DESC(ip, "Local ip address.");

/**
* array uses a extra parameter, which is a pointer of integer,
* in charge of counting the number of arguments
*
*/
module_param_array(excluded_ports, int, &number_ports, 0000);
MODULE_PARM_DESC(excluded_ports, "List of excluded_ports ports (max 5).");

static int __init simple_init(void)
{
    int i;
    printk(KERN_INFO "hello... port=%d ip=%s\n", port, ip);
    for(i=0; i<(sizeof(excluded_ports)/sizeof(int)); i++) {
        if(excluded_ports[i] != 0) {
            printk(KERN_INFO "hello.. excluded_port: %d\n", excluded_ports[i]);
        }
    }
    return 0;
}

static void __exit simple_exit(void)
{
    printk(KERN_INFO "bye... port=%d id=%s\n", port, ip);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlos menjivar");
MODULE_DESCRIPTION("Passing params through module");
