#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <IO.h>

int main()
{
    init();

    int sockFD = 0, clientSockFD= 0;
    char inputBuffer[256] = {};
    char outputBuffer[256] = {};
    fd_set masterFD;
    fd_set readFD;
    int fdMax;

    // create socket
    sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if(sockFD == -1){
        perror("socket()");
        exit(0);
    }

    struct sockaddr_in serverInfo, clientInfo;
    int addrLen = sizeof(clientInfo);
    bzero(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(8080);

    // bind port
    if(bind(sockFD, (struct sockaddr *)&serverInfo, sizeof(serverInfo))){
        perror("bind()");
    }
    // listen
    if(listen(sockFD, 10)){
        perror("listen()");
    }

    FD_SET(sockFD, &masterFD);
    fdMax = sockFD;

    while(1){
        readFD = masterFD;

        if(select(fdMax + 1, &readFD, NULL, NULL, NULL) == -1){
            perror("select()");
            exit(0);
        }

        for(int i = 0; i <= fdMax; i++){
            if(FD_ISSET(i, &readFD)){
                if(i == sockFD){
                    if((clientSockFD = accept(sockFD, (struct sockaddr *)&clientInfo, &addrLen)) < 0){
                        perror("accept()");
                    }

                    FD_SET(clientSockFD, &masterFD);
                    if(clientSockFD > fdMax){
                        fdMax = clientSockFD;
                    }

                    printf("selectserver: new connection\n");
                }
                else{
                    int recvStatus = recv(i, inputBuffer, sizeof(inputBuffer), 0);
                    if(recvStatus <= 0){
                        if(recvStatus < 0){perror("recv()");}
                        else if(recvStatus = 0){printf("colse connected");}

                        close(i);
                        FD_CLR(i, &masterFD);
                    }
                    else{
                        printf("client: %s\n", inputBuffer);
                        printf("%d\n", recvStatus);
                        char* others[2];
                        int behavior = strtokInput(inputBuffer, others);
                        printf("%d\n", behavior);
                        send(i, inputBuffer, recvStatus, 0);
                    }
                }
            }
        }
    }


    return 0;
}
