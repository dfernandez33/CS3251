/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	David Jose Fernandez
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu 
* DESCRIPTION:	Network Client Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum operations {
    BAL,
    WITHDRAW,
    TRANSFER
};

enum acount_type {
    mySavings,
    myChecking,
    myCD,
    my401K,
    my529
};

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define REPLYLEN 32

void buildMssg(char *args[], int *mssg);
int nameToEnum(char *name);

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* server address structure */

    int operation;            /* Operation Name */
    char *servIP;		    /* Server IP address  */
    unsigned short servPort;	    /* Server Port number */


    int sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */

    int balance;		    /* Account balance */

    if (argc < 5) {
        printf("At least five arguments are required\n");
        exit(1);
    } else {
        /* Extract common fields from different message types.
         * Message specific fields like from-account, to-account will be retrieved when contructing the message
         * for the server.
         */
        switch (argc) {
            case 5: // BAL
                operation = BAL;
                servIP = argv[3];
                servPort = (unsigned short) atoi(argv[4]);
                break;
            case 6: // WITHDRAW
                operation = WITHDRAW;
                servIP = argv[4];
                servPort = (unsigned short) atoi(argv[5]);
                break;
            case 7: // TRANSFER
                operation = TRANSFER;
                servIP = argv[5];
                servPort = (unsigned short) atoi(argv[6]);
                break;
            default:
                printf("No support for the number of arguments passed in.\n");
                exit(1);

        }
    }

    memset(&sndBuf, 0, SNDBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);

    /* Create a new TCP socket*/
    clientSock = socket(AF_INET, SOCK_STREAM, 0);

    /* Construct the server address structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(servPort);
    inet_aton(servIP, &serv_addr.sin_addr);

    /* Establish connection to the server */
    connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    /* Build message to send to the server */
    sndBuf[0] = operation;
    buildMssg(argv, sndBuf);

    /* Send the string to the server */
    send(clientSock, sndBuf, SNDBUFSIZE, 0);

    /* Receive and print response from the server */
    if (operation == BAL) {
        recv(clientSock, &balance, REPLYLEN, 0);
        close(clientSock);
        balance = ntohl(balance);
        printf("Balance is: %i\n", balance);
    } else if (operation == WITHDRAW | operation == TRANSFER) {
        recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
        close(clientSock);
        printf("%s\n", rcvBuf);
    }

    return 0;
}

void buildMssg(char *args[], int *mssg) {
    // mssg[0] will always contain the type of operation that is being executed
    if (mssg[0] == BAL) {
        int account = nameToEnum(args[2]);
        mssg[1] = account;
        return;
    } else if (mssg[0] == WITHDRAW) {
        int account = nameToEnum(args[2]);
        mssg[1] = account;
        mssg[2] = atoi(args[3]);
        return;
    } else if (mssg[0] == TRANSFER) {
        int from_account = nameToEnum(args[2]);
        int to_account = nameToEnum(args[3]);
        mssg[1] = from_account;
        mssg[2] = to_account;
        mssg[3] = atoi(args[4]);
        return;
    } else {
        printf("The message type entered was invalid.\n");
        exit(1);
    }
}

int nameToEnum(char *name) {
    if (strcmp(name, "mySavings") == 0) {
        return mySavings;
    } else if (strcmp(name, "myChecking") == 0) {
        return myChecking;
    } else if (strcmp(name, "myCD") == 0) {
        return myCD;
    } else if (strcmp(name, "my401K") == 0) {
        return my401K;
    } else if (strcmp(name, "my529") == 0) {
        return my529;
    } else {
        printf("One of the account names entered was invalid.\n");
        exit(1);
    }
}