#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc , char *argv[])
{
    int sockFD = 0;
    sockFD = socket(AF_INET , SOCK_STREAM , 0);

    if (sockFD == -1){
        printf("Fail to create a socket.");
    }

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8080);


    if(connect(sockFD,(struct sockaddr *)&info,sizeof(info)) == -1){
        perror("connect()");
    }


    char receiveMessage[100] = {};
    memset(receiveMessage, 0, sizeof(receiveMessage));

    char* input;
    while(scanf("%s", input) == 1){
        if(strcmp("exit", input) == 0){
            break;
        }

        if(send(sockFD, input, sizeof(input), 0) < 0){
            perror("send()");
        }

        if(recv(sockFD, receiveMessage, sizeof(receiveMessage), 0) < 0){
            perror("recv()");
        }

        printf("Server: %s\n",receiveMessage);
    }
    printf("close Socket\n");
    close(sockFD);
    return 0;
}
