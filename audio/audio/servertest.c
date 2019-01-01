#include <sys/types.h>

#include <sys/socket.h>

#include <stdio.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <string.h>

#include <stdlib.h>

#include <fcntl.h>

#include <sys/shm.h>

 

#define MYPORT  8887

#define QUEUE   20

#define BUFFER_SIZE 1024

 

int main()

{

    ///定义sockfd

    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    

    ///定义sockaddr_in

    struct sockaddr_in server_sockaddr;

    server_sockaddr.sin_family = AF_INET;

    server_sockaddr.sin_port = htons(MYPORT);

    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    

    ///bind，成功返回0，出错返回-1

    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)

    {

        perror("bind");

        exit(1);

    }

    

    printf("监听%d端口\n",MYPORT);

    ///listen，成功返回0，出错返回-1

    if(listen(server_sockfd,QUEUE) == -1)

    {

        perror("listen");

        exit(1);

    }

    

    ///客户端套接字

    char buffer[BUFFER_SIZE];

    struct sockaddr_in client_addr;

    socklen_t length = sizeof(client_addr);

    

    printf("等待客户端连接\n");

    ///成功返回非负描述字，出错返回-1

    int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);

    if(conn<0)

    {

        perror("connect");

        exit(1);

    }

    printf("客户端成功连接\n");

    

    while(1)

    {

        memset(buffer,0,sizeof(buffer));

        int len = recv(conn, buffer, sizeof(buffer),0);

        //客户端发送exit或者异常结束时，退出

        if(strcmp(buffer,"exit\n")==0 || len<=0)

            break;

        printf("来自客户端数据：%s\n",buffer);

        send(conn, buffer, len, 0);

        printf("发送给客户端数据：%s\n",buffer);

    }

    close(conn);

    close(server_sockfd);

    return 0;

}