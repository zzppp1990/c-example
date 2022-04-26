#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#define DEFAULT_IP "192.168.56.101"
#define DEFAULT_PORT 6666
#define PARAM_NUM 3
#define IPBUFMAXLEN 64

void client_set_DestIp(char* pcSrcIp, char* pcDstIp)
{
    if(strlen(pcSrcIp) > IPBUFMAXLEN-1)
    {
        strcpy(pcDstIp, DEFAULT_IP);
        return;
    }
    strcpy(pcDstIp, pcSrcIp);
}

int main(int argc, char* argv[])
{
    int iSockfd;
    struct sockaddr_in dest_addr;
    char szDestIp[IPBUFMAXLEN] = {0};
    int  iDestPort = DEFAULT_PORT;

    if(argc != PARAM_NUM)
    {
        fprintf(stderr,"Usage: %s [DEST_IP] [DEST_PORT]\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    client_set_DestIp(argv[1], szDestIp);
    iDestPort = atoi(argv[2]);

    iSockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(iSockfd >= 0);

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(iDestPort);
    dest_addr.sin_addr.s_addr = inet_addr(szDestIp);
    bzero(&(dest_addr.sin_zero),sizeof(dest_addr.sin_zero) );

    int ret = connect(iSockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
    if(0 == ret)
    {
      printf("connect success.\n");
    }
    else
    {
      printf("connect failed. \n");
    }

    /*测试TCP传输*/
    write(iSockfd, "hello\n", 7);
    
    close(iSockfd);
    return 0;
}

