/*
 * devecho - a minimal device driver, which reads data from the /dev/devecho
 * port, sorts it and writes it back to the console
 *
 * Florian Deragisch <floriade@ee.ethz.ch>
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/string.h>
#include <asm/uaccess.h>

/*
 * sort_read is the function called when a process calls read() on
 * /dev/devecho.  
 */

char * txtbuffer;
int * myIntArray;

static ssize_t sort_read(struct file * file,
			 char * buf, 
			 size_t count,
			 loff_t *ppos)
{
	int len = strlen(txtbuffer); /* Don't include the null byte. */
	/*
	 * We only support reading the whole string at once.
	 */
	if (count < len)
		return -EINVAL;
	/*
	 * If file position is non-zero, then assume the string has
	 * been read and indicate there is no more data to be read.
	 */
	if (*ppos != 0)
		return 0;
	/*
	 * Besides copying the string to the user provided buffer,
	 * this function also checks that the user has permission to
	 * write to the buffer, that it is mapped, etc.
	 */
	if (copy_to_user(buf, txtbuffer, len)){
		printk(KERN_ERR "Failed\n");
		return -EINVAL;
	}
	/*
	 * Tell the user how much data we wrote.
	 */
	*ppos = len;

	return len;
}

int cmpint(const void *a, const void *b){
         return *(int *)a - *(int *)b;
}

static ssize_t sort_write(struct file *file,
			  const char __user *in,
		     	  size_t count, loff_t *off)
{
	
	char *next, *txtcpy, *tmpstr;
	int i = 0, k= 0;
	int dummy;

	if(count > 100){
		printk(KERN_ERR "Too many arguments.\n");
		return -EINVAL;
	}

	txtbuffer = kmalloc(sizeof(char)*(count+1), 0);
	myIntArray = kmalloc(sizeof(int)*count, 0);
	tmpstr = kmalloc(sizeof(char)*100, 0);
	

	if(!txtbuffer){
		printk(KERN_ERR "Could not allocate Kernel memory.\n");
		return -EINVAL;
	}

	if(copy_from_user(txtbuffer, in, count)){
		printk(KERN_ERR "Copy from user error\n");
		return -EINVAL;
	}

	txtbuffer[count-1] = '\0';
	
	txtcpy = txtbuffer;
	
	
	do {
		next = strsep(&txtcpy, " ");
		if(next == NULL) 
			break;
		/*printk(KERN_ERR "Token: %s\n", next);*/
		dummy = kstrtouint(next, 10, &myIntArray[i]);
		if(dummy != 0) {
			printk(KERN_ERR "StrToInt failure.\n");
			return -EINVAL;
		}
		/*printk(KERN_ERR "Int: %d\n", myIntArray[i]);*/
		i++;
	} while(next != NULL);	

	sort((void*) myIntArray, i, sizeof(int), cmpint, NULL);
	
	k = i;
	i = 0;
	strcpy(txtbuffer, "");

	while( i != k-1){
		sprintf(tmpstr, "%d ", myIntArray[i]); 
		strcat(txtbuffer, tmpstr);
		i++;
	}
	sprintf(tmpstr, "%d\n", myIntArray[i]); 
	strcat(txtbuffer, tmpstr); 

	//printk(KERN_ERR "%s", txtbuffer); 

	*off = count;

	kfree(myIntArray);
	kfree(tmpstr);

	return count;
}

/*
 * The only file operation we care about is read and write
 */

static const struct file_operations sort_fops = {
	.owner		= THIS_MODULE,
	.read		= sort_read,
	.write		= sort_write,
};

static struct miscdevice sort_dev = {MISC_DYNAMIC_MINOR, "devecho", &sort_fops};
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 *
	 * Name ourselves /dev/hello.
	 *
	 *
	 *
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	


static int __init sort_init(void)
{
	int ret;

	/*
	 * Create the device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/hello device using
	 * the default rules.
	 */
	ret = misc_register(&sort_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register misc device\n");

	return ret;
}

static void __exit sort_exit(void)
{
	kfree(txtbuffer);	
	misc_deregister(&sort_dev);
}

module_init(sort_init);
module_exit(sort_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Deragisch <floriade@ee.ethz.ch>");
MODULE_DESCRIPTION("Sort dev module");
MODULE_VERSION("dev");
