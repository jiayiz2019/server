/*************************************************************************
	> File Name: server.c
	> Author:Louis Tang 
	> Mail:1328147875@qq.com 
	> Created Time: Thu 09 Jul 2020 10:51:58 AM CST
 ************************************************************************/

#include "head.h"

char *conf = "./soccer.conf";

struct Map court;              //足球场正式场地
struct Bpoint ball;            //球的位置
struct BallStatus ball_status; //球的状态
struct Score score;            //比分
int repollfd, bepollfd;
struct User *rteam, *bteam;
int port = 0;

struct task_queue redQueue;
struct task_queue blueQueue;

void closesrv(int signum) //关闭服务器（收到Ctrl + C后）
{
    struct ChatMsg msg;
    bzero(&msg, sizeof(struct ChatMsg));
    msg.type = CHAT_SYS;
    sprintf(msg.msg, "Server closed!\n");
    send_all(&msg);
    printf(GREEN"Server Info"NONE" : Server closed!\n");
    exit(0);
}

int main(int argc, char **argv)
{
    int opt, listener, epollfd;
    pthread_t red_t, blue_t;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s = -p port\n", argv[0]);
                exit(1);
        }
    }

    //判断配置文件的合法性

    if (!port) {
        port = atoi(get_conf_value(conf, "PORT"));
    }

    court.width = atoi(get_conf_value(conf, "COLS"));
    court.height = atoi(get_conf_value(conf, "LINES"));
    court.start.x = 3;
    court.start.y = 3;

    ball.x = court.width / 2;
    ball.y = court.height / 2;

    bzero(&ball_status, sizeof(ball_status));
    ball_status.who = -1;
    bzero(&score, sizeof(score));

    if ((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp()");
        exit(1);
    }

    DBG(GREEN"INFO"NONE" : Server start on port %d.\n", port);

    rteam = (struct User *)calloc(MAX, sizeof(struct User));
    bteam = (struct User *)calloc(MAX, sizeof(struct User));

    epollfd = epoll_create(MAX * 2);
    repollfd = epoll_create(MAX);
    bepollfd = epoll_create(MAX);

    if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
        perror("epoll_create()");
        exit(1);
    }

    task_queue_init(&redQueue, MAX, repollfd);
    task_queue_init(&blueQueue, MAX, bepollfd);

    pthread_create(&red_t, NULL, sub_reactor, (void *)&redQueue);
    pthread_create(&blue_t, NULL, sub_reactor, (void *)&blueQueue);

    struct epoll_event ev, events[MAX * 2];
    ev.events = EPOLLIN;
    ev.data.fd = listener;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl()");
        exit(1);
    }

    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    socklen_t len = sizeof(client);

    signal(SIGINT, closesrv);

    while (1) {
        DBG(YELLOW"Main Reactor"NONE" : Waiting for client.\n");
        int nfds = epoll_wait(epollfd, events, MAX * 2, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            struct User user;
            char buff[512] = {0};
            if (events[i].data.fd == listener) {
                int new_fd = udp_accept(listener, &user);
                if (new_fd > 0) {
                    struct ChatMsg air;
                    bzero(&air, sizeof(air));
                    sprintf(air.msg, GREEN"%s login!"NONE, user.name);
                    air.type = CHAT_SYS;
                    send_all(&air);
                    printf(GREEN"%s 上线啦\n"NONE, user.name);

                    add_to_sub_reactor(&user);
                }
            }
        }
    }

    return 0;
}
