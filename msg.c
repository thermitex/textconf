#include "utils.h"

struct message * msg_init(int type) {
    struct message *msg = malloc(sizeof(struct message));
    msg->type = type;
    msg->size = 0;
    strcpy(msg->source, "");
    strcpy(msg->data, "");
    return msg;
}

void send_msg(struct message *msg, int sockfd, int free_msg) {
    char *to_be_sent = malloc(BUFFER_SIZE);
    struct_to_char(msg, to_be_sent);
    write(sockfd, to_be_sent, BUFFER_SIZE);
    free(to_be_sent);
    if (free_msg) free(msg);
}

void struct_to_char(struct message *org, char *res) {
    char type_buf[5];
    char size_buf[5];
    sprintf(type_buf, "%d", org->type);
    sprintf(size_buf, "%d", org->size);
    char *new = malloc(BUFFER_SIZE);
    int msg_ptr = 0;
    for (int i = 0; i < strlen(type_buf); i++) {
        new[msg_ptr++] = type_buf[i];
    }
    new[msg_ptr++] = ':';
    for (int i = 0; i < strlen(size_buf); i++) {
        new[msg_ptr++] = size_buf[i];
    }
    new[msg_ptr++] = ':';
    for (int i = 0; i < strlen(org->source); i++) {
        new[msg_ptr++] = org->source[i];
    }
    new[msg_ptr++] = ':';
    for (int i = 0; i < strlen(org->data); i++) {
        new[msg_ptr++] = org->data[i];
    }
    new[msg_ptr] = '\0';
    strcpy(res, new);
    free(new);
}

void char_to_struct(char *org, struct message * res) {
    int colon_cnt = 0;
    res->type = 0;
    res->size = 0;
    int src_ptr = 0, dta_ptr = 0;
    for (int i = 0; i < strlen(org); i++) {
        if (org[i] == ':') {
            if (colon_cnt == 2) res->source[src_ptr] = '\0';
            if (colon_cnt > 2) res->data[dta_ptr++] = org[i];
            colon_cnt++;
            continue;
        }
        switch (colon_cnt) {
            case 0:
                res->type *= 10;
                res->type += (int)(org[i] - '0');
                break;
            case 1:
                res->size *= 10;
                res->size += (int)(org[i] - '0');
                break;
            case 2:
                res->source[src_ptr] = org[i];
                src_ptr++;
                break;
            default:
                res->data[dta_ptr] = org[i];
                dta_ptr++;
        }
    }
    res->data[dta_ptr] = '\0';
}