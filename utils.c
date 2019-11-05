#include "utils.h"
#include "msg.h"

/* server authenticate if the login is successful */
int server_authenticate(char *id, char *pswd) {
    if (!strcmp(id, "user1")) {
        if (!strcmp(pswd, "password")) return 1;
    } else if (!strcmp(id, "user2")) {
        if (!strcmp(pswd, "password")) return 1;
    }
    return 0;
}

/* main func for client to execute commands */
void client_exec(char *cmd, char **args, int argc, int *sockfd, struct sockaddr_in *servaddr, char *id) {
    if (!strcmp(args[0], "/quit")) {
        exit(0);
    } else if (!strcmp(args[0], "/login")) {

        /* login */
        if (strcmp(id, "NotLoggedIn")) {
            printf("Already logged in.\n");
            return;
        }
        if (argc < 5) {
            printf("Usage: /login <client ID> <password> <server-IP> <server-port>\n");
            return;
        }
        int port = atoi(args[4]);
        client_connect(*sockfd, servaddr, args[3], port);
        client_login(args[1], args[2], *sockfd);
        /* check response */
        char buffer[BUFFER_SIZE];
        struct message *resp;
        read(*sockfd, buffer, sizeof(buffer));
        resp = (struct message *)malloc(sizeof(struct message));
        char_to_struct(buffer, resp);
        if (resp->type == LO_ACK) {
            printf("Login successful.\n");
            strcpy(id, args[1]);
        } else {
            printf("Invalid username or password.\n");
            close(*sockfd);
            sock_init(sockfd, servaddr);
        }
        free(resp);

    } else if (!strcmp(args[0], "/logout")) {

        /* logout */
        close(*sockfd);
        sock_init(sockfd, servaddr);
        id = "NotLoggedIn";
        printf("Logged out from server.\n");

    } else if (!strcmp(args[0], "/list")) {

        /* list */
        struct message *query = msg_init(QUERY);
        strcpy(query->source, id);
        send_msg(query, *sockfd, 1);

    } else if (!strcmp(args[0], "/createsession")) {

        /* create new session */
        if (argc < 2) {
            printf("Usage: /createsession <session ID>\n");
            return;
        }
        struct message *new_sess_req = msg_init(NEW_SESS);
        strcpy(new_sess_req->source, id);
        strcpy(new_sess_req->data, args[1]);
        send_msg(new_sess_req, *sockfd, 1);

    } else if (!strcmp(args[0], "/joinsession")) {

        /* join session */
        if (argc < 2) {
            printf("Usage: /joinsession <session ID>\n");
            return;
        }
        struct message *join_req = msg_init(JOIN);
        strcpy(join_req->source, id);
        strcpy(join_req->data, args[1]);
        send_msg(join_req, *sockfd, 1);

    } else if (!strcmp(args[0], "/leavesession")) {

        /* leave session */
        struct message *leave_req = msg_init(LEAVE_SESS);
        strcpy(leave_req->source, id);
        send_msg(leave_req, *sockfd, 1);
        printf("[Left current session]\n");

    } else if (strcmp(id, "NotLoggedIn")) {

        /* send message */
        struct message *user_msg = msg_init(MESSAGE);
        strcpy(user_msg->source, id);
        strcpy(user_msg->data, cmd);
        send_msg(user_msg, *sockfd, 1);

    } else {
        printf("Invalid command.\n");
    }
}

/* init the socket */
void sock_init(int *sockfd, struct sockaddr_in *servaddr) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (*sockfd == -1) { 
        printf("socket creation failed.\n"); 
        exit(0); 
    } 
    bzero(servaddr, sizeof(*servaddr));
}

/* init the client's connection to the server */
void client_connect(int sockfd, struct sockaddr_in *servaddr, char *ipaddr, int port) {
    servaddr->sin_family = AF_INET; 
    servaddr->sin_addr.s_addr = inet_addr(ipaddr); 
    servaddr->sin_port = htons(port); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)servaddr, sizeof(*servaddr)) != 0) { 
        printf("Connection with the server failed.\n"); 
    } else {
        printf("Connected to the server.\n");
    }
}

