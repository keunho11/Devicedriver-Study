#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

///////////////////////////////////////////////////////////////////

void str2int_test(void)
{
	long value;
	char *endptr;
	char *token[] = {"123", "123abc", "abc", "abc123"};
	int i;

	for(i = 0;i != (sizeof(token) / sizeof(token[0])); ++i){
	        value = simple_strtol(token[i], &endptr, 10);
	        if(value == 0 && endptr == token[i]){
	                printk("%s is not a valid number\n", token[i]);
	        }
	        else{
	                printk("%s is %ld\n", token[i], value);
	        }
	}
}

///////////////////////////////////////////////////////////////////

void int2str_test(void)
{
	char buf[128];

	snprintf(buf, sizeof(buf), "jiffies is %lld\n", get_jiffies_64());

	printk("%s", buf); /* use not '\n' */
}

///////////////////////////////////////////////////////////////////

void strfmt2int(void)
{
	char *buf = "HZ is 100";
	int hz = -1;

	sscanf(buf, "%d", &hz);
	printk("first: HZ is %d\n", hz);

	sscanf(buf, "HZ is %d", &hz);
	printk("second: HZ is %d\n", hz);
}

///////////////////////////////////////////////////////////////////

void strfind_test(void)
{
	char *buf = "welcome to kernel world";
	char *r;
	int len, i;

	len = strlen(buf);

	for(i = 0; i != len; ++i) {
	        r = strnchr(buf, i + 1, 'o');
	        if(r != NULL) {
	               printk("index: %d, string: %s\n", i, r);
	               break;
	        }
	}

	r = strstr(buf, "ker");
	printk("%s\n", (r != NULL) ? r : "not found");

	r = strstr(buf, "kerx");
	printk("%s\n", (r != NULL) ? r : "not found");
}

///////////////////////////////////////////////////////////////////

#include <linux/slab.h>

void kmalloc_test(void)
{
	int i;
	void * p;
	int unit = 4 * 1024;

	for(i = 0; i != 1024 * 100; ++i){
		p = (int *)kmalloc(unit * i, GFP_KERNEL);
		if(p == NULL){
			printk("not alloc : %dMByte\n", ((unit * i) / 1024) / 1024);
			break;
		}
		else{
			kfree(p);
		}
	}
}

///////////////////////////////////////////////////////////////////

#include <linux/delay.h>
#include <linux/kthread.h>

int my_thread(void *data){
	while(!kthread_should_stop()) {
		printk("running thread...\n");
		ssleep(1);
	}
	printk("stop thread...\n");

	return 0;
}

void kthread_test(void)
{
	struct task_struct *t;

	t = kthread_run(my_thread, NULL, "my_thread");
	ssleep(10);
	kthread_stop(t);
}

///////////////////////////////////////////////////////////////////

void ktime_test(void)
{
	char buf[128];

	struct timeval tv1;
	struct timespec tv2;
	unsigned long j1;
	unsigned long long j2;

	j1 = jiffies;
	j2 = get_jiffies_64();
	do_gettimeofday(&tv1);
	tv2 = current_kernel_time();

	sprintf(buf,
		"jiffies : 0x%08lx\n jiffies_64 : 0x%016Lx\n"
		"tv1 : %10i.%06i\n tv2 : %10i.%09i\n",
		j1, j2, (int)tv1.tv_sec, (int)tv1.tv_usec, (int)tv2.tv_sec, (int)tv2.tv_nsec);
	printk(buf);

}

///////////////////////////////////////////////////////////////////

#define START_WATCH(t, u){ \
	struct timeval my; \
	do_gettimeofday(&my); \
	t = (long)my.tv_sec;  \
	u = (long)my.tv_usec; \
}

#define STOP_WATCH(t, u, resule){ \
	struct timeval my; \
	do_gettimeofday(&my); \
	resule = ((long long)(my.tv_sec - t) << 32) | (my.tv_usec - u); \
}

