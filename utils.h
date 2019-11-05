#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h>
#include <unistd.h> 
#include <string.h> 
#include <time.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include <pthread.h>
#include "msg.h"

#define BUFFER_SIZE     1024
#define CLIENT_NUM      100
#define SMALL_SIZE      100
#define SESSION_NUM     300
#define SA struct sockaddr

#define MAX_NAME        1024
#define MAX_DATA        1024

struct message {
    unsigned int type;
    unsigned int size;
    char source[MAX_NAME];
    char data[MAX_DATA];
};
typedef struct client {
    char id[SMALL_SIZE];
    in_addr_t ipaddr;
    in_port_t port;
    int closed;
    int connfd;
    char session[SMALL_SIZE];
    pthread_t *handler;
} Client;

void sock_init(int *sockfd, struct sockaddr_in *servaddr);
int server_authenticate(char *id, char *pswd);
void* server_client_handler(void *client);
void server_msg_handler(struct message *msg, Client* this_client, Client *all_clients, char **sessions, int *ses_cnt);
void client_exec(char *cmd, char **args, int argc, int *sockfd, struct sockaddr_in *servaddr, char *id);
void client_connect(int sockfd, struct sockaddr_in *servaddr, char *ipaddr, int port);
void client_login(char *id, char *pswd, int sockfd);
void* listener(void *vsockfd);

#endif