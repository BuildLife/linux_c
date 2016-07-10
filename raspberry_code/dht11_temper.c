#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/syscore_ops.h>
#include <linux/irq.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>

#include <linux/fs.h>

// for put_user 
#include <asm/uaccess.h>

// include RPi harware specific constants 
#include <mach/hardware.h>

#define DHT11_DRIVER_NAME "dht11"
#define RBUF_LEN 256

//when exec function have worked success, return this define
#define SUCCESS 0

// Max length of the message from the device 
#define BUF_LEN 80		

/****************define GPIO for to use**********/
// set GPIO pin g as input 
#define GPIO_DIR_INPUT(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))

// set GPIO pin g as output 
#define GPIO_DIR_OUTPUT(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))

// get logical value from gpio pin g 
#define GPIO_READ_PIN(g) (*(gpio+13) & (1<<(g))) && 1

// sets   bits which are 1 ignores bits which are 0 
#define GPIO_SET_PIN(g)	*(gpio+7) = 1<<g;

// clears bits which are 1 ignores bits which are 0 
#define GPIO_CLEAR_PIN(g) *(gpio+10) = 1<<g;

// Clear GPIO interrupt on the pin we use 
#define GPIO_INT_CLEAR(g) *(gpio+16) = (*(gpio+16) | (1<<g));

// GPREN0 GPIO Pin Rising Edge Detect Enable/Disable 
#define GPIO_INT_RISING(g,v) *(gpio+19) = v ? (*(gpio+19) | (1<<g)) : (*(gpio+19) ^ (1<<g))

// GPFEN0 GPIO Pin Falling Edge Detect Enable/Disable 
#define GPIO_INT_FALLING(g,v) *(gpio+22) = v ? (*(gpio+22) | (1<<g)) : (*(gpio+22) ^ (1<<g))


// module parameters 
static int sense = 0;
static struct timeval lasttv = {0,0};

//lock system 
static spinlock_t lock;


/**Global variables are declared as static, so are global within the file. **/

// Is device open?  Used to prevent multiple access to device
static int Device_Open = 0;		

// The msg the device will give when asked 		 
static char msg[BUF_LEN];	
			
static char *msg_Ptr;
static unsigned int bitcount=0;
static unsigned int bytecount=0;

//Indicate if we have started a read or not
static unsigned int started=0;	

//for result bytes
static unsigned char dht[5];

//default result format;
static format = 0;

//default GPIO PIN
static int gpio_pin = 0;

//default dricer number
static int driverno = 99;


volatile unsigned *gpio;


// Forward declarations dev function
static int read_dht11(struct inode *, struct file *);
static int close_dht11(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);


static void clear_interrupts(void);

//Operations that can be performed on the device
static struct file_operations fops = {
	.read = device_read,
	.open = read_dht11,
	.release = close_dht11
};


// Possible valid GPIO pins
int valid_gpio_pins[] = { 0, 1, 4, 8, 7, 9, 10, 11, 14, 15, 17, 18, 21, 22, 23,	24, 25 };


// Called when a process wants to read the dht11 "cat /dev/dht11"
static int read_dht11()
{
	char result[3];			//To say if the result is trustworthy or not
	int retry = 0;
	
	if (Device_Open)
		return -EBUSY;

	try_module_get(THIS_MODULE);		//Increase use count

	Device_Open++;

	// Take data low for min 18mS to start up DHT11
    //printk(KERN_INFO DHT11_DRIVER_NAME " Start setup (read_dht11)\n");

start_read:
	started = 0;
	bitcount = 0;
	bytecount = 0;
    dht[0] = 0;
	dht[1] = 0;
	dht[2] = 0;
	dht[3] = 0;
	dht[4] = 0;
	GPIO_DIR_OUTPUT(gpio_pin); 	// Set pin to output
    GPIO_CLEAR_PIN(gpio_pin);	// Set low
    mdelay(20);					// DHT11 needs min 18mS to signal a startup
    GPIO_SET_PIN(gpio_pin);		// Take pin high
    udelay(40);					// Stay high for a bit before swapping to read mode
    GPIO_DIR_INPUT(gpio_pin); 	// Change to read
	
	//Start timer to time pulse length
	do_gettimeofday(&lasttv);
	
	// Set up interrupts
	setup_interrupts();
	
	//Give the dht11 time to reply
	mdelay(10);
	
	//Check if the read results are valid. If not then try again!
	if((dht[0] + dht[1] + dht[2] + dht[3] == dht[4]) & (dht[4] > 0))
		sprintf(result, "OK");
	else
		{
		retry++;
		sprintf(result, "BAD");
		if(retry == 5)
			goto return_result;		//We tried 5 times so bail out
		clear_interrupts();
		mdelay(1100);				//Can only read from sensor every 1 second so give it time to recover
		goto start_read;
		}

		//Return the result in various different formats
return_result:	
	switch(format){
		case 0:
			sprintf(msg, "Values: %d, %d, %d, %d, %d, %s\n", dht[0], dht[1], dht[2], dht[3], dht[4], result);
			break;
		case 1:
			sprintf(msg, "%0X,%0X,%0X,%0X,%0X,%s\n", dht[0], dht[1], dht[2], dht[3], dht[4], result);
			break;
		case 2:
			sprintf(msg, "%02X%02X%02X%02X%02X%s\n", dht[0], dht[1], dht[2], dht[3], dht[4], result);
			break;
		case 3:
			sprintf(msg, "Temperature: %dC\nHumidity: %d%%\nResult:%s\n", dht[0], dht[2], result);
			break;
		
	}
	msg_Ptr = msg;

	return SUCCESS;
}

