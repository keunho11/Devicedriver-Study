#ifndef __IOCTL_H__
#define __IOCTL_H__

#define IOCTLTEST_MAGIC 't'
typedef struct
{
	unsigned long intval;
} __attribute__((packed)) ioctl_test_info;

#define IOCTLTEST_INTVAL _IOR(IOCTLTEST_MAGIC, 0,ioctl_test_info)
#define IOCTLTEST_MAXNR			1
#endif
