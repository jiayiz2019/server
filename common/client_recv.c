/*************************************************************************
	> File Name: client_recv.c
	> Author:yubin 
	> Mail:2941012976@qq.com 
	> Created Time: Fri 10 Jul 2020 03:23:24 PM CST
 ************************************************************************/

#include "head.h"
extern int sockfd;
void *do_recv(void *arg) {
    while (1) {
        struct ChatMsg msg;
        bzero(&msg, sizeof(msg));
        int ret = recv(sockfd, (void *)&msg, sizeof(msg), 0);
        if (msg.type & CHAT_WALL) {
            printf(L_BLUE"<Public Chat>  From %s\n"NONE"%s\n", msg.name, msg.msg);
        } else if (msg.type & CHAT_MSG) {
            printf(L_PINK"<Private Chat>  From %s\n"NONE"%s\n", msg.name, msg.msg);
        } else if (msg.type & CHAT_SYS) {
            printf(YELLOW"<Server Info>"NONE" : %s\n", msg.msg);
        } else if (msg.type & CHAT_FIN) {
            printf(L_RED"<Server Info>"NONE"Server Down!\n");
            exit(1);
        } else if (msg.type & CHAT_FUNC) {
            printf(/*GREEN"Online"NONE*/"%s\n",msg.name);
	}
    }
}