// Called when a process, which already opened the dev file, attempts to read from it.
static ssize_t device_read(struct file *filp,	// see include/linux/fs.h   
			   char *buffer,	// buffer to fill with data 
			   size_t length,	// length of the buffer     
			   loff_t * offset)
{
	// Number of bytes actually written to the buffer 
	int bytes_read = 0;

	// If we're at the end of the message, return 0 signifying end of file 
	if (*msg_Ptr == 0)
		return 0;

	// Actually put the data into the buffer 
	while (length && *msg_Ptr) {

		// The buffer is in the user data segment, not the kernel  segment so "*" assignment won't work.  We have to use 
		// put_user which copies data from the kernel data segment to the user data segment. 
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	// Return the number of bytes put into the buffer
	return bytes_read;
}

// Called when a process closes the device file.
static int close_dht11(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	//Device_Open--;
	
	clear_interrupts();
	return 0;
}

// Clear the GPIO edge detect interrupts
static void clear_interrupts(void)
{
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);

	// GPREN0 GPIO Pin Rising Edge Detect Disable 
	GPIO_INT_RISING(gpio_pin, 0);

	// GPFEN0 GPIO Pin Falling Edge Detect Disable 
	GPIO_INT_FALLING(gpio_pin, 0);

	spin_unlock_irqrestore(&lock, flags);

	free_irq(INTERRUPT_GPIO0, (void *) gpio);
}

//First interrupt need to create irq_handler
static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
{
	struct timeval tv;
	long deltv;
	int data = 0, signal;

	//use the GPIO signal level -> GPIO_READ_PIN : get logical value from gpio pin g 
	signal = GPIO_READ_PIN(gpio_pin);
	
	//reset interrupt  -> Clear GPIO interrupt on the pin we use
	GPIO_INT_CLEAR(gpio_pin);	
	
	//get current time
	do_gettimeofday(&tv);

	/* get time since last interrupt in microseconds*/
	deltv = tv.tv_sec - lasttv.tv_sec;
	data = (int)(deltv*1000000 + (tv.tv_usec - lasttv.tv_usec));
	lasttv = tv; //<- Save last interrupt time
	
	if(signal == 1 && data > 40)
	{
		started = 1;
		return IRQ_HANDLED;
	}
	if(signal == 0 && started == 1)
	{
		/*if(data > 80)
			return IRQ_HANDLED; //->/Start/spurious? signal
		if(data < 15)
			return IRQ_HANDLED; //->//Spurious signal?*/
		if(data > 60)
			dht[bytecount] = dht[bytecount] | (0x80 >> bitcount);	//Add a 1 to the data byte
		
		//Uncomment to log bits and durations - may affect performance and not be accurate!
		//printk("B:%d, d:%d, dt:%d\n", bytecount, bitcount, data);
		bitcount++;
		if(bitcount == 8)
		{
			bitcount = 0;
			bytecount++;
		}
	}
	
	//means that we did have a valid interrupt and handled it.
	return IRQ_HANDLED;
}

