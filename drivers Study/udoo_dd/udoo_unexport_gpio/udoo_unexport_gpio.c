#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/gpio.h>
#include <asm/gpio.h>
#include <linux/kmod.h>

#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))
int led[] = {
    IMX_GPIO_NR(1, 16),
    IMX_GPIO_NR(1, 17),
    IMX_GPIO_NR(1, 18),
    IMX_GPIO_NR(1, 19),
};

int tac_sw[] = {
	IMX_GPIO_NR(1, 20),
	IMX_GPIO_NR(1, 21),
	IMX_GPIO_NR(4, 8),
	IMX_GPIO_NR(4, 9),
	IMX_GPIO_NR(4, 5),
	IMX_GPIO_NR(7, 13),
	IMX_GPIO_NR(1, 7),
	IMX_GPIO_NR(1, 8),
};

int pir = IMX_GPIO_NR(3, 25);

int fnd[] = {
    /* data */
    IMX_GPIO_NR(2, 0),
    IMX_GPIO_NR(2, 1),
    IMX_GPIO_NR(2, 2),
    IMX_GPIO_NR(2, 3),
    IMX_GPIO_NR(4, 28),
    IMX_GPIO_NR(4, 29),
    IMX_GPIO_NR(4, 30),
    IMX_GPIO_NR(4, 31),
    /* select */
    IMX_GPIO_NR(4, 27),
    IMX_GPIO_NR(5, 13),
    IMX_GPIO_NR(5, 14),
    IMX_GPIO_NR(5, 15),
    IMX_GPIO_NR(5, 16),
    IMX_GPIO_NR(5, 17),
};

int textlcd[] = {
    /* control */
    IMX_GPIO_NR(2, 8),	//0:command, 1:data
    IMX_GPIO_NR(2, 9),	//0:write, 1:read
    IMX_GPIO_NR(2, 10),	//command plus enable
    /* data */
    IMX_GPIO_NR(5, 5),
    IMX_GPIO_NR(5, 6),
    IMX_GPIO_NR(5, 7),
    IMX_GPIO_NR(5, 8),
    IMX_GPIO_NR(5, 9),
    IMX_GPIO_NR(5, 10),
    IMX_GPIO_NR(5, 11),
    IMX_GPIO_NR(5, 12),
};

int motor[] = {
    IMX_GPIO_NR(6, 1), 	//a_in1
    IMX_GPIO_NR(6, 0), 	//a_in2
    IMX_GPIO_NR(1, 9), 	//a_pwm
    IMX_GPIO_NR(6, 2), 	//b_in1
    IMX_GPIO_NR(5, 22), //b_in2
    IMX_GPIO_NR(1, 3), 	//b_pwm
};

int piezo[] = {
    IMX_GPIO_NR(1, 1),
};

static int init_udoo_unexport_gpio(void){
	int i;

	/* led */
	for (i = 0; i < ARRAY_SIZE(led); i++) {
		gpio_free(led[i]);
	}

	/* sw */
	for (i = 0; i < ARRAY_SIZE(tac_sw); i++) {
		gpio_free(tac_sw[i]);
	}

	/* pir */
	gpio_free(pir);

	/* fnd */
	for (i = 0; i < ARRAY_SIZE(fnd); i++) {
		gpio_free(fnd[i]);
	}

	/* textlcd */
	for(i = 0; i < ARRAY_SIZE(textlcd); ++i){
		gpio_free(textlcd[i]);
	}

	/* motor */
	for(i = 0; i < ARRAY_SIZE(motor); ++i){
		gpio_free(motor[i]);
	}

	/* piezo */
	for(i = 0; i < ARRAY_SIZE(piezo); ++i){
		gpio_free(piezo[i]);
	}

#if 0
	{
		int ret;
		char * run_program = "/data/local/tmp/remove_udoo_unexport_gpio";
		char *argv[] = {run_program, NULL};
		char *env[] = {"HOME=/", "PATH=/system/bin:/system/xbin", NULL};

		ret = call_usermodehelper(run_program, argv, env, UMH_NO_WAIT);
	}
#endif
	return 0;
}
static void exit_udoo_unexport_gpio(void){
	printk("%s\n", __FUNCTION__);
}

module_init(init_udoo_unexport_gpio);
module_exit(exit_udoo_unexport_gpio);

MODULE_AUTHOR("PlanX Studio");
MODULE_DESCRIPTION("udoo_unexport_gpio");
MODULE_LICENSE("Dual BSD/GPL");
