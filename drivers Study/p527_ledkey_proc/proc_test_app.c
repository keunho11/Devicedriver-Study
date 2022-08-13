#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>

#define DEVICE_FILENAME "/proc/ledkey/led"
#define DEVICE_FILENAME "/proc/ledkey/key"

int main(int argc, char *argv[])
{
	int leddev;
	int keydev;
	char buff[10] ={0};

	if(argc < 2)
	{
		printf("Usage : %s [0~127]\n",argv[0]);
		return 1;
	}

	leddev = open(DEVICE_FILENAME, O_WRONLY );
	keydev = open(DEVICE_FILENAME, O_RDONLY );
	if(leddev < 0)
	{
		perror("open");
		return 2;
	}
	if(keydev < 0)
	{
		perror("open");
		return 3;
	}
	strcpy(buff,argv[1]);
	write(dev,buff,strlen(buff));
	printf("write : %s\n",buff);
	read(dev,buff,strlen(buff));
	printf("read : %s\n",buff);
	close(leddev);
	close(keydev);
	return 0;
}
