#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME  "/dev/leddev"

int main()
{
    int dev;
    char buff = 15;
    int ret;

    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
    ret = write(dev,&buff,sizeof(buff));
	if(ret < 0)
		perror("write()");
	buff = 0;
    ret = read(dev,&buff,sizeof(buff));              
	if(ret < 0)
		perror("read()");
	printf("ret : %d\n",buff);
    close(dev);
    return 0;
}
