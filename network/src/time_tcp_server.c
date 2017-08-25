#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <time.h>

void sig_handler(int signo);
void out_addr(struct sockaddr_in * clientaddr);
void do_service(int fd);


int sockfd;


int main(int argc,char **argv)
{
    if(argc < 2)
    {
        printf("usage: %s #port\n",argv[0]);
        exit(1);
    }
    if(signal(SIGINT,sig_handler) == SIG_ERR) 
    {
        perror("signal sight error");
        exit(1);
    }
    //step 1, to create a socket 
    //ps: socket is created in the kernel ,it is a struct 
    //AF_INET:IPV4
    //sock_stream:tcp protocol 
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        perror("socket error");
        exit(1);
    }
    //step 2,to bind() a socket with a  address(ip,port)
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;//IPv4
    serveraddr.sin_port = htons(atoi(argv[1]));//port
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
    {
        perror("bind error");
        exit(1);
    }
    //step 3,to start a listen()(specific port)
    //inform the system to accept the request of the client
    //to insert the request from client in the queue
    //the second param is the queue's size is backlog 
    if(listen(sockfd,10) < 0)
    {
        perror("listen error");
        exit(1);
    }
    //step4,to start a accept() reciving requests from the client 
    //  return a new socket fd
    //  caution without the connection to client the function would be stuck
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    while(1)
    {
        int fd = accept(sockfd,(struct sockaddr*)&clientaddr,&clientaddr_len);
        if(fd < 0)
        {
            perror("accept error");
            continue;
        }
        //step 5,to start the io functions (read/write)
        //      to connect the client with alternative 
        out_addr(&clientaddr);
        do_service(fd);
        //close the socket that bind to a certain client
        close(fd);
    }

    return 0;
}



void sig_handler(int signo)
{
    if(signo == SIGINT)
    {
        printf("\n");
        printf("server close\n");
        //step 6,to close the socket create by server
        close(sockfd);
        exit(1);
    }
}

void out_addr(struct sockaddr_in *clientaddr)
{
    //the  port was encapsulate in web code style 
    int port = ntohs(clientaddr->sin_port);
    char ip[16];
    memset(ip,0,sizeof(ip));
    //to transact ip from web code style to host code 
    inet_ntop(AF_INET,&clientaddr->sin_addr.s_addr,ip,sizeof(ip));
    printf("client : %s(%d) connected\n",ip,port);
}


void do_service(int fd)
{
    //to abtain the system's time
    long t = time(0);
    char *s = ctime(&t);
    size_t size = strlen(s) * sizeof(char);
    //to put it into the client 
    if(write(fd,s,size) != size)    
    {
        perror("write error");
    }

    
        

}
