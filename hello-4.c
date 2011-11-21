/* 
 * hello the simplest kernel module.
 */

#include <linux/module.h>	// needed by all modules
#include <linux/kernel.h>	// needed for KERN_INFO
#include <linux/init.h>		// needed for the macros

#define DRIVER_AUTHOR "Florian Deragisch <floriade@ee.ethz.ch>"
#define DRIVER_DESC "A sample driver"

static int __init init_hello_4(void)
{
	printk(KERN_INFO "Hello World 4\n");
	return 0;
}

static void __exit cleanup_hello_4(void)
{
	printk(KERN_INFO "Goodbye world 4\n");
}

module_init(init_hello_4);
module_exit(cleanup_hello_4);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

MODULE_SUPPORTED_DEVICE("mydevice")

