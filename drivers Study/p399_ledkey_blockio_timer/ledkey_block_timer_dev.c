#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/slab.h>

#include "ioctl_test.h"

#define CALL_DEV_NAME	"ledkey_block_timer"
#define CALL_DEV_MAJOR	240

#define DEBUG 0
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))
typedef struct
{
	uint64_t old_timer;
	uint64_t int_timer;
	uint64_t int_time_diff;
} __attribute__ ((packed)) KERNEL_TIMER_MANAGER;

DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read);
static int sw_irq[8] = {0};
static unsigned long sw_no = 0;
int led[] = {
	IMX_GPIO_NR(1, 16),   //16
	IMX_GPIO_NR(1, 17),	  //17
	IMX_GPIO_NR(1, 18),   //18
	IMX_GPIO_NR(1, 19),   //19
};
static int key[] = {
	IMX_GPIO_NR(1, 20),
	IMX_GPIO_NR(1, 21),
	IMX_GPIO_NR(4, 8),
	IMX_GPIO_NR(4, 9),
  	IMX_GPIO_NR(4, 5),
  	IMX_GPIO_NR(7, 13),
  	IMX_GPIO_NR(1, 7),
 	IMX_GPIO_NR(1, 8),
};
static int led_init(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(led); i++) {
		ret = gpio_request(led[i], "gpio led");
		if(ret<0){
			printk("#### FAILED Request gpio %d. error : %d \n", led[i], ret);
		} 
		gpio_direction_output(led[i], 0);
	}

	return ret;
}
static void led_exit(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(led); i++){
		gpio_free(led[i]);
	}
}

void led_write(char data)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(led); i++){
//		gpio_direction_output(led[i], (data >> i ) & 0x01);
		gpio_set_value(led[i], (data >> i ) & 0x01);
	}
#if DEBUG
	printk("#### %s, data = %d\n", __FUNCTION__, data);
#endif
}
/*
static int key_init(void)
{
    int ret = 0;
    int i;

    for (i = 0; i < ARRAY_SIZE(key); i++) {
        ret = gpio_request(key[i], "gpio key");
        if(ret<0){
            printk("#### FAILED Request gpio %d. error : %d \n", key[i], ret);
        }
        gpio_direction_input(key[i]); 
    }   

    return ret;
}
static void key_exit(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(key); i++){
        gpio_free(key[i]);
    }   
}
void key_read(char * key_data)
{
	int i;
	char data=0;
	for(i=0;i<8;i++)
	{
		if(gpio_get_value(key[i]))
		{
			data = i+1;
			break;
		}
	}
#if DEBUG
	printk("#### %s, data = %d\n", __FUNCTION__, data);
#endif
	*key_data = data;
	return;
}
*/
//irq function : init,exit,irq
irqreturn_t sw_isr(int irq, void *unuse)
{
	int i;
	for(i=0; i< ARRAY_SIZE(key); i++){
		if(irq == sw_irq[i]) {
			sw_no = i+1;
			break;
		}
	}
	printk("IRQ : %d, %ld\n",irq,sw_no);
	wake_up_interruptible(&WaitQueue_Read);
	return IRQ_HANDLED;
}
static int key_irq_init(void)
{
	int ret=0;
	int i;
	char * irq_name[8]={"irq sw1","irq sw2","irq sw3","irq sw4",
		"irq sw5","irq sw6","irq sw7","irq sw8"};
	
	for(i=0; i< ARRAY_SIZE(key); i++){
		sw_irq[i]=gpio_to_irq(key[i]);
	}
	for(i=0; i< ARRAY_SIZE(key); i++){
		ret = request_irq(sw_irq[i],sw_isr,IRQF_TRIGGER_RISING,irq_name[i],NULL);
		if(ret) {
			printk("### FAILED Request irq %d. error : %d\n",sw_irq[i],ret);
		}
	}
	return ret;
}
static void key_irq_exit(void)
{
	int i;
	for(i=0; i< ARRAY_SIZE(key); i++){
		free_irq(sw_irq[i],NULL);
	}
}



