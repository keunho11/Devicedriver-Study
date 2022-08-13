#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>

pthread_t thread;  /* 사용자 스레드 */

int thread_start_flag = 1;
int thr_id;

#define NODE_NAME "/dev/drv_hw_pir"

void *pir_thread(void * unuse) {
	int fd;
	int ret;
	int stat;

	if(access(NODE_NAME, F_OK)) {       /* 노드 파일이 존재하는지 검사 */
        system("insmod /mnt/nfs/drv_hw_pir.ko");      /* 드라이버 적재 */
    }
	fd = open(NODE_NAME, O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}

	while(thread_start_flag) {
		ret = read(fd, &stat, sizeof(stat));
		printf("--------------> %dbyte read, stat = %d\n", ret, stat);
		usleep(100000); // 0.1sec
	}

	close(fd);

	printf("#### Terminate thead...\n");
	pthread_exit((void *)fd);
}

int main(int argc, char * argv[]) 
{
	int status, i;

	thr_id = pthread_create(&thread, 0, pir_thread, 0);
	if (thr_id < 0) {
		printf("#### ERROR pthread_create\n");
		exit(0);
	}

	for(i = 0; i < 10; ++i){
		printf("count = %d\n", 9 - i);
		sleep(1);
	}

	thread_start_flag = 0;
	pthread_join(thread, (void **)&status);
	printf("#### Exit program, status = %d\n", status);

	exit(0);
}
