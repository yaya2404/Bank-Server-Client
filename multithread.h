#ifndef MULTITHREAD_H
#define MULTITHREAD_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
struct account {
	char name[100];
	float balance;
	int on;
};
struct List{
	struct account alist[20];
	int size;
};

void alarmHandler(int sig);

void *clientSession(void *vargp);

void *sessionAThread(void *vargp);

struct List *l;
sem_t mutexOpen;

#endif
