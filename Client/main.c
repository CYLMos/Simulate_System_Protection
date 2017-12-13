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
#define SHOW 7

int checkCommand(char*);

char* fileName;
int loginFlag = 0;

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
    printf(">");
    while(fgets(input, 256 , stdin) != NULL){
        if(input[strlen(input) - 1] == '\n'){
            input[strlen(input) - 1] = '\0';
        }

        if(strcmp("exit", input) == 0){
            break;
        }

        int status = checkCommand(input);

        if(status == CREATE && loginFlag == 1){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }

            if(recv(sockFD, receiveMessage, sizeof(receiveMessage), 0) < 0){
                perror("recv()");
            }
        }

        else if(status == READ && loginFlag == 1){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);

            int numbytes;
            char buff[256];
            memset(buff, 0, sizeof(buff));
            FILE* file = fopen(fileName, "wb");
            if(strcmp(receiveMessage, "no file!") != 0){;
                numbytes = read(sockFD, buff, sizeof(buff));
                if(strcmp(receiveMessage, "0") == 0){
                    printf("file is 0 bytes!\n");
                }

                if(strcmp(buff, "00") != 0){
                    numbytes = fwrite(buff, sizeof(char), numbytes, file);
                    printf("download %d bytes\n", numbytes);
                }
            }
            /*else if(strcmp(receiveMessage, "no file!") == 0){
                printf("no file\n");
            }*/
            fclose(file);
        }

        else if(status == WRITE && loginFlag == 1){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);

            if(strcmp(receiveMessage, "no file!") != 0){
                int numbytes;
                int sendFlag = 0;
                char buff[256];
                int flag = 0;
                FILE* file = fopen(fileName, "rb");
                while(1){
                    if(!feof(file)){
                        numbytes = fread(buff, sizeof(char), sizeof(buff), file);
                        if(numbytes == 0 && sendFlag == 0){
                            send(sockFD, "0", 1, 0);
                            sendFlag = 1;
                            flag = 1;
                        }
                        else{
                            if(sendFlag == 0){send(sockFD, "has file", 8, 0); sendFlag == 1;}

                            numbytes = write(sockFD, buff, numbytes);
                            printf("\rSending %d bytes",numbytes);
                            fflush(stdout);
                        }
                    }
                    else{
                        sleep(1);
                        if(flag == 1){write(sockFD, "00", 2);}
                        break;
                    }
                }

                fclose(file);
                printf("\n");
            }
            /*else if(strcmp(receiveMessage, "no file!") == 0){
                printf("no file\n");
            }*/
        }

        else if(status == MODIFY && loginFlag == 1){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);
        }

        else if(status == LOGIN && loginFlag == 0){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);
            if(strcmp(receiveMessage, "login success!") == 0){loginFlag = 1;}
        }

        else if(status == LOGOUT && loginFlag == 1){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            recv(sockFD, receiveMessage, sizeof(receiveMessage), 0);
            printf("Server: %s\n",receiveMessage);
            loginFlag = 0;
            break;
        }
        else if(status == SHOW && loginFlag == 1){
            if(send(sockFD, input, sizeof(input), 0) < 0){
                perror("send()");
            }
            memset(receiveMessage, 0, sizeof(receiveMessage));
            while(1){
                if(recv(sockFD, receiveMessage, sizeof(receiveMessage), 0) < 0){
                    perror("recv()");
                }
                if(strcmp(receiveMessage, "00") != 0){
                    printf("%s", receiveMessage);
                    memset(receiveMessage, 0, sizeof(receiveMessage));
                }
                else{
                    memcpy(receiveMessage, "done!\n", 6);
                    break;
                }
            }
            printf("\n");
        }
        else{
            if(loginFlag == 0){
                printf("You must login first!\n");
            }
            else{
                printf("make sure your input is right!\n");
            }
        }

        printf("Server: %s\n",receiveMessage);
        memset(input, 0, sizeof(input));
        memset(receiveMessage, 0, sizeof(receiveMessage));

        printf(">");
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
        if(strncmp(command, "show", 4) == 0){
            return SHOW;
        }
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

    else if(strcmp(first, "login") == 0){
        status = LOGIN;
    }

    else if(strcmp(first, "logout") == 0){
        status = LOGOUT;
    }

    else{
        status = ERROR;
    }

    return status;
}
