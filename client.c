#include "utils.h"

char *id;

int main(int argc, char **argv) {

    char cmd[1000];
    char *args[20];
    int n = 0;
    char *ptr;
    id = malloc(100);
    strcpy(id, "NotLoggedIn");
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli;

    /* Init socket */
    sock_init(&sockfd, &servaddr);
    pthread_t *listent = malloc(sizeof(pthread_t));
    pthread_create(listent, NULL, listener, (void *)&sockfd);

    while (1) {
        // if (header) printf("[%s] ", id);
        // fflush(stdout);
        if (!fgets(cmd, 1000, stdin))
            break;
        char resv_cmd[1000];
        strcpy(resv_cmd, cmd);
        resv_cmd[strlen(cmd) - 1] = '\0';
        ptr = index(cmd, 0);
        if (ptr[-1] != '\n') {
            fprintf(stderr, "textconf.cli: command too long\n");
            break;
        }
        ptr = cmd; // parse the command
        for (n = 0; (args[n] = strtok(ptr, " \n")); n++, ptr = NULL);
        if (n <= 0) // no input
            continue;
        client_exec(resv_cmd, args, n, &sockfd, &servaddr, id);
    }
    
}

void* listener(void *vsockfd) {
    int *sockfd = (int *)vsockfd;
    while (!strcmp(id, "NotLoggedIn")) {
        ;
    }
    while (1) {
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        read(*sockfd, buffer, BUFFER_SIZE);
        struct message *msg = malloc(sizeof(struct message));
        char_to_struct(buffer, msg);
        if (msg->type == MESSAGE) printf("[%s] %s\n", msg->source, msg->data);
        if (msg->type == QU_ACK) printf("%s", msg->data);
        if (msg->type == NS_ACK) printf("[Successfully created session %s]\n", msg->data);
        if (msg->type == NS_NAK) printf("[Create new session failed: %s]\n", msg->data);
        if (msg->type == JN_ACK) printf("[Successfully joined session %s]\n", msg->data);
        if (msg->type == JN_NAK) printf("[Join session failed: %s]\n", msg->data);
    }
    return 0;
}