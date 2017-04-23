#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#include "dcenter.h"
#include "file_warp.h"
#include "network_warp.h"
#define true  1
#define false 0
#define SERVER_ADDRESS  "10.41.228.83"
#define SERVER_PORT     4123
extern unsigned char superloop;
int sockfd = 0;

int network_send(char *buf, int size)
{
    int ret;
    ret = write(sockfd, buf, size);

    if(ret<0) {
        ret = network_init();
     
        if(ret<0) {
            superloop = false;
			lprintf("network_init fail\n");
        } 
        ret = write(sockfd, buf, size);
    }
    return ret;
}

int network_recv(char *buf, int size)
{
    return read(sockfd, buf, size);
}


int network_init(void)
{
    int ret = 0;
    struct sockaddr_in serv_addr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        lprintf("\n Error : Could not create socket \n");
        return -1;
    } 
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    
    if(inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr)<=0)
    {
        lprintf("\n inet_pton error occured\n");
        return -2;
    } 

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       lprintf("\n Error : Connect Failed \n");
       return -3;
    }
    
    dcenter_init();
    lprintf("network_init successfully.\n");
    return ret;
}

void network_destroy(void)
{
    if(sockfd) 
    {
        close(sockfd);
        sockfd = 0;
    }    
    lprintf("network_destroy successfully.\n");
}

