/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	David Jose Fernandez
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <time.h>

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/

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

struct accounts {
    int mySavings;
    int myChecking;
    int myCD;
    int my401K;
    int my529;
};

struct withdraws {
    int mySavings;
    int myChecking;
    int myCD;
    int my401K;
    int my529;
};

int getBalance(int account, struct accounts *accounts);
void updateBalance(int account, struct accounts *accounts, int amount);
void updateWithdraws(int account, struct withdraws *withdraws);
int checkWithdraws(int account, struct withdraws *withdraws);

/* The main function */
int main(int argc, char *argv[])
{

    char *ipAddr;
    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in serverAddr;		/* Local address */
    struct sockaddr_in clntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */

    int requestBuf[BUFSIZE];			/* Buff to store account name from client */
    int requestOperation;

    // set up account balances
    struct accounts accounts;
    accounts.mySavings = 100;
    accounts.myChecking = 120;
    accounts.myCD = 200;
    accounts.my401K = 15;
    accounts.my529 = 50;

    // set number of withdraws for all accounts initially to 0
    struct withdraws withdraws;
    withdraws.mySavings = 0;
    withdraws.myChecking = 0;
    withdraws.myCD = 0;
    withdraws.my401K = 0;
    withdraws.my529 = 0;

    //clear nameBuff
    memset(&requestBuf, 0, BUFSIZE);
    /* Get the IP Address and Port from the command line */
    if (argc != 3)
    {
        printf("Incorrect number of arguments. The correct format is:\n\t ipAddr port");
        exit(1);
    }
    /*Read Command Line Arguments*/
    ipAddr = argv[1];
    changeServPort = (unsigned short) atoi(argv[2]);

    /* Create new TCP Socket for incoming requests*/
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    /* Construct local address structure*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(changeServPort);
    inet_aton(ipAddr, &serverAddr.sin_addr);

    /* Bind to local address structure */
    bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    /* Listen for incoming connections */
    listen(serverSock, 10);

    /* Loop server forever*/
    printf("LISTENING\n");

    time_t baseTime, currTime;
    time(&baseTime);
    while(1)
    {
        // start recording the time

        /* Accept incoming connection */
        clientSock = accept(serverSock, (struct sockaddr *)&clntAddr, &clntLen);

        /* Extract request information and execute the appropriate commands */
        (int)recv(clientSock, &requestBuf, BUFSIZE, 0);
        /*The requestBuf has the following structure
         * requestBuf[0] = type of operation either BAL|WITHDRAW|TRASNFER
         * requestBuf[1] = has either account name or from_account name
         * requestBuf[2] = has either amount or name of to_account
         * requestBuf[3] = when this is used it will always have the amount to transfer
         */
        requestOperation = requestBuf[0];
        switch (requestOperation) {
            case BAL:
            {
                int balance = htonl(getBalance(requestBuf[1], &accounts));
                send(clientSock, &balance, sizeof(balance), 0);
                break;
            }
            case WITHDRAW:
            {
                int account = requestBuf[1];
                updateWithdraws(account, &withdraws);
                if (checkWithdraws(account, &withdraws)) {
                    char mssg[31] = "Too many withdraws in a minute!";
                    send(clientSock, mssg, sizeof(mssg), 0);
                } else {
                    int amount = requestBuf[2];
                    int balance = getBalance(account, &accounts);
                    if (balance >= amount) {
                        updateBalance(account, &accounts, -amount);
                        char mssg[20] = "Withdraw Successful!";
                        send(clientSock, mssg, sizeof(mssg), 0);
                    } else {
                        char mssg[21] = "Insufficient funds :(";
                        send(clientSock, mssg, sizeof(mssg), 0);
                    }
                }
                break;
            }
            case TRANSFER:
            {
                int from_account = requestBuf[1];
                int to_account = requestBuf[2];
                int amount = requestBuf[3];
                int from_account_balance = getBalance(from_account, &accounts);
                if (from_account_balance >= amount) {
                    updateBalance(from_account, &accounts, -amount);
                    updateBalance(to_account, &accounts, amount);
                    char mssg[20] = "Transfer Successful!";
                    send(clientSock, mssg, sizeof(mssg), 0);
                } else {
                    char mssg[34] = "Insufficient funds for transfer :(";
                    send(clientSock, mssg, sizeof(mssg), 0);
                }
                break;

            }
            default:
            {
                char mssg[23] = "Can't recognize command";
                send(clientSock, mssg, sizeof(mssg), 0);
                break;
            }
        }
        close(clientSock);
        time(&currTime);
        if (difftime(baseTime, currTime) >= 60) {
            time(&baseTime);    //reset base time
            // reset withdraw count
            withdraws.mySavings = 0;
            withdraws.myChecking = 0;
            withdraws.myCD = 0;
            withdraws.my401K = 0;
            withdraws.my529 = 0;
        }
    }

}

int getBalance(int account, struct accounts *accounts) {
    if (account == mySavings) {
        return accounts->mySavings;
    } else if (account == myChecking) {
        return accounts->myChecking;
    } else if (account == myCD) {
        return accounts->myCD;
    } else if (account == my401K) {
        return accounts->my401K;
    } else if (account == my529) {
        return accounts->my529;
    } else {
        return -1;
    }
}

void updateBalance(int account, struct accounts *accounts, int amount) {
    if (account == mySavings) {
        accounts->mySavings += amount;
        return;
    } else if (account == myChecking) {
        accounts->myChecking += amount;
        return;
    } else if (account == myCD) {
        accounts->myCD += amount;
        return;
    } else if (account == my401K) {
        accounts->my401K += amount;
        return;
    } else if (account == my529) {
        accounts->my529 += amount;
        return;
    }
}

void updateWithdraws(int account, struct withdraws *withdraws) {
    if (account == mySavings) {
        withdraws->mySavings++;
        return;
    } else if (account == myChecking) {
        withdraws->myChecking++;
        return;
    } else if (account == myCD) {
        withdraws->myCD++;
        return;
    } else if (account == my401K) {
        withdraws->my401K++;
        return;
    } else if (account == my529) {
        withdraws->my529++;
        return;
    }
}

int checkWithdraws(int account, struct withdraws *withdraws) {
    if (account == mySavings) {
        return withdraws->mySavings >= 4;
    } else if (account == myChecking) {
        return withdraws->myChecking >= 4;
    } else if (account == myCD) {
        return withdraws->myCD >= 4;
    } else if (account == my401K) {
        return withdraws->my401K >= 4;
    } else if (account == my529) {
        return withdraws->my529 >= 4;
    }
}