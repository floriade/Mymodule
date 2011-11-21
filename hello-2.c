/* 
 * hello the simplest kernel module.
 */

#include <linux/module.h>	// needed by all modules
#include <linux/kernel.h>	// needed for KERN_INFO
#include <linux/init.h>		// needed for the macros

static int __init hello_2_init(void)
{
	printk(KERN_INFO "Hello World 2\n");
// A non 0 return means init_module failed.
	return 0;
}

static void __exit hello_2_exit(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);

