/* 
 * hello the simplest kernel module.
 */

#include <linux/module.h>	// needed by all modules
#include <linux/kernel.h>	// needed for KERN_INFO

int init_module(void)
{
	printk(KERN_INFO "Hello World 1.\n");
// A non 0 return means init_module failed.
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}

