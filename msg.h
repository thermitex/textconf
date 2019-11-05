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

struct message * msg_init(int type);
void struct_to_char(struct message *org, char *res);
void char_to_struct(char *org, struct message *res);
void send_msg(struct message *msg, int sockfd, int free_msg);

#endif