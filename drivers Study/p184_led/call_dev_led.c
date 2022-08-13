#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/gpio.h>
#define CALL_DEV_NAME "leddev"
#define CALL_DEV_MAJOR 240
#define DEBUG 1
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))
int led[] = {
    IMX_GPIO_NR(1, 16),   //16
    IMX_GPIO_NR(1, 17),   //17
    IMX_GPIO_NR(1, 18),   //18
    IMX_GPIO_NR(1, 19),   //19
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
        gpio_direction_output(led[i],0);
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
void led_write(unsigned long data)
{
    int i;
    for(i = 0; i < ARRAY_SIZE(led); i++){
        gpio_direction_output(led[i],(data>>i) &0x01);
       // gpio_set_value(led[i], (data >> i ) & 0x01);
    }
#if DEBUG
    printk("#### %s, data = %ld\n", __FUNCTION__, data);
#endif
}
//void led_read(unsigned long * led_data)
void led_read(char* led_data)
{
    int i;
    unsigned long data=0;
    unsigned long temp;
    for(i=0;i<4;i++)
    {
        gpio_direction_input(led[i]); //error led all turn off
        temp = gpio_get_value(led[i]) << i;
        data |= temp;
    }
    *led_data =(char)data;
	led_write(data);
    return;
}


static int call_open(struct inode *inode, struct file *filp)
{
	int ret;
	ret = led_init();
    return ret;
}
static ssize_t call_read(struct file *filp, char * buf, size_t count, loff_t *f_pos)
{
	led_read(buf);
	return 0;
}
static ssize_t call_write(struct file *filp, char * buf, size_t count, loff_t *f_pos)
{
	led_write((unsigned long)*buf);
	return count;
}
static ssize_t call_release(struct inode *inode, struct file *filp)
{
	led_exit();
	return 0;
}

struct file_operations call_fops =
{
	.owner	= THIS_MODULE,
	.read	= call_read,
	.write	= call_write,
	.open	= call_open,
	.release	= call_release,
};
static int call_init(void)
{
	int result;
	printk("call call_init \n");
	result = register_chrdev(CALL_DEV_MAJOR, CALL_DEV_NAME, &call_fops);
	if(result < 0) return result;
	return 0;
}
static void call_exit(void)
{
	printk("call call_exit \n");
	unregister_chrdev(CALL_DEV_MAJOR, CALL_DEV_NAME);
}

module_init(call_init);
module_exit(call_exit);
MODULE_LICENSE("Dual BSD/GPL");
