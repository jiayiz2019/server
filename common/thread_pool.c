/*************************************************************************
	> File Name: thread_pool.c
	> Author:Louis Tang 
	> Mail:1328147875@qq.com 
	> Created Time: Thu 09 Jul 2020 02:49:38 PM CST
 ************************************************************************/

#include "head.h"

extern int repollfd, bepollfd;
extern struct User *rteam, *bteam;

void send_all(struct ChatMsg *msg)
{
    for(int i = 0; i < MAX; i++) {
        if(rteam[i].online) {
            send(rteam[i].fd, msg, sizeof(struct ChatMsg), 0);
        }
        if(bteam[i].online) {
            send(bteam[i].fd, msg, sizeof(struct ChatMsg), 0);
        }
    }
}

void do_work(struct User *user) {
    struct ChatMsg msg, air;
    bzero(&msg, sizeof(msg));
    bzero(&air, sizeof(air));

    recv(user->fd, (void *)&msg,sizeof(msg), 0);

    if(msg.type & CHAT_WALL) {
        printf("<%s> ~ %s \n",user->name, msg.msg);

        strcpy(air.msg, msg.msg);
        strcpy(air.name, user->name);
        air.type = CHAT_WALL;
        
        send_all(&air);
    } else if (msg.type&CHAT_MSG) {
        printf("<%s> $  %s \n",user->name, msg.msg);
    } else if (msg.type & CHAT_FIN) {
        user->online = 0;
        int epollfd = user->team ? bepollfd : repollfd;
        del_event(epollfd, user->fd);
        printf(L_YELLOW": %s logout!\n"NONE, user->name);

        sprintf(air.msg, L_YELLOW"%s 已下线"NONE, user->name);
        air.type = CHAT_SYS;
        send_all(&air);
        close(user->fd);
    } else if (msg.type & CHAT_FUNC) {
        printf(YELLOW"Online List : \n"NONE);
        for (int i = 0; i < MAX; i++) {
            if (rteam[i].online) {
                strcpy(msg.name, rteam[i].name);
                printf("%s   ",msg.name);
                //sprintf(buff,"%s ,",rteam[i].name);
                send(user->fd, (void *)&msg,sizeof(msg),0);
            }//printf("%s \n",rteam[i].name);
        }
        for (int i = 0; i < MAX; i++) {
            if (bteam[i].online){
                //sprintf(buff,"%s ,",bteam[i].name);
                strcpy(msg.name, bteam[i].name);
                printf("%s   ",msg.name);
                send(user->fd, (void *)&msg, sizeof(msg),0);
            }
        }
    }
}

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd)
{
    taskQueue->sum = sum;
    taskQueue->epollfd = epollfd;
    taskQueue->team = calloc(sum, sizeof(void *));
    taskQueue->head = taskQueue->tail = 0;
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue *taskQueue, struct User *user)
{
    pthread_mutex_lock(&taskQueue->mutex);
    taskQueue->team[taskQueue->tail] = user;
    DBG(L_GREEN"Thread Pool"NONE" : Task push %s\n", user->name);

    if (++taskQueue->tail == taskQueue->sum) {
        DBG(L_GREEN"Thread Pool"NONE" : Task Queue End\n");
        taskQueue->tail = 0;
    }
    
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct User *task_queue_pop(struct task_queue *taskQueue)
{
    pthread_mutex_lock(&taskQueue->mutex);

    while (taskQueue->tail == taskQueue->head) {
        DBG(L_GREEN"Thread Pool"NONE" : Task Queue Empty, Waiting For Task\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }

    struct User *user = taskQueue->team[taskQueue->head];
    DBG(L_GREEN"Thread Pool"NONE" : Task Pop %s\n", user->name);

    if (++taskQueue->head == taskQueue->sum) {
        DBG(L_GREEN"Thread Pool"NONE" : Task Queue End\n");
        taskQueue->head = 0;
    }

    pthread_mutex_unlock(&taskQueue->mutex);
    return user;
}

void *thread_run(void *arg)
{
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;

    while (1) {
        struct User *user = task_queue_pop(taskQueue);
        do_work(user);
    }
}
