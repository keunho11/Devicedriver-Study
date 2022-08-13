#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILENAME  "/dev/ledkey"

int main()
{
    int dev;
    char buff[20] = {"led1"};
    int ret;
    int key_old = 0;

    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
    ret = write(dev,buff,strlen(buff));
	if(ret < 0)
		perror("write()");
	do {
    	ret = read(dev,buff,sizeof(buff));              
  		if(ret < 0)
  			perror("read()");
		printf("ret : %d, buf : %s\n",ret,buff);
		strncpy(buff,"led",3);
		write(dev,buff,strlen(buff));
/*
		if(buff != key_old)
		{
			if(buff)
			{
				printf("key_no : %d\n",buff);
				write(dev,&buff,sizeof(buff));
			}
			if(buff == 8)
				break;
			key_old = buff;
		}
*/
	} while(1);
    close(dev);
    return 0;
}
