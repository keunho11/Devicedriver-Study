#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "ioctl_test.h"

#define DEVICE_FILENAME  "/dev/ledkey_block_timer"

int main()
{
    ioctl_test_info info={0,{0}};
	int dev;
    char buff = 15;
    int ret;
    int key_old = 0;
	int sec, msec;
//    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
//	  dev = open( DEVICE_FILENAME, O_RDWR|O_NONBLOCK);
	dev = open( DEVICE_FILENAME, O_RDWR); // block mode
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
    ret = write(dev,&buff,sizeof(buff));
	if(ret < 0)
		perror("write()");
	buff = 0;
	do {
    	ret = read(dev,&buff,sizeof(buff));

		ioctl(dev, IOCTLTEST_READ, &info );
		sec = (int)(info.size/100);
		msec = (int)(info.size % 100);
		printf("intterupt time : %d.%d\n",sec, msec);
		printf("ret : %d\n", ret);
		if(ret < 0){
  			perror("read()");
			return 1;
		}
//		if(ret == 0)
//			printf("Time out!\n");

  		if(buff ==0) continue;//sw_no : 0
		if(buff != key_old)
		{
			if(buff)
			{
				printf("key_no : %d\n",buff);
				//printf();info
				write(dev,&buff,sizeof(buff));
			}
			if(buff == 8)
				break;
			key_old = buff;
		}
	} while(1);
    close(dev);
    return 0;
}
