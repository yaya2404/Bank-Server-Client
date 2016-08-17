all: server client

server: server.c multithread.c
	gcc -Wall -g -lpthread -o server server.c multithread.c
client: client.c multithread.c 
	gcc -Wall -g -lpthread -o client client.c multithread.c
