#include "utils.h"

Client *clients;
pthread_t *timeoutt;
char *sessions[SESSION_NUM];
int ses_cnt = 0;

int main(int argc, char **argv) {

    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli;
    int client_cnt = 0;
    clients = (Client *)malloc(CLIENT_NUM * sizeof(Client));


    for (int i = 0; i < CLIENT_NUM; i++) {
        reset_client(&clients[i]);
        clients[i].handler = malloc(sizeof(pthread_t));
    }

    timeoutt = malloc(sizeof(pthread_t));
    pthread_create(timeoutt, NULL, client_check_timeout, NULL);
    
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
        } else {
            printf("Incoming connection, save at slot %d\n", client_cnt);
            (clients + client_cnt)->connfd = connfd;
            (clients + client_cnt)->ipaddr = cli.sin_addr;
            (clients + client_cnt)->port = cli.sin_port;
            int ret = pthread_create((clients + client_cnt)->handler, NULL, \
            server_client_handler, (void *)(clients + client_cnt));
            assert(!ret);
            client_cnt++;
            client_cnt %= CLIENT_NUM;
            while (strcmp((clients + client_cnt)->id, "")) {
                client_cnt++;
                client_cnt %= CLIENT_NUM;
            }
        }
    }
  
    close(sockfd); 
    
}

void* server_client_handler(void *client) {
    Client *cclient = (Client *)client;
    int connfd = cclient->connfd;
    gettimeofday(&cclient->start, NULL);
    while (1) {
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        if (cclient->closed) {
            printf("Exit thread.\n");
            reset_client(cclient);
            pthread_exit(NULL);
        }
        read(connfd, buffer, BUFFER_SIZE);
        gettimeofday(&cclient->start, NULL);
        struct message *msg = malloc(sizeof(struct message));
        char_to_struct(buffer, msg);
        server_msg_handler(msg, cclient, clients, sessions, &ses_cnt);
    }
    return 0;
}

void* client_check_timeout(void* unused) {
    printf("Timeout check start\n");
    while (1) {
        struct timeval now;
        double elapsedTime;
        gettimeofday(&now, NULL);
        for (int i = 0; i < CLIENT_NUM; i++) {
            if (!strcmp(clients[i].id, "")) continue;
            elapsedTime = (now.tv_sec - clients[i].start.tv_sec) * 1000.0;
            elapsedTime += (now.tv_usec - clients[i].start.tv_usec) / 1000.0;
            if (elapsedTime > TIMEOUT_TIME) {
                struct message *logout_msg = msg_init(LOGOUT);
                send_msg(logout_msg, clients[i].connfd, 1);
                reset_client(&clients[i]);
                clients[i].closed = 1;
            }
        }
    }
}
