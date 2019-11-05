#ifndef __MSG_H_
#define __MSG_H_

#include "utils.h"

#define LOGIN           102
#define LO_ACK          103
#define LO_NAK          104
#define EXIT            105
#define JOIN            106
#define MESSAGE         107
#define QUERY           108
#define QU_ACK          109
#define NEW_SESS        110
#define LEAVE_SESS      111
#define NS_ACK          113
#define NS_NAK          114
#define JN_ACK          115
#define JN_NAK          116

struct message * msg_init(int type);
void struct_to_char(struct message *org, char *res);
void char_to_struct(char *org, struct message *res);
void send_msg(struct message *msg, int sockfd, int free_msg);

#endif