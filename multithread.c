#include "multithread.h"
/*
 * Alarm function the prints the complete list of all accounts every 20 seconds. This functin prints the name, balance, and "IN SERVICE"
 */
void alarmHandler(int sig){

	int z;
	signal(SIGALRM, SIG_IGN);
	sem_wait(&mutexOpen);
	//Print information
	for(z = 0; z < l->size; z++){
		printf("Account name: %s\n", l->alist[z].name);
		printf("Balance: %f\n", l->alist[z].balance);
		if(l->alist[z].on == 1){
			printf("IN SERVICE\n");
		}
		printf("\n"); 
	}
	sem_post(&mutexOpen);
	alarm(20);
	signal(SIGALRM, alarmHandler);
}
/*
* Client session after connection is made. Runs command syntax.
* 
* buffer[strlen(buffer)-1] = '\0' is used to remove the newline character. Since my client program records user input from stdin, the newline character is also recorded which could cause errors for some strcmp functions.
* An easy way to remedy that issue is to remove the newline character and add a null terminating character 
*
*/
void *clientSession(void *vargp){
	
	int connfd = *((int *)vargp);
	int x, p, z;
	float val = 0.0;
	char buffer[256];
	pthread_detach(pthread_self());
	free(vargp);
	char *msg;
	
	msg = "Welcome to the Bank server.\n"
	write(connfd,msg,strlen(msg));
	while(1){

		bzero(buffer,255);
		msg = "Options:\nopen\nstart\nexit\n";
		write(connfd,msg,strlen(msg));
		if((x = read(connfd, buffer, 255)) < 0){
			write(connfd,"Error: server could not read client input", 41);
			continue;
		}

		buffer[strlen(buffer)-1] = '\0';	//removes new line character

		if(strcmp(buffer, "exit") == 0){
			close(connfd);
			printf("Server disconnected from client\n");
			return NULL;

		}else if(strncmp(buffer,"open", 4) == 0){

			strcpy(buffer, &buffer[5]);

			sem_wait(&mutexOpen);
			//checks if given name is already in bank
			for(p = 0; p < 20; p++){
				if(strcmp(l->alist[p].name, buffer) == 0){
					break;
				}
			}
			
			//error checking for redundant accounts and full bank
			if(p != 20){
				write(connfd, "Error: client name already exists", 33);
			}else if(l->size == 20){
				write(connfd, "Error: bank has full list of accounts", 37);
			}else{
			
				//adds account into bank
				strcpy(l->alist[l->size].name, buffer);
				l->alist[l->size].balance = 0;
				l->alist[l->size].on = 0;
				l->size++;
				write(connfd, "Account successfully opened", 27);
			}
			sem_post(&mutexOpen);

		}else if(strncmp(buffer, "start", 5) == 0){

			strcpy(buffer, &buffer[6]);

			//Error checks if given name is in session
			for(p = 0; p < 20; p++){
				if(strcmp(l->alist[p].name, buffer) == 0){
					break;
				}
			}

			if(l->alist[p].on == 1){
				write(connfd, "Error: given client name already in session", 43);
				continue;
			}else if(p == 20){
				write(connfd, "Error: account does not exist in bank", 37);
				continue;
			}
			
			//customer session begins here	
			write(connfd, "Entering client session", 23);
			msg = "Welcome to your account ";
			strcat(msg, buffer);
			write(connfd, msg, strlen(msg);
			l->alist[p].on = 1;
			while(1){

				msg = "Options:\ncredit\ndebit\nbalance\nfinish\n";
				write(connfd, msg, strlen(msg);
				//empties out buffer and checks for error
				bzero(buffer, 255);
				if(read(connfd, buffer, 255) < 0){
					write(connfd,"Error: server could not read client input", 41);
					continue;
				}
					
				buffer[strlen(buffer)-1] = '\0';	//removes new line character

				if(strncmp(buffer, "credit", 6) == 0){
				
					strcpy(buffer, &buffer[7]);
				
					//checks for invalid inputs before converting using atof()
					for(z = 0; z < strlen(buffer); z++){
						if(buffer[z] < 48 || buffer[z] > 57){
							break;
						}
					}
					if(z != strlen(buffer)){
						write(connfd, "Error: invalid input", 20);
						continue;
					}

					//adds to balance
					val = atof(buffer);
					l->alist[p].balance += val;
					write(connfd, "Credit transaction complete",27);

				}else if(strncmp(buffer, "debit", 5) == 0){
						
					strcpy(buffer, &buffer[6]);
		
					//checks for invalid inputs before converting using atof()
					for(z = 0; z < strlen(buffer); z++){
						if(buffer[z] < 48 || buffer[z] > 57){
							break;
						}
					}
					if(z != strlen(buffer)){
						write(connfd, "Error: invalid input", 20);
						continue;
					}

					//subtracts from balance
					val = atof(buffer);
					if(val > l->alist[p].balance){
						write(connfd, "Error: requested amount exceeds the current balance for your account", 68); 
					}else{
						l->alist[p].balance -= val;
						write(connfd, "Debit transaction complete", 26);
					}

				}else if(strncmp(buffer, "balance", 7) == 0){

					//displays balance
					snprintf(buffer, 255, "%f", l->alist[p].balance);
					write(connfd, "Balance: ", 9);	
					write(connfd, buffer, sizeof(buffer));

				}else if(strncmp(buffer, "finish", 6) == 0){

					//exits client session
					l->alist[p].on = 0;
					write(connfd, "Exiting client session", 22);
					break;

				}else if(strncmp(buffer, "exit", 4) == 0){

					//in case client wants to disconnect from server during customer session
					l->alist[p].on = 0;	
					close(connfd);
					printf("Server disconnected from client\n");
					return NULL;			
					
				}else{
			
					//error checking
					if(strncmp(buffer, "open", 4) == 0){
						write(connfd, "Error: cannot open account during customer session", 50);
					}else if(strncmp(buffer, "start", 5) == 0){
						write(connfd, "Error: cannot start account during customer session", 49);
					}else{
						write(connfd, "Error: invalid input",20);
					}

				}
			}//end of customer session

		}else{
			
			//error checking
			if(strncmp(buffer, "credit", 6) == 0 || strncmp(buffer, "debit", 5) == 0 || strncmp(buffer, "balance", 7) == 0 || strncmp(buffer, "finish", 6) == 0){
				write(connfd, "Error: command only valid in customer session", 44);
			}else{
				write(connfd, "Error: invalid input", 20);
			}

		}	
	}//end of client session 

	return NULL;	
}
/*
* Single session-acceptor thread that accepts incoming client connections from separate client processes.
*/
void *sessionAThread(void *vargp){
	int sockfd, *connfd;
	socklen_t clilen;
	struct sockaddr_in servaddr, cliaddr;
	pthread_t ctid;

	//Setup for 20 second alarm
	signal(SIGALRM, alarmHandler);
	alarm(20); 

	//Initialize socket for accepting client calls	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
   	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(7291);
	if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){ 
		printf("Error: %s\n", strerror(errno));
		close(sockfd);
		exit(0);	
	}
	listen(sockfd, 1024);

	//Infinite loop that waits for connection requests and exits when server recieves CTRL-C.
	while(1){
		clilen = sizeof(cliaddr);
		connfd = malloc(sizeof(int *));
		if((*connfd = accept(sockfd, (struct  sockaddr *) &cliaddr, &clilen)) < 0){
			printf("Error: %s\n", strerror(errno));
		}else{
			printf("Server established connection with client\n");
			pthread_create(&ctid, NULL, clientSession, connfd);
		}
	}
	close(sockfd);
	return NULL;
}
