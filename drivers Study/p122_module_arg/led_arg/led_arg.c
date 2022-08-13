#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h>

#define DEBUG 1
#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

static unsigned long ledvalue = 0;

int led[] = {
    IMX_GPIO_NR(1, 16),   //16
    IMX_GPIO_NR(1, 17),   //17
    IMX_GPIO_NR(1, 18),   //18
    IMX_GPIO_NR(1, 19),   //19
};

module_param(ledvalue, ulong , 0);

void led_write(unsigned long data)
{
    int i;
    for(i = 0; i < ARRAY_SIZE(led); i++){
        gpio_set_value(led[i], (data >> i ) & 0x01);
    }
#if DEBUG
    printk("#### %s, data = %ld\n", __FUNCTION__, data);
#endif
}

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
	led_write(ledvalue);
	printk("<7>" "Hello, led, ledvalue=%ld\n",ledvalue);
	printk(KERN_DEBUG "Hello, led, ledvalue=%ld\n",ledvalue);
    return ret;
}

static void led_exit(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(led); i++){
		gpio_direction_output(led[i],0);
        gpio_free(led[i]);
    }
	printk("Goodbye, led");
}



module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("kcci");
MODULE_DESCRIPTION("test module");
MODULE_LICENSE("Dual BSD/GPL");
