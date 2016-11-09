#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<asm/uaccess.h>

#define MAJOR_NUMBER 60
#define MODULE_NAME "hello_NAME"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Charater device driver demo");
MODULE_AUTHOR("Frank Chen");

static char msg[128] = {0};
static int readPos = 0;

static int hello_open(struct inode *inode,struct file *filp)
{
	//int value = 0;
	printk(KERN_ALERT "hello module open\n");
	return 0;
}

static int hello_close(struct inode *inode,struct file *filp)
{
	printk(KERN_ALERT "hello module close\n");
	return 0;

}

static ssize_t hello_read(struct file *filp,char *buf,size_t size,loff_t *f_pos)
{
	/*size_t pos;
    uint8_t byte;
	char buffer[] = "Copy from kernel to user\n";
	ssize_t buffer_size = sizeof(buffer);
	if(*f_pos >= buffer_size)
		return 0;
	printk("hello module read (size = %zu)\n",size);
	//for(pos = 0;pos<size;++pos){
	if(*f_pos + size > buffer_size)
		size = buffer_size - *f_pos;
	if(copy_to_user(buf,buffer + *f_pos, size)!=0)
			return -EFAULT;
			*f_pos += size;
			return size;*/
	//return 0;
	int count = 0;
	while(size && (msg[count] != 0))
	{
		
		put_user(msg[readPos],buf++);
		size--;
		count++;
		readPos++;
	}

	return count;
}

static ssize_t hello_write(struct file *filp,char *buf,size_t size,loff_t *f_pos)
{
	/*size_t pos;
	uint8_t byte;
	//char *buffer;
	printk("hello module write (size = %zu)\n",size);
	for(pos = 0;pos<size;++pos){
		if(copy_from_user(&byte,buf+pos, 1)!=0){
			break;
			
	printk("<1> hello:write (buf[%zu] = %02x)\n",pos,(unsigned)(byte));
			}
	
	//printk("<1> hello:write (buf[%zu] = %02x)\n",pos,(unsigned)(buffer+pos));
	}
	return pos;*/
	int count = 0;
	int ind = size - 1;
	memset(msg,0,128);
	readPos = 0;

	while(size > 0)
	{
		msg[count++] = buf[ind--];
		size--;
	}

		return count;
}

static struct file_operations hello_fops={
	.open = hello_open,
	.release = hello_close,
	.read = hello_read,
	.write = hello_write,
};



static int hello_init(void)
{
	int value = 0;
	printk(KERN_ALERT "hello init\n");
	value = register_chrdev(MAJOR_NUMBER,MODULE_NAME,&hello_fops);

	if(value < 0)
	{
		printk(KERN_ALERT "hello module register error\n");
		return 0;

	}
	else
		printk(KERN_ALERT "hello module device registered...\n");
	return value;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "hello exit\n");
	unregister_chrdev(MAJOR_NUMBER,MODULE_NAME);
}

module_init(hello_init);
module_exit(hello_exit);
