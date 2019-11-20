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
#include <sys/time.h>
#include "msg.h"

#define BUFFER_SIZE     1024
#define CLIENT_NUM      100
#define SMALL_SIZE      100
#define SESSION_NUM     300
#define TIMEOUT_TIME    300000
#define SA struct sockaddr

#define MAX_NAME        1024
#define MAX_DATA        1024

#define WS_NULL         0
#define WS_INVITED      1

struct message {
    unsigned int type;
    unsigned int size;
    char source[MAX_NAME];
    char data[MAX_DATA];
};

typedef struct client {
    char id[SMALL_SIZE];
    struct in_addr ipaddr;
    in_port_t port;
    int closed;
    int connfd;
    struct timeval start;
    char session[SMALL_SIZE];
    pthread_t *handler;
} Client;

typedef struct cache {
    int cache_int[100];
    char cache_char_a[SMALL_SIZE];
    char cache_char_b[SMALL_SIZE];
    int waiting_status;
} Cache;

void sock_init(int *sockfd, struct sockaddr_in *servaddr);
int server_authenticate(char *id, char *pswd);
void* server_client_handler(void *client);
void server_msg_handler(struct message *msg, Client* this_client, Client *all_clients, char **sessions, int *ses_cnt);
void client_exec(char *cmd, char **args, int argc, int *sockfd, struct sockaddr_in *servaddr, Cache *cache, char *id);
int client_connect(int sockfd, struct sockaddr_in *servaddr, char *ipaddr, int port);
void client_login(char *id, char *pswd, int sockfd);
void* listener(void *vsockfd);
void reset_client(Client *client);
void client_cache_init(Cache *cache);
void* client_check_timeout(void*);

#endif