static int ledkeyint_open(struct inode *inode, struct file *filp)
{
	KERNEL_TIMER_MANAGER* ptrmng = NULL;
    int num0 = MAJOR(inode->i_rdev);
    int num1 = MINOR(inode->i_rdev);
    printk( "call open -> major : %d\n", num0 );
    printk( "call open -> minor : %d\n", num1 );

 	ptrmng = (KERNEL_TIMER_MANAGER *)kmalloc( sizeof(KERNEL_TIMER_MANAGER ), GFP_KERNEL);
	if(ptrmng == NULL) return -ENOMEM;
    memset( ptrmng, 0, sizeof( KERNEL_TIMER_MANAGER));

	ptrmng->old_timer = get_jiffies_64();
	printk("start time : %lld\n", ptrmng->old_timer);
	filp->private_data = ptrmng;

	return 0;
}
static loff_t ledkeyint_llseek(struct file *filp, loff_t off, int whence)
{
	printk("call llseek -> off : %08X, whence : %08X\n",(unsigned int)off, whence);
	return 0x23;
}
static ssize_t ledkeyint_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char kbuf;
	int ret;
	KERNEL_TIMER_MANAGER* ptrmng;
    ptrmng= (KERNEL_TIMER_MANAGER*)filp->private_data;

	if(!(filp->f_flags & O_NONBLOCK))
	{
		if(sw_no == 0)
			interruptible_sleep_on(&WaitQueue_Read);
//			wait_event_interruptible(WaitQueue_Read,sw_no);
//			wait_event_interruptible_timeout(WaitQueue_Read,sw_no,100);
//		if(sw_no == 0)
//			return 0;
	}
		ptrmng->int_timer = get_jiffies_64();
		ptrmng->int_time_diff = (ptrmng->int_timer)-(ptrmng->old_timer);
//		printk("interrupt time : %lld\n", (ptrmng->int_timer));
		printk("interrupt time diff: %lld\n", (ptrmng->int_time_diff));
		kbuf = (char)sw_no;
		ret=copy_to_user(buf,&kbuf,count);
		sw_no = 0;	
		ptrmng->old_timer = get_jiffies_64();
	return count;
}
static ssize_t ledkeyint_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	char kbuf;
//	printk("call write -> buf : %08X, count : %08X \n", (unsigned int)buf,count);
//	get_user(kbuf,buf);
	count=copy_from_user(&kbuf,buf,count);
	led_write(kbuf);
	return count;
}
static long ledkeyint_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	ioctl_test_info ctrl_info = {0,{0}};
    int err, size;
	KERNEL_TIMER_MANAGER* ptrmng;
    ptrmng= (KERNEL_TIMER_MANAGER*)filp->private_data;

    if( _IOC_TYPE( cmd ) != IOCTLTEST_MAGIC ) return -EINVAL;
    if( _IOC_NR( cmd ) >= IOCTLTEST_MAXNR ) return -EINVAL;

    size = _IOC_SIZE( cmd );
    if( size )
    {
        err = 0;
        if( _IOC_DIR( cmd ) & _IOC_READ )
            err = access_ok( VERIFY_WRITE, (void *) arg, size );
        else if( _IOC_DIR( cmd ) & _IOC_WRITE )
            err = access_ok( VERIFY_READ , (void *) arg, size );
        if( !err ) return err;
    }
    switch( cmd )
    {
        case IOCTLTEST_KEYLEDINIT :
            break;
        case IOCTLTEST_KEYLEDFREE :
            break;
        case IOCTLTEST_LEDOFF :
            break;
        case IOCTLTEST_LEDON :
            break;
        case IOCTLTEST_GETSTATE :
			break;
        case IOCTLTEST_READ :
            ctrl_info.size = (unsigned long)(ptrmng->int_time_diff);
			printk("info : %ld",ctrl_info.size);
            err = copy_to_user((void*)arg,(void*)&ctrl_info,sizeof(ctrl_info));
            break;
        case IOCTLTEST_WRITE :
            break;
        case IOCTLTEST_WRITE_READ :
            break;
        default:
            break;
    }
    return 0;

}
static int ledkeyint_release(struct inode *inode, struct file *filp)
{
	KERNEL_TIMER_MANAGER* ptrmng = (KERNEL_TIMER_MANAGER*)filp->private_data;
	printk("call release \n");
    if(ptrmng != NULL)
    {
        kfree(ptrmng);
    }
    led_write(0);

	return 0;
}

struct file_operations ledkeyint_fops = 
{
	.owner	= THIS_MODULE,
	.llseek	= ledkeyint_llseek,
	.read	= ledkeyint_read,
	.write	= ledkeyint_write,
	.unlocked_ioctl	= ledkeyint_ioctl,
	.open	= ledkeyint_open,
	.release	= ledkeyint_release,
};
static int ledkeyint_init(void)
{
	int result;

	led_init();
	key_irq_init();
	printk("call call_init \n");
	result = register_chrdev(CALL_DEV_MAJOR,CALL_DEV_NAME, &ledkeyint_fops);
	if(result < 0) return result;
	return 0;
}
static void ledkeyint_exit(void)
{
	printk("call call_exit \n");
	led_exit();
	key_irq_exit();
	unregister_chrdev(CALL_DEV_MAJOR,CALL_DEV_NAME);
}
module_init(ledkeyint_init);
module_exit(ledkeyint_exit);
MODULE_LICENSE("Dual BSD/GPL");
