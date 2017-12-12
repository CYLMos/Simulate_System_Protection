#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ERROR -1
#define CREATE 1
#define READ 2
#define WRITE 3
#define MODIFY 4
#define LOGIN 5
#define LOGOUT 6

int checkCommand(char*);

char* fileName;

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

    char input[256];
    while(fgets(input, 256 , stdin) != NULL){
        if(input[strlen(input) - 1] == '\n'){
            input[strlen(input) - 1] = '\0';
        }

        if(strcmp("exit", input) == 0){
            break;
        }

        int status = checkCommand(input);

        if(status == CREATE){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }

            if(recv(sockFD, receiveMessage, sizeof(receiveMessage), 0) < 0){
                perror("recv()");
            }
        }

        else if(status == READ){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);

            int numbytes;
            char buff[256];
            FILE* file = fopen(fileName, "wb");
            printf("%s\n", receiveMessage);
            if(strcmp(receiveMessage, "0") == 0){
                printf("file is 0 bytes!\n");
            }
            else if(strcmp(receiveMessage, "no file!") != 0){
                numbytes = read(sockFD, buff, sizeof(buff));
                numbytes = fwrite(buff, sizeof(char), numbytes, file);
                printf("download %d bytes\n", numbytes);
            }
            else if(strcmp(receiveMessage, "no file!") == 0){
                printf("no file\n");
            }
            fclose(file);
        }

        else if(status == WRITE){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);

            if(strcmp(receiveMessage, "no file!") != 0){
                int numbytes;
                char buff[256];
                FILE* file = fopen(fileName, "rb");
                while(!feof(file)){
                    numbytes = fread(buff, sizeof(char), sizeof(buff), file);
                    numbytes = write(sockFD, buff, numbytes);
                    printf("\rSending %d bytes",numbytes);
                    fflush(stdout);
                }
                fclose(file);
                printf("\n");
            }
            else if(strcmp(receiveMessage, "no file!") == 0){
                printf("no file\n");
            }
        }

        else if(status == MODIFY){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);
        }

        printf("Server: %s\n",receiveMessage);
        memset(input, 0, sizeof(input));
    }
    printf("close Socket\n");
    close(sockFD);
    return 0;
}

int checkCommand(char* command){
    int status = -1;
    char* s = strdup(command);
    char* first = strtok(s, " ");
    fileName = strtok(NULL, " ");
    if(fileName == NULL){
        return ERROR;
    }

    if(strcmp(first, "create") == 0){
        status = CREATE;
    }

    else if(strcmp(first, "read") == 0){
        status = READ;
    }

    else if(strcmp(first, "write") == 0){
        status = WRITE;
    }

    else if(strcmp(first, "modify") == 0){
        status = MODIFY;
    }

    else if(strcmp(first, "login")){
        status = LOGIN;
    }

    else if(strcmp(first, "logout")){
        status = LOGOUT;
    }

    else{
        status = ERROR;
    }

    return status;
}
