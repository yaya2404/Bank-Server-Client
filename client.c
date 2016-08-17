#include "multithread.h"
/*
 * A thread that reads messages from bank server and prints them out to stdout
 */
void *outputThread(void *vargp){
	int sockfd = *((int *) vargp);
	char buffer[256];

	while(1){
		bzero(buffer,255);
		if(read(sockfd, buffer, sizeof(buffer)) < 0){
			printf("Error: %s\n", strerror(errno));
		}else{
			if(strlen(buffer) == 0){
				printf("Client disconnected from server\n");
				return NULL;
			}else{
				printf("%s\n", buffer);
			}
		}
	}
	return NULL;
}
/*
 * A thread that reads in user input using stdin and sends input to bank server
 */
void *inputThread(void *vargp){
	int sockfd = *((int *) vargp);
	char buffer[256];
	pthread_detach(pthread_self());	

	while(1){
		sleep(2);
		bzero(buffer, 255);
		fgets(buffer, 255, stdin);
		write(sockfd, buffer, strlen(buffer));
		if(strcmp(buffer, "exit\n") == 0){
			return NULL;
		}
	}
	return NULL;
}
/*
 * main thread that creates socket and binds socket with address to bank server. The main thread takes in host name where bank server is located. 
 */
int main(int argc, char *argv[]){
		
	int *sockfd;
	struct sockaddr_in servaddr;
	struct hostent *server;
	pthread_t bid, oid;; 

	if(argc != 2){
		printf("Error: Invalid input\n");
		exit(0);
	}
	
	//creates socket for client 
	sockfd = malloc(sizeof(int *));
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if(sockfd < 0){
		printf("Error: %s\n", strerror(errno));
		exit(0);
	}	
	server = gethostbyname(argv[1]);
	if(server == NULL){
		printf("Error: Invalid host name\n");
		exit(0);	
	}
	bzero((char *) &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);
	servaddr.sin_port = htons(7291);

	//continually connects to bank server until connection is established. 
	while(1){
		if(connect(*sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){ 
			printf("Error: %s\nRetrying connection...\n", strerror(errno));
			sleep(3);
		}else{
			printf("Connection established with server\n");
			pthread_create(&bid, NULL, inputThread, sockfd);
			pthread_create(&oid, NULL, outputThread, sockfd);
			pthread_join(oid, NULL);
			break;
		}
	}
	close(*sockfd);
	free(sockfd);
	return 0;
}