int check_bit_loop1(unsigned long n)
{
	int bits = 0;

	while(n != 0)  {
		if(n & 1) bits++;
		n >>= 1;
	}

	return bits;
}

int check_bit_loop2(unsigned long n)
{
	int bits = 0;

	while(n != 0) {
		if(n & 1) bits++;
		if(n & 2) bits++;
		if(n & 4) bits++;
		if(n & 8) bits++;
		n >>= 4;
	}

	return bits;
}

void perform_test(void)
{
	int i, r1, r2;
	int sec, usec;
	long long time1, time2;

	START_WATCH(sec, usec);
	for(i = 0; i != 1000000; ++i){ r1 = check_bit_loop1(0xF13D); }
	STOP_WATCH(sec, usec, time1);

	START_WATCH(sec, usec);
	for(i = 0; i != 1000000; ++i){ r2 = check_bit_loop2(0xF13D); }
	STOP_WATCH(sec, usec, time2);

	printk("------>result: r1 = %d, r2 = %d, d1 = %lld, d2 = %lld\n", r1, r2, time1, time2);
}

///////////////////////////////////////////////////////////////////

void kdelay_test(void)
{
	int i;

	for(i = 0; i != 5; ++i){
		printk("first: current time: %lld\n", get_jiffies_64());
		schedule_timeout_interruptible(1 * HZ); //1 sec
	}

	for(i = 0; i != 5; ++i){
		printk("second current time: %lld\n", get_jiffies_64());
		schedule_timeout_interruptible((5 * HZ) / 100); //0.05 sec
	}
}

///////////////////////////////////////////////////////////////////

#include <linux/sched.h>

void kevent_delay_test(void)
{
	char buf[128];
	unsigned long j0, j1;
	wait_queue_head_t wait;

	init_waitqueue_head(&wait);
	j0 = jiffies;

	wait_event_interruptible_timeout(wait, 0, HZ * 5);
	j1 = jiffies;

	sprintf(buf, "%9li %9li\n", j0, j1);
	printk(buf);
}

///////////////////////////////////////////////////////////////////

struct timer_list tl;

void my_timer(unsigned long unuse){
	if(tl.data <= 0){
		return;
	}

	printk("call timer handler\n");

	tl.expires = get_jiffies_64() + (1 * HZ);
	tl.data -= 1;
	add_timer(&tl);
}

void ktimer_test(void)
{
	init_timer(&tl);
	tl.data = 5;
	tl.function = my_timer;
	tl.expires = get_jiffies_64() + (1 * HZ);
	init_timer(&tl);
	add_timer(&tl);
}

///////////////////////////////////////////////////////////////////

#include <linux/delay.h>
#include <linux/kthread.h>

static int my_thread2(void *data)
{
	while(!kthread_should_stop()){
		printk("default message...\n");
		printk(KERN_INFO "info message...\n");
		printk(KERN_ALERT "alert message...\n");

		ssleep(1);
	}

	printk("stop thread...\n");

	return 0;
}

static void printk_loglevel_test(void)
{
	struct task_struct *t;

	t = kthread_run(my_thread2, NULL, "my_thread");
	ssleep(30);
	kthread_stop(t);
}

///////////////////////////////////////////////////////////////////

static int mod_api_init(void)
{
	//str2int_test();
	//int2str_test();
	//strfmt2int();
	//strfind_test();
	//kmalloc_test();
	//kthread_test();
	//ktime_test();
	//perform_test();
	//kdelay_test();
	//kevent_delay_test();
	//ktimer_test();
	//printk_loglevel_test();
	
	return 0;
}

static void mod_api_exit(void)
{
	printk("%s\n", __FUNCTION__);
	
}

module_init(mod_api_init);
module_exit(mod_api_exit);

MODULE_AUTHOR("PlanX Studio");
MODULE_DESCRIPTION("mod_api");
MODULE_LICENSE("Dual BSD/GPL");
