#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE  1460 
#define SERVERPORT 8000 

void* thread_main(void *arg);

pthread_mutex_t mutex;

int main(int argc, char *argv[]){

	int i;

	// socket
	int serv_sd, clnt_sd;

	//pthread
	pthread_t t_id[5];
	int thr_id[5];
	int status;		

	// addr
	struct sockaddr_in serveraddr, clientaddr;
	socklen_t clientaddr_s;
	int len;

	// addr
	serv_sd = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);

	// bind
	if((len = bind(serv_sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) == -1){
		printf("bind() failed\n");
		return 1;
	}
	
	// listen
	if((len = listen(serv_sd, 5)) == -1){
		printf("listen() failed\n");
		return 1;
	}
	
	// pthread init
	pthread_mutex_init(&mutex,NULL);

	i = 0;
	// accept
	while(1){
		clnt_sd = accept(serv_sd, (struct sockaddr*)&clientaddr, &clientaddr_s);
		if((thr_id[i] = pthread_create(&t_id[i],NULL,thread_main,(void*)&clnt_sd)) == -1){
			printf("pthread_create() failed\n");
			return 1;
		}
		i++;
		if( i > 4 || thr_id[i%5] == NULL )
			i = i % 5;
	}
	
	// pthread destroy
	pthread_mutex_destroy(&mutex);
	return 0;
}

void* thread_main(void *arg){
	
	int fsize, len;
	char buf[BUFSIZE];
	int clnt_sd = *((int*) arg);
	printf("clnt_sd = %d\n",clnt_sd);
	
	FILE* fp = fopen("final_test.mp4", "rb");
	fseek(fp, 0, SEEK_END);
    	fsize = ftell(fp);
    	fseek(fp, 0, SEEK_SET);
	
	printf("%d sending...\n",clnt_sd);
	while(1)
    	{
		pthread_mutex_lock(&mutex);
		len = fread(buf, 1, BUFSIZE, fp);
		pthread_mutex_unlock(&mutex);

		send(clnt_sd, buf, BUFSIZE, MSG_WAITALL);
		if (feof(fp)){
			break;
		}
    	}
	printf("%d sending complete.\n",clnt_sd);

	fclose(fp);
	close(clnt_sd);
    	
	return NULL;
}
