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

#define BUFFER_SIZE 1024

char* SERVER_IP = "192.168.1.103";

 

int main()

{

    ///定义sockfd

    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);

    

    ///定义sockaddr_in

    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    servaddr.sin_port = htons(MYPORT);  ///服务器端口

    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  ///服务器ip

    

    printf("连接%s:%d\n",SERVER_IP,MYPORT);

    ///连接服务器，成功返回0，错误返回-1

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)

    {

        perror("connect");

        exit(1);

    }

    printf("服务器连接成功\n");

    char sendbuf[BUFFER_SIZE];

    char recvbuf[BUFFER_SIZE];

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)

    {

        printf("向服务器发送数据：%s\n",sendbuf);

        send(sock_cli, sendbuf, strlen(sendbuf),0); ///发送

        if(strcmp(sendbuf,"exit\n")==0)

            break;

        recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收

        printf("从服务器接收数据：%s\n",recvbuf);

        

        memset(sendbuf, 0, sizeof(sendbuf));

        memset(recvbuf, 0, sizeof(recvbuf));

    }

    

    close(sock_cli);

    return 0;

}