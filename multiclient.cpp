#define SERVERIP "192.168.52.130"
#define SERVERPORT 8000
#define BUFSIZE 1460

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <linux/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

int main(int argc,char* argv[]){

	// TIME
	time_t s_time, e_time;
	float gap;

	// SOCK
	int sock;
	int state, opt_val;
	struct sockaddr_in serveraddr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// FILE
	char buf[BUFSIZE];
	int len;
	FILE *fw = NULL;
	fw = fopen("./final_test.mp4","wb+");
	if(fw == NULL){
		printf("cannot open this file\n");
	}

	if(connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		printf("connect() failed\n");
		return -1;
	}	
	
	state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&buf, sizeof(buf));
	if (state){
		printf("socket option error\n");
		return -1;
	}

	opt_val = 1;
	state = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val));
	if (state){
		printf("socket option error\n");
		return -1;
	}
	
	// RECV
	s_time = clock();
	while (1){
		len = recv(sock, buf, BUFSIZE, MSG_WAITALL);
		if (len == -1) {
			printf("recv() failed.\n");
			break;
		}else if (len == 0){
			break;
		}

		fwrite(buf, sizeof(char), BUFSIZE, fw);
	}
	e_time = clock();
	gap = (float)(e_time - s_time) / (float)1000;
	printf("최종 시간: %.4f초\n",gap);
	
	return 0;
}
