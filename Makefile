all: Server	Client

Server: server.c
	gcc -o Server server.c

Client: client.c
	gcc -o Client client.c