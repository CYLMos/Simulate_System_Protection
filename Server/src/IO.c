#include "IO.h"

int init(){
    root = (struct user *)malloc(sizeof(struct user));
    root->name = "root";
    root->group = "root";
    root->id = -1;
    root->next = NULL;

    char* fileName[] = {"capability_lists", "group", "user"};
    for(int i = 0; i < 3; i++){
        int status = initFile(fileName[i]);
        if(status == FILE_ERROR){
            exit(0);
        }

        else if(status == FILE_CREATE){
            if(i == 1){
                char* groupName[] = {"AOS-students\n", "CSE-students"};
                FILE* file = fopen("group", "w");

                for(int j = 0; j < 2; j++){
                    if(file != NULL){
                        fprintf(file, groupName[j]);
                    }
                }
                fclose(file);
            }

            else if(i == 2){
                char* userName[] = {" Mos\n", " CYL\n", " Sank\n", " Ken\n", " Barbie\n", " Harry\n"};
                FILE* file = fopen("user", "w");
                for(int j = 0; j < 6; j++){
                    char group[25] = "AOS-students";
                    if(j >= 3){
                        memset(group, 0, sizeof(group));
                        memcpy(group, "CSE-students", 13);
                    }

                    strcat(group, userName[j]);

                    fprintf(file, group);
                }
                fclose(file);
            }
        }
    }
}

int initFile(char* fileName){
    FILE* file = fopen(fileName, "rb+");
    int status = FILE_EXIST;
    if(file == NULL){
        file = fopen(fileName, "wb+");
        if(file == NULL){
            perror("fopen()");
            status = FILE_ERROR;
        }
        else{
            status = FILE_CREATE;
        }
    }

    fclose(file);

    return status;
}

int strtokInput(char* input, char* others[]){
    char* str = strtok(input, " ");
    int behavior = 0;

    if(strcmp(str, "create") == 0){
        behavior = CREATE;

        if((str = strtok(NULL, " ")) != NULL){
            char* rights = strtok(NULL, " ");
            if(rights == NULL){return ERROR;}

            others[0] = str;
            others[1] = rights;
            if(rightCheck(rights) != 1){
                behavior = ERROR;
            }
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "read") == 0){
        behavior = READ;

        if((str = strtok(NULL, " ")) != NULL){
            others[0] = str;
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "write") == 0){
        behavior = WRITE;

        if((str = strtok(NULL, " ")) != NULL){
            char* oa = strtok(NULL, " ");
            if(oa == NULL){return ERROR;}

            others[0] = str;
            others[1] = oa;

            /*if( !(strcmp(oa, "o") == 0 || strcmp(oa, "a") == 0) ){
                behavior = ERROR;
            }*/
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "modify") == 0){
        behavior = MODIFY;

        if((str = strtok(NULL, " ")) != NULL){
            char* rights = strtok(NULL, " ");
            if(rights == NULL){return ERROR;}

            others[0] = str;
            others[1] = rights;

            if(rightCheck(rights) != 1){
                behavior = ERROR;
            }

        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "login") == 0){
        behavior = LOGIN;

        if((str = strtok(NULL, " ")) != NULL){
            others[0] = str;
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "logout") == 0){
        behavior = LOGOUT;

        if((str = strtok(NULL, " ")) != NULL){
            others[0] = str;
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "show") == 0){
        behavior = SHOW;
    }

    else{
        behavior = ERROR;
    }

    return behavior;
}

char* doCommandLine(int behavior, char* others[]){
    FILE* capList = fopen("capability_lists", "ra");
    char* message;
    if(behavior == CREATE){
        char* read;
        char* fileName;
        ssize_t len = 0;
        int flag = 0;
        while(getline(&read, &len, capList) != -1){
            fileName = strtok(read, " ");
            if(strcmp(others[0], fileName) == 0){
                flag = 1;
                message = "This file has been exist!";
                break;
            }
        }

        if(flag == 0){
            fclose(capList);
            capList = fopen("capability_lists", "a");
            if(capList == NULL){
                perror("fopen()");
            }
            char command[50] = "touch ";
            strcat(command, others[0]);
            system(command);

            struct user* u = root->next;
            int noUser = 1;
            while(u != NULL){
                noUser = 0;
                if(u->id == id){
                    fprintf(capList, "%s %s %s %s\n", others[0], u->name, u->group, others[1]);
                    message = "create success!";
                    send(id, "create success!", 15, 0);
                    break;
                }
                u = u->next;
            }
            if(noUser == 1){
                //fprintf(capList, "%s %s %s %s\n", others[0], "test", "test", others[1]);
                message = "no user";
                send(id, "This file has been exist!", 25, 0);
            }
        }
        else{
            send(id, "This file has been exist!", 7, 0);
        }
    }

    else if(behavior == READ){

        struct user* u = root->next;
        while(u != NULL){
            if(u->id == id){
                break;
            }
            u = u->next;
        }

        char* readLine;
        char* fileName;
        ssize_t len = 0;
        int flag = 0;
        int canAccess = 0;
        while(getline(&readLine, &len, capList) != -1){
            fileName = strtok(readLine, " ");
            if(strcmp(others[0], fileName) == 0){
                flag = 1;

                char* userName = strtok(NULL, " ");
                char* groupName = strtok(NULL, " ");
                char* rights = strtok(NULL, " ");
                //printf("%s %d\n", userName, strlen(userName));
                if(strcmp(u->name, userName) == 0){
                    if(rights[OWNER_READ] == 'r'){
                        canAccess = 1;
                    }
                }
                else if(strcmp(u->group, groupName) == 0){
                    if(rights[GROUP_READ] == 'r'){
                        canAccess = 1;
                    }
                }
                else{
                    if(rights[OTHER_READ] == 'r'){
                        canAccess = 1;
                    }
                }
                //canAccess = 1;

                if(canAccess == 1){
                    FILE* file = fopen(fileName, "rb");
                    char buff[256];
                    int numbytes = 0;
                    int sendFlag = 0;
                    while(1){
                        if(!feof(file)){
                            numbytes = fread(buff, sizeof(char), sizeof(buff), file);
                            if(numbytes == 0 && sendFlag == 0){
                                send(id, "0", 1, 0);
                                sendFlag = 1;
                            }
                            else{
                                if(sendFlag == 0){send(id, "has file", 8, 0); sendFlag == 1;}
		                        numbytes = write(id, buff, numbytes);
		                        printf("\rSending %d bytes",numbytes);
		                        fflush(stdout);
		                    }
		                }
		                else{
		                    sleep(1);
		                    write(id, "00", 2);
		                    break;
		                }
                    }
                    printf("\n");

	                fclose(file);
	                message = "success to read!";
                }
                else{
                    message = "You can not read this file!";
                    send(id, "no file!", 8, 0);
                }
                break;
            }
        }

        if(flag == 0){
            message = "This file dosen't exist!";
            send(id, "no file!", 8, 0);
        }

        u = NULL;

    }

    else if(behavior == WRITE){
        struct user* u = root->next;
        while(u != NULL){
            if(u->id = id){
                break;
            }
            u = u->next;
        }

        char* readLine;
        char* fileName;
        ssize_t len = 0;
        int flag = 0;
        int canAccess = 0;
        while(getline(&readLine, &len, capList) != -1){
            fileName = strtok(readLine, " ");
            if(strcmp(others[0], fileName) == 0){
                flag = 1;

                char* userName = strtok(NULL, " ");
                char* groupName = strtok(NULL, " ");
                char* rights = strtok(NULL, " ");
                //printf("%s\n", rights);
                if(strcmp(u->name, userName) == 0){
                    if(rights[OWNER_WRITE] == 'w'){
                        canAccess = 1;
                    }
                }
                else if(strcmp(u->group, groupName) == 0){
                    if(rights[GROUP_WRITE] == 'w'){
                        canAccess = 1;
                    }
                }
                else{
                    if(rights[OTHER_WRITE] == 'w'){
                        canAccess = 1;
                    }
                }
                //canAccess = 1;

                if(canAccess == 1){
                    send(id, "has file!", 9, 0);
                    //char buff[256];
                    char recvBuff[256];

                    memset(recvBuff, 0, sizeof(recvBuff));

                    recv(id, recvBuff, sizeof(recvBuff), 0);
                    int numbytes;
                    FILE* file;

                    if(strcmp(others[1], "o") == 0){
                        file = fopen(fileName, "w");
                    }
                    else if(strcmp(others[1], "a") == 0){
                        file = fopen(fileName, "a");
                    }

                    extern char inputBuffer[256];
                    memset(inputBuffer, 0, sizeof(0));
                    numbytes = read(id, inputBuffer, sizeof(inputBuffer));

                    if(strcmp(recvBuff, "0") == 0){
                        printf("file is 0 bytes!\n");
                        //memset(recvBuff, 0, sizeof(recvBuff));
                    }

                    if(strcmp(inputBuffer, "00") != 0){
                        numbytes = fwrite(inputBuffer, sizeof(char), numbytes, file);
                        printf("download %d bytes\n", numbytes);
                    }

                    memset(inputBuffer, 0, sizeof(inputBuffer));

                    fclose(file);

	                message = "success to write!";
                }
                else{
                    message = "You can not write this file!";
                    send(id, "no file!", 8, 0);
                }
                break;
            }
        }

        if(flag == 0){
            message = "This file dosen't exist!";
            send(id, "no file!", 8, 0);
        }

        u = NULL;
    }

    else if(behavior == MODIFY){
        struct user* u = root->next;

        while(u != NULL){
            if(u->id == id){
                break;
            }
            u = u->next;
        }

        char* readLine;
        char* fileName;
        ssize_t len = 0;
        int flag = 0;
        int canAccess = 0;
        FILE* capWrite = fopen("target", "a");
        int lineLen;
        int status = 0;


        while((lineLen = getline(&readLine, &len, capList)) != -1){
            char* s = strdup(readLine);
            fileName = strtok(s, " ");

            if(strcmp(others[0], fileName) == 0){
                flag = 1;

                char* userName = strtok(NULL, " ");

                if(u != NULL){
                    if(strcmp(userName, u->name) == 0){
                        status = 1;
                        fprintf(capWrite, "%s %s %s %s\n", others[0], u->name, u->group, others[1]);
                        message = "write success!";
                    }
                    else{
                        status = -1;
                        message = "You can't change!";
                        system("rm target");
                        break;
                    }
                }
            }
            else{
                fprintf(capWrite, "%s", readLine);
            }
        }

        if(flag != 0){
            if(status == -1){send(id, "You can't change!", 17, 0);}
            else if(status == 1){
                system("rm capability_lists");
                system("mv target capability_lists");
                send(id, "Modify success!", 15, 0);
            }
        }
        else{
            message = "This file dosen't exist!";
            send(id, "no file!", 8, 0);
        }

        u = NULL;

        fclose(capWrite);

    }

    else if(behavior == LOGIN){
        struct user* u;
        struct user* pre;
        int hasLogin = 0;
        int hasUser = 0;
        int loginSuccess = 0;
        u = root->next;
        pre = root;
        while(u != NULL){
            hasUser = 1;
            if(strcmp(others[0], u->name) == 0){
                message = "has been login!";
                hasLogin = 1;
                break;
            }
            pre = u;
            u = u->next;
        }

        if(hasLogin == 0){
            FILE* file = fopen("user", "r");
            char* readLine;
            ssize_t len = 0;
            int lineLen;

            while((lineLen = getline(&readLine, &len, file)) != -1){
                char* group = strtok(readLine, " ");
                char* name = strtok(NULL, " ");
                /*printf("1. %s %d\n", name, strlen(name));
                printf("2. %s %d\n", others[0], strlen(others[0]));*/

                if(strncmp(name, others[0], strlen(others[0])) == 0 && name[strlen(name) - 1] == '\n'){
                    struct user* newUser = (struct user *)malloc(sizeof(struct user));
                    newUser->group = group;
                    newUser->name = strtok(name, "\n");
                    newUser->id = id;
                    newUser->next = NULL;

                    pre->next = newUser;
                    newUser = NULL;
                    loginSuccess = 1;

                    message = "login success!";
                    send(id, "login success!", 14, 0);

                    break;
                }
            }
        }
        else{
            send(id, "has been login!", 15, 0);
        }

        if(hasLogin == 0){
            if(loginSuccess == 0){
                message = "this user dosen't exist!";
                send(id, "this user dosen't exist!", 24, 0);
            }
        }

        u = NULL;
        pre = NULL;
    }

    else if(behavior == LOGOUT){
        struct user* u;
        struct user* pre;
        struct user* next;
        int hasLogin = 1;
        pre = root;
        u = root->next;
        while(u != NULL){
            next = u->next;

            if(strcmp(others[0], u->name) == 0){
                pre->next = next;
                u->next = NULL;
                free(u);

                message = "logout success!";
                send(id, "logout success!", 15, 0);
                hasLogin = 0;
                break;
            }

            pre = u;
            u = next;
        }

        if(hasLogin == 1){
            message = "can not find this user!";
            send(id, "can not find this user!", 23, 0);
        }

        u = NULL;
        pre = NULL;
        next = NULL;
    }
    else if(behavior == SHOW){
        FILE* file = fopen("capability_lists", "r");
        char* read;
        ssize_t len = 0;
        while(getline(&read, &len, capList) != -1){
            send(id, read, strlen(read), 0);
        }
        sleep(1);
        send(id, "00", 2, 0);
    }

    else{
        message = "ERROR!";
        send(id, "ERROR!", 6, 0);
    }

    fclose(capList);

    return message;

}

int rightCheck(char* rights){
    if(strlen(rights) != RIGHT_LEN){
        return 0;
    }
    for(int i = 0; i < RIGHT_LEN; i++){
        char c = i % 2 == 0 ? 'r' : 'w';
        if(rights[i] != c && rights[i] != '-'){
            return 0;
        }
    }
    return 1;
}
