#include "utils.h"

char *id;
pthread_t *listent;
int sockfd, connfd;
Cache cache;

struct addrinfo *s;

int main(int argc, char **argv) {

    char cmd[1000];
    char *args[20];
    int n = 0;
    char *ptr;
    id = malloc(100);
    client_cache_init(&cache);
    strcpy(id, "NotLoggedIn"); 
    struct sockaddr_in servaddr;

    /* Init socket */
    sock_init(&sockfd, &servaddr);
    listent = malloc(sizeof(pthread_t));
    pthread_create(listent, NULL, listener, (void *)&sockfd);

    /* get auto login */
    FILE *configfp;
    configfp = fopen("client.config", "r");
    if (configfp == NULL) {
        printf("[Auto login configure file not found]\n");
    } else {
        char al_cmd[1000];
        int cnt = 1;
        char delim[] = ",";
        fscanf(configfp, "%s", al_cmd);
        char *ptr = strtok(al_cmd, delim);
        while(ptr != NULL)
        {
            args[cnt] = ptr;
            ptr = strtok(NULL, delim);
            cnt++;
        }
        args[0] = "/login";
        client_exec("", args, cnt, &sockfd, &servaddr, &cache, id);
        fclose(configfp);
    }

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
            fprintf(stderr, "[ERROR: Command/message too long]\n");
            break;
        }
        ptr = cmd; // parse the command
        for (n = 0; (args[n] = strtok(ptr, " \n")); n++, ptr = NULL);
        if (n <= 0) // no input
            continue;
        client_exec(resv_cmd, args, n, &sockfd, &servaddr, &cache, id);
    }
    
}

void* listener(void *vsockfd) {
    int *sockfd = (int *)vsockfd;
    while (!strcmp(id, "NotLoggedIn")) {
        if (cache.cache_int[0]) {
            // printf("Restart thread\n");
            // cache.cache_int[0] = 0;
            // pthread_create(listent, NULL, listener, (void *)&sockfd);
            // pthread_exit(NULL);
        }
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
        if (msg->type == INVIT_ACK) printf("[%s has accepted your invitation]\n", msg->source);
        if (msg->type == INVIT_NAK) printf("[Invitation to %s failed: %s]\n", msg->source, msg->data);
        if (msg->type == INVIT_RX) {
            printf("---------------------------\n[Initation from %s to join session %s]\nenter /y to accept, else to decline\n---------------------------\n", msg->source, msg->data);
            strcpy(cache.cache_char_a, msg->source);
            strcpy(cache.cache_char_b, msg->data);
            cache.waiting_status = WS_INVITED;
        }
        if (msg->type == LOGOUT) {
            printf("[You have been logged out by server due to inactivity]\n");
            exit(0);
        }
    }
    return 0;
}