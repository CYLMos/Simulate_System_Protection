#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "IO.h"

char inputBuffer[256];

int main()
{
    init();

    extern struct user* root;

    int sockFD = 0, clientSockFD= 0;
    fd_set masterFD;
    fd_set readFD;
    int fdMax;

    memset(inputBuffer, 0, sizeof(inputBuffer));
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

    int option = 1;
    setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // bind port
    if(bind(sockFD, (struct sockaddr *)&serverInfo, sizeof(serverInfo))){
        perror("bind()");
    }
    // listen
    if(listen(sockFD, 10)){
        perror("listen()");
    }

    // add fd to master
    FD_SET(sockFD, &masterFD);
    fdMax = sockFD;

    while(1){
        readFD = masterFD;
        extern int id;

        if(select(fdMax + 1, &readFD, NULL, NULL, NULL) == -1){
            perror("select()");
            exit(0);
        }

        for(id = 0; id <= fdMax; id++){
            if(FD_ISSET(id, &readFD)){
                if(id == sockFD){
                    if((clientSockFD = accept(sockFD, (struct sockaddr *)&clientInfo, &addrLen)) < 0){
                        perror("accept()");
                    }

                    FD_SET(clientSockFD, &masterFD);
                    // update fdMax
                    if(clientSockFD > fdMax){
                        fdMax = clientSockFD;
                    }

                    printf("Server: new connection\n");
                }
                else{
                    int recvStatus = recv(id, inputBuffer, sizeof(inputBuffer), 0);

                    // if error, close fd
                    if(recvStatus <= 0){
                        if(recvStatus < 0){perror("recv()");}
                        else if(recvStatus = 0){printf("colse connected");}

                        close(id);
                        FD_CLR(id, &masterFD);
                    }
                    else{

                        printf("client: %s\n", inputBuffer);

                        char* others[2]; //param

                        // check behavior
                        int behavior = strtokInput(inputBuffer, others);

                        //work and get message
                        char* message = doCommandLine(behavior, others);
                        printf("%s\n", message);

                        memset(inputBuffer, 0, sizeof(inputBuffer));

                        //send(id, message, strlen(message), 0);
                    }
                }
            }
        }
    }


    return 0;
}
