#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
	
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>

#include <linux/gpio.h>
#include <asm/gpio.h>

#define DEBUG 1
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))
#define PIR IMX_GPIO_NR(3, 25)

static ssize_t drv_hw_pir_read(struct file * file, char * buf, size_t length, loff_t * ofs)
{
	int ret;
	int stat;

	stat = gpio_get_value(PIR);
	ret = copy_to_user(buf, &stat, sizeof(stat));

#if DEBUG
	printk("### [%s] stat = %d\n", __FUNCTION__,  stat);
#endif

	return (ret == 0) ? sizeof(stat) : ret;

	return 0;
}

static struct file_operations drv_hw_pir_fops = 
{
	.owner = THIS_MODULE,
	.read = drv_hw_pir_read,
};

static struct miscdevice drv_hw_pir_driver = 
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "drv_hw_pir",
	.fops = &drv_hw_pir_fops,
};

static int drv_hw_pir_init(void)
{
	int ret;

	ret = gpio_request(PIR, "gpio pir");

	if(ret){
		printk("#### FAILED Request gpio %d. error : %d \n", PIR, ret);
	} else {
		gpio_direction_input(PIR);
	}
	
	return misc_register(&drv_hw_pir_driver);
}

static void drv_hw_pir_exit(void)
{
	gpio_free(PIR);

	misc_deregister(&drv_hw_pir_driver);
	
}

module_init(drv_hw_pir_init);
module_exit(drv_hw_pir_exit);

MODULE_AUTHOR("PlanX Studio");
MODULE_DESCRIPTION("drv_hw_pir");
MODULE_LICENSE("Dual BSD/GPL");
