#include "utils.h"

Client *clients;

int main(int argc, char **argv) {

    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli;
    int client_cnt = 0;
    clients = (Client *)malloc(CLIENT_NUM * sizeof(Client));

    for (int i = 0; i < CLIENT_NUM; i++) {
        clients[i].closed = 0;
        strcpy(clients[i].id, "");
        clients[i].connfd = -1;
        strcpy(clients[i].session, "NaN");
        clients[i].handler = malloc(sizeof(pthread_t));
    }
    
    if (argc <= 1) {
        printf("Usage: server <TCP port number to listen on>\n");
        exit(0);
    }
  
    sock_init(&sockfd, &servaddr);
  
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(atoi(argv[1])); 

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Socket bind failed.\n"); 
        exit(0); 
    } 

    while(1) {
        if ((listen(sockfd, 5)) != 0) { 
            printf("Listen failed...\n"); 
            exit(0); 
        } 
        len = sizeof(cli); 
        connfd = accept(sockfd, (SA*)&cli, &len); 
        if (connfd < 0) { 
            printf("Incoming connection error.\n"); 
            exit(0); 
        } 
        else {
            printf("Incoming connection.\n");
            (clients + client_cnt)->connfd = connfd;
            (clients + client_cnt)->ipaddr = cli.sin_addr.s_addr;
            (clients + client_cnt)->port = cli.sin_port;
            int ret = pthread_create((clients + client_cnt)->handler, NULL, \
            server_client_handler, (void *)(clients + client_cnt));
            assert(!ret);
            client_cnt++;
        }
    }
  
    close(sockfd); 
    
}

void* server_client_handler(void *client) {
    Client *cclient = (Client *)client;
    int connfd = cclient->connfd;
    while(1) {
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        if (cclient->closed) {
            free(cclient);
            pthread_exit(NULL);
        }
        read(connfd, buffer, BUFFER_SIZE);
        struct message *msg = malloc(sizeof(struct message));
        char_to_struct(buffer, msg);
        server_msg_handler(msg, cclient, clients);
    }
    return 0;
}