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

static int ledkey_init(void)
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

	for(i =0; i < ARRAY_SIZE(key); i++)
	{
		ret = gpio_request(key[i], "gpio key");
		if(ret <0){	
            printk("#### FAILED Request gpio %d. error : %d \n", key[i], ret);
		}
	}

    return ret;
}
static void ledkey_exit(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(led); i++){
        gpio_free(led[i]);
    }
    for (i = 0; i < ARRAY_SIZE(key); i++){
        gpio_free(key[i]);
    }
}
void led_write(unsigned long data)
{
    int i;
    for(i = 0; i < ARRAY_SIZE(led); i++){
       // gpio_direction_output(led[i],(data>>i) &0x01);
        gpio_set_value(led[i], (data >> i ) & 0x01);
    }
}
//void led_read(unsigned long * led_data)
void key_read(char* key_data)
{
    int i;
    unsigned long data=0;
    unsigned long temp;
    for(i=0;i<8;i++)
    {
        gpio_direction_input(key[i]); //error led all turn off
        temp = gpio_get_value(key[i]) << i;
        data |= temp;
    }
	for(i=0;i<8;i++)
	{
		if((data >> i) == 0x01)
		{
    		*key_data =(char)(i+1);
			break;
		}
	}
    return;
}


static int call_open(struct inode *inode, struct file *filp)
{
	int ret;
	ret = ledkey_init();
    return ret;
}
static ssize_t call_read(struct file *filp, char * buf, size_t count, loff_t *f_pos)
{
	key_read(buf);
	return count;
}
static ssize_t call_write(struct file *filp, char * buf, size_t count, loff_t *f_pos)
{
	led_write((unsigned long)*buf);
	return count;
}
static ssize_t call_release(struct inode *inode, struct file *filp)
{
	ledkey_exit();
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
