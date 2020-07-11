/*************************************************************************
	> File Name: udp_client.c
	> Author:Louis Tang 
	> Mail:1328147875@qq.com 
	> Created Time: Thu 09 Jul 2020 01:31:50 PM CST
 ************************************************************************/

#include "head.h"

int socket_udp() 
{
    int fd;
        
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return fd;
}

