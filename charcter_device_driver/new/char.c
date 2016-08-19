
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h> 


#define DEVICE_NAME "char"
#define DEVICE_CLASS "char_class"

MODULE_LICENSE("GPL");

static int majorNumber;
static char message[256]={0};
static short size_of_message;
static int numberOpens =0;
static struct device *char_device = NULL;
static struct class *char_class =NULL;

static int dev_open(struct inode *,struct file*);
static int dev_close(struct inode*,struct file*);
static ssize_t dev_read(struct file*, char *,size_t,loff_t*);
static ssize_t dev_write(struct file*,const char *,size_t,loff_t*);

static struct file_operations fops =
{
	.open = dev_open,
	.release = dev_close,
	.write = dev_write,
	.read = dev_read,

};


static int __init char_init(void)
{
	printk(KERN_INFO "WE are in char_init fuction......\n");

	majorNumber = register_chrdev(0,DEVICE_NAME,&fops)

		if(majorNumber < 0)
		{
			printk(KERN_ALERT "failt register majorNumber for char\n");
			return majornumber;
		}

	printk(KERN_ALERT "register majorNumber correctly  for char\n");

	// register device class

	char_class = class_create(THIS_MODULE,DEVICE_CLASS);
	if(IS_ERR(char_class))
	{
		unregister_chrdev(majorNumber,DEVICE_NAME);
		printk("fail to register device class\n");
		return PTR_ERR(char_class);
	}

	// register the device driver

	char_device = device_create(char_class ,NULL,MKDEV(majorNumber,0),NULL,DEVICE_NAME);
	if(IS_ERR(char_device))
	{
		class_destroy(char_class);
		unregister_chardev(0,DEVICE_NAME);
		prink(KERN_ALERT "fail to create device \n")
			return PTR_ERR(char_device)
	}

	printk(KERN_INFO,"sucessfully crate device class \n");

	return 0;
}

static void __exit char_exit(void)
{
	device_destroy(char_class,MKDEV(majorNumber,DEVICE_NAME));
	class_unregister(char_class);
	class_destroy(char_class);
	unregister_chardev(majorNumber,DEVICE_NAME);
	printk(KERN_INFO,"BYE BYE");
}


static int dev_open(struct inod *inodep,struct file *filep)
{
	numberOpens++;
	printk(KERN_INFO,"device is open (%d) times",numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep,char *buffer ,size_t len ,loff_t *offset)
{
	int error_count=0;

	error_count = copy_to_user(buffer,message,size_of_message)

		if(error_count==0)
		{
			printk(KERN_INFO,"char %d charcter send to user \n",size_of_message);
			return 0;
		}
		else
		{
			printk(KERN_INFO,"fail to send %d charcter to user\n",size_of_messsage);
			return -EFAULT;
		}
}

static ssize_t dev_write(struct file *filep,const char *buffer,size_t len, loff_t *offset )
{

	sprintf(message,"%s(%d) letters",buffer,len);
	size_of_message = strlen(message);

	printk(KERN_INFO,"recieve %d charcter from user\n,size_of_message",size_of_message);
	return len;
}

static dev_close(struct inode *inodep ,struct file *filep)
{
	prink(KERN_INFO "device successfully close\n");
	return 0;
}

module_init(char_init);
module_exit(char_exit);
