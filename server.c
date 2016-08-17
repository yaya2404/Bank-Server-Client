#include "multithread.h"

int main (int argc, char *argv[]){
	l = (struct List *) malloc(sizeof(*l));
	l->size = 0;
	pthread_t tid;
	sem_init(&mutexOpen, 0, 1);

	//starts session acceptor thread
	pthread_create(&tid, NULL, sessionAThread, NULL);
	pthread_join(tid, NULL);

	free(l);	
	return 0;
}
