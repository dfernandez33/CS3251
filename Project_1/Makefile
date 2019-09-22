#################################################################
##
## FILE:	Makefile
## PROJECT:	CS 3251 Project 1 - Professor Jun Xu
## DESCRIPTION: Compile Project 1
## CREDIT:	Adapted from Professor Traynor
##
#################################################################

CC=gcc

OS := $(shell uname -s)

# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: client server 

client: client.c
	$(CC) client.c -o client

server: server.c
	$(CC) -lcrypto server.c -o server

clean:
	    rm -f client server *.o

