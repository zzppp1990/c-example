#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

#define MAXEVENTS 64
#define PARAM_NUM_SERVER 2
#define READ_BUF 512

/*创建和bind socket*/
static int create_and_bind(char* pcPort)
{
    struct addrinfo stHints;
    struct addrinfo *pstResult;
    struct addrinfo *pstRp;
    int iSocketFd;
    int iRet;

    memset(&stHints,0,sizeof(struct addrinfo));
    /* Return IPv4 and IPv6 */
    stHints.ai_family= AF_UNSPEC;
    /* TCP socket */
    stHints.ai_socktype= SOCK_STREAM;
    /* All interfaces */
    stHints.ai_flags= AI_PASSIVE;

    /*more info about getaddrinfo() please see:man getaddrinfo!*/
    iRet = getaddrinfo(NULL, pcPort, &stHints, &pstResult);
    if(iRet != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(iRet));
        return -1;
    }

    for(pstRp= pstResult; pstRp!= NULL; pstRp=pstRp->ai_next)
    {
        iSocketFd = socket(pstRp->ai_family, pstRp->ai_socktype, pstRp->ai_protocol);
        if(iSocketFd == -1)
            continue;

        iRet = bind(iSocketFd, pstRp->ai_addr, pstRp->ai_addrlen);
        if(iRet == 0)
        {
            /* We managed to bind successfully! */
            break;
        }
        close(iSocketFd);
    }
 
    if(pstRp == NULL)
    {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(pstResult);
    return iSocketFd;
}

/*设置非阻塞IO*/
static int make_socket_non_blocking(int iSocketFd)
{
    int iFlags;
    int iRet;
    iFlags = fcntl(iSocketFd, F_GETFL,0);

    if(iFlags == -1)
    {
        perror("fcntl");
        return -1;
    }

    iFlags |= O_NONBLOCK;
    iRet = fcntl(iSocketFd, F_SETFL, iFlags);
    
    if(iRet == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}

/*处理新的tcp连接*/
void handle_new_connection(int iSocketFd, int iEpollFd)
{
    while(1)
    {
        struct sockaddr in_addr;
        socklen_t in_len;
        struct epoll_event stEpollEvent;
        int iInConnectionFd;
        int iRet;
        char szHostBuf[NI_MAXHOST],szPortBuf[NI_MAXSERV];

        in_len = sizeof in_addr;
        iInConnectionFd = accept(iSocketFd, &in_addr, &in_len);
        if(iInConnectionFd == -1)
        {
            if((errno == EAGAIN)||
                    (errno == EWOULDBLOCK))
            {
                /* We have processed all incoming
                   connections. */
                break;
            }
            else
            {
                perror("accept");
                break;
            }
        }

        iRet =getnameinfo(&in_addr,in_len,
                szHostBuf,sizeof szHostBuf,
                szPortBuf,sizeof szPortBuf,
                NI_NUMERICHOST | NI_NUMERICSERV);
        if(iRet == 0)
        {
            printf("Accepted connection on descriptor %d "
                    "(host=%s, port=%s)\n",iInConnectionFd, szHostBuf, szPortBuf);
        }

        /* Make the incoming socket non-blocking and add it to the
           list of fds to monitor. */
        iRet = make_socket_non_blocking( iInConnectionFd);
        if(iRet == -1)
            abort();

        stEpollEvent.data.fd = iInConnectionFd;
        stEpollEvent.events = EPOLLIN | EPOLLET;
        iRet = epoll_ctl(iEpollFd, EPOLL_CTL_ADD, iInConnectionFd, &stEpollEvent);
        if(iRet == -1)
        {
            perror("epoll_ctl");
            abort();
        }
    }
}

/*处理tcp连接收到的数据*/
void handle_recv_data(int iConnectionFd)
{
    int iDone =0;
    int iRet;
    
    while(1)
    {
        ssize_t ulCount;
        char szBuf[READ_BUF];
        ulCount = read(iConnectionFd, szBuf, sizeof szBuf);
        if(ulCount == -1)
        {
            /* If errno == EAGAIN, that means we have read all
               data. So go back to the main loop. */
            if(errno != EAGAIN)
            {
                perror("read");
                iDone = 1;
            }
            break;
        }
        else if(ulCount == 0)
        {
            /* End of file. The remote has closed the
               connection. */
            iDone = 1;
            break;
        }
        /* Write the buffer to standard output */
        iRet = write( 1, szBuf, ulCount);
        if(iRet == -1)
        {
            perror("write");
            abort();
        }
    }
    if(iDone)
    {
        printf("Closed connection on descriptor %d\n", iConnectionFd);
        /* Closing the descriptor will make epoll remove it
           from the set of descriptors which are monitored. */
        close(iConnectionFd);
    }
}

int main(int argc,char* argv[])
{
    int iRet;
    int iSocketFd;
    int iEpollFd;
    struct epoll_event stEpollEvent;
    struct epoll_event* pstEpollEvents;

    if(argc != PARAM_NUM_SERVER)
    {
        fprintf(stderr,"Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    iSocketFd = create_and_bind(argv[1]);
    if( iSocketFd == -1)
        abort();

    iRet = make_socket_non_blocking(iSocketFd);
    if(iRet == -1)
        abort();

    iRet = listen(iSocketFd, SOMAXCONN);
    if(iRet == -1)
    {
        perror("listen");
        abort();
    }

    iEpollFd = epoll_create1(0);
    if(iEpollFd == -1)
    {
        perror("epoll_create");
        abort();
    }

    stEpollEvent.data.fd = iSocketFd;
    stEpollEvent.events = EPOLLIN | EPOLLET;
    iRet = epoll_ctl(iEpollFd, EPOLL_CTL_ADD, iSocketFd, &stEpollEvent);
    if(iRet == -1)
    {
        perror("epoll_ctl");
        abort();
    }

    /* Buffer where events are returned */
    pstEpollEvents = calloc(MAXEVENTS, sizeof stEpollEvent);

    /* The event loop */
    while(1)
    {
        int iEventNum,i;
        iEventNum = epoll_wait(iEpollFd, pstEpollEvents, MAXEVENTS, -1);
        for(i = 0; i < iEventNum; i++)
        {
            if( (pstEpollEvents[i].events & EPOLLERR) ||
                    (pstEpollEvents[i].events & EPOLLHUP) ||
                    (!(pstEpollEvents[i].events & EPOLLIN)) )
            {
                /* An error has occured on this fd, or the socket is not
                   ready for reading (why were we notified then?) */
                fprintf(stderr,"epoll error\n");
                close(pstEpollEvents[i].data.fd);
                continue;
            }
            else if(iSocketFd == pstEpollEvents[i].data.fd)
            {
            /* We have a notification on the listening socket, which
               means one or more incoming connections. */
                handle_new_connection(iSocketFd, iEpollFd);
            }
            else
            {
                /* We have data on the fd waiting to be read. Read and
                   display it. We must read whatever data is available
                   completely, as we are running in edge-triggered mode
                   and won't get a notification again for the same
                   data. */
                handle_recv_data(pstEpollEvents[i].data.fd);
            }
        }
    }
    free(pstEpollEvents);
    close(iSocketFd);
    return EXIT_SUCCESS;
}

