#ifndef __MSG_H_
#define __MSG_H_

#include "utils.h"

#define LOGIN           0
#define LO_ACK          1
#define LO_NAK          2
#define EXIT            3
#define JOIN            4
#define MESSAGE         5

struct message * msg_init(int type);
void struct_to_char(struct message *org, char *res);
void char_to_struct(char *org, struct message *res);
void send_msg(struct message *msg, int sockfd, int free_msg);

#endif