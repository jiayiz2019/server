/*************************************************************************
	> File Name: udp_epoll.h
	> Author:Louis Tang 
	> Mail:1328147875@qq.com 
	> Created Time: Thu 09 Jul 2020 04:40:50 PM CST
 ************************************************************************/

#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H

int  udp_connect(struct sockaddr_in *client);
int  udp_accept(int fd, struct User *user);
void add_event_ptr(int epollfd, int fd, int events, struct User *user);
void del_event(int epollfd, int fd);
int  find_sub(struct User *team);
void add_to_sub_reactor(struct User *user);

#endif