//insert irq to setup interrupts function 
static int setup_interrupts(void)
{
	int result;
	unsigned long flags;
	
	result = request_irq(,(irqreturn_t)irq_handler, 0, DHT11_DRIVER_NAME, (void*)gpio);
	
	switch(result)
	{
		case -EBUSY:
			printk(KERN_ERR DHT11_DRIVER_NAME ": IRQ %d is busy\n", INTERRUPT_GPIO0);
		return -EBUSY;
		case -EINVAL:
			printk(KERN_ERR DHT11_DRIVER_NAME ": Bad irq number or handler\n");
		return -EINVAL;
		default:
			printk(KERN_INFO DHT11_DRIVER_NAME	": Interrupt %04x obtained\n", INTERRUPT_GPIO0);
		break;
	}
	
	//locks for irq
	spin_lock_irqsave(&lock, flags);
	
	// GPREN0 GPIO Pin Rising Edge Detect Enable 
	GPIO_INT_RISING(gpio_pin, 1);
	// GPFEN0 GPIO Pin Falling Edge Detect Enable 
	GPIO_INT_FALLING(gpio_pin, 1);

	// clear interrupt flag 
	GPIO_INT_CLEAR(gpio_pin);
	
	
	spin_unlock_irqrestore(&lock, flags);
	return SUCCESS;
}



//give to init_module function
static int init_port(void)
{
	//reserve GPIO memory region.
	if(request_mem_region(GPIO_BASE, SZ_4K, DHT11_DRIVER_NAME) == NULL){
		printk(KERN_ERR DHT11_DRIVER_NAME "unable to obtain GPIO I/O memory address\n");
		return -EBUSY;
	}
	
	//remap the GPIO memory
	//SZ_4K 
	if((gpio = ioremap_nocache(GPIO_BASE, SZ_4K)) == NULL){
		printk(KERN_ERR DHT11_DRIVER_NAME ": failed to map GPIO I/O memory\n");
		return -EBUSY;
	}
	
	return 0;
}


static void __init dht11_init_module(void)
{
	int result;
	int i;
	
	//check for valid gpio pin number
	result = 0;
	for(i=0;(i<ARRAY_SIZE(valid_gpio_pins)) && (result!=1) ;i++)
	{
		if(gpio_pin == valid_gpio_pins[i])
			result++;
	}
	
	if(result != 1)
	{
		//-EINVAL --> return wrong variable.
		result = -EINVAL;
		
		//KERN_ERR -->  failed situation. The driver usually use KERN_ERR for report hw problem. 
		printk(KERN_ERR DHT11_DRIVER_NAME ":INVALID GPIO pin specified!\n");
		goto exit_rpi;
	}

	//old register chrdev driver
	result = register_chrdev(driverno, DHT11_DRIVER_NAME, &fops);
	
	if(result < 0){
		
		//KERN_ALERT ---> danger message,need immediately to fix.
		printk(KER_ALERT DHT11_DRIVER_NAME "Registering dht11 failded with %d\n",result); 
		return result;
		
	}
	
	//KERN_INFO --> reference  : print driver hw message when started in init.
	printk(KERN_INFO DHT11_DRIVER_NAME ":driver registered!\n");
	
	result = init_port();
	if(result < 0)
		goto exit_rpi;
	
exit_rpi:
	return result;
}


static void __exit dht11_exit_module(void)
{
		if(gpio != NULL){
			//when do the iounmap function. It don't need to keep pin,so use release_mem_region function for cancel.
			//iounmap(gpio);
			//release_mem_region(GPIO_BASE, SZ_4K);
			printk(DHT11_DRIVER_NAME ": Cleaned up module\n");
		}
		
		//old to release the driver.
		unregister_chrdev(driverno, DHT11_DRIVER_NAME);
		printk(DHT11_DRIVER_NAME ": Cleaned up module\n");
	
}


module_init(dht11_init_module);
module_exit(dht11_exit_module);


MODULE_DESCRIPTION("DHT11 temperature/humidity sendor driver for Raspberry Pi GPIO.");
MODULE_AUTHOR("Nigel Morton");
MODULE_LICENSE("GPL");


// Command line paramaters for gpio pin and driver major number
module_param(format, int, S_IRUGO);
MODULE_PARM_DESC(format, "Format of output");

module_param(gpio_pin, int, S_IRUGO);
MODULE_PARM_DESC(gpio_pin, "GPIO pin to use");

module_param(driverno, int, S_IRUGO);
MODULE_PARM_DESC(driverno, "Driver handler major value");