/* when a message comes, server decides how to handle it */
void server_msg_handler(struct message *msg, Client* this_client, Client *all_clients, char **sessions, int *ses_cnt) {
    switch (msg->type) {
    case LOGIN:

        /* handling login request */
        printf("Received login request.\n");
        struct message *login_resp;
        if (server_authenticate(msg->source, msg->data)) {
            login_resp = msg_init(LO_ACK);
            strcpy(this_client->id, msg->source);
        } else {
            login_resp = msg_init(LO_NAK);
            // this_client->closed = 1;
        }
        send_msg(login_resp, this_client->connfd, 1);
        break;

    case MESSAGE:

        /* handling new message */
        printf("Received message.\n");
        for (int i = 0; i < CLIENT_NUM; i++) {
            if (!strcmp(all_clients[i].id, "")) continue;
            if (!strcmp(all_clients[i].session, this_client->session) && strcmp(all_clients[i].session, "-"))
                send_msg(msg, all_clients[i].connfd, 0);
        }
        free(msg);
        break;

    case QUERY:

        /* handling query request */
        printf("Received query request.\n");
        struct message *query_resp = msg_init(QU_ACK);
        int cli_cnt = 0;
        char line[30] = "---------------------------\n";
        strcat(query_resp->data, line);
        for (int i = 0; i < CLIENT_NUM; i++) {
            if (!strcmp(all_clients[i].id, "")) continue;
            cli_cnt++;
            char entry[SMALL_SIZE];
            sprintf(entry, "%s | IP %d | Port %d | Joined session %s\n", \
                    all_clients[i].id, \
                    all_clients[i].ipaddr, \
                    all_clients[i].port, \
                    all_clients[i].session);
            strcat(query_resp->data, entry);
        }
        char ending[SMALL_SIZE];
        sprintf(ending, "%d active clients in total\n", cli_cnt);
        strcat(query_resp->data, ending);
        strcat(query_resp->data, line);
        send_msg(query_resp, this_client->connfd, 1);
        break;

    case NEW_SESS:

        /* handling create session request */
        printf("Received create new session request.\n");
        struct message *new_sess_resp;
        for (int i = 0; i < *ses_cnt; i++) {
            if (!strcmp(sessions[i], msg->data)) {
                new_sess_resp = msg_init(NS_NAK);
                strcpy(new_sess_resp->data, "Duplicate session name");
                send_msg(new_sess_resp, this_client->connfd, 1);
                return;
            }
        }
        sessions[*ses_cnt] = malloc(SMALL_SIZE);
        strcpy(sessions[*ses_cnt], msg->data);
        *ses_cnt = *ses_cnt + 1;
        new_sess_resp = msg_init(NS_ACK);
        strcpy(new_sess_resp->data, msg->data);
        send_msg(new_sess_resp, this_client->connfd, 1);
        break;
    
    case JOIN:

        /* handling join session request */
        printf("Received join session request.\n");
        struct message *join_resp;
        for (int i = 0; i < *ses_cnt; i++) {
            if (!strcmp(sessions[i], msg->data)) {
                strcpy(this_client->session, msg->data);
                join_resp = msg_init(JN_ACK);
                strcpy(join_resp->data, msg->data);
                send_msg(join_resp, this_client->connfd, 1);
                return;
            }
        }
        join_resp = msg_init(JN_NAK);
        strcpy(join_resp->data, "Session not found");
        send_msg(join_resp, this_client->connfd, 1);
        break;

    case LEAVE_SESS:

        /* handling leave session request */
        printf("Received leave session request.\n");
        strcpy(this_client->session, "-");

    default:
        break;
    }
}

void client_login(char *id, char *pswd, int sockfd) {
    printf("Logging in as %s...\n", id);
    struct message *lo_msg = msg_init(LOGIN);
    strcpy(lo_msg->source, id);
    strcpy(lo_msg->data, pswd);
    lo_msg->size = strlen(pswd);
    send_msg(lo_msg, sockfd, 1);
}
