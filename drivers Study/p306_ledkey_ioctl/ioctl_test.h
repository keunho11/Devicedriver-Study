#ifndef __IOCTL_H__
#define __IOCTL_H__

#define IOCTLTEST_MAGIC 't'
typedef struct
{
	unsigned long size;
	unsigned char buff[128];
} __attribute__((packed)) ioctl_test_info;

#define IOCTLTEST_KEYLEDINIT _IO(IOCTLTEST_MAGIC, 0) 
#define IOCTLTEST_KEYLEDFREE _IO(IOCTLTEST_MAGIC, 1)
#define IOCTLTEST_LEDOFF	_IO(IOCTLTEST_MAGIC, 2)
#define IOCTLTEST_LEDON		_IO(IOCTLTEST_MAGIC, 3)
#define IOCTLTEST_LEDWRITE	_IOR(IOCTLTEST_MAGIC, 4, ioctl_test_info)

#define IOCTLTEST_MAXNR		5
#endif
