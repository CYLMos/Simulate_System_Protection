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
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "login")){
        behavior = LOGIN;
    }

    else if(strcmp(str, "logout")){
        behavior = LOGOUT;
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

            struct user* u;
            int noUser = 1;
            while((u = root->next) != NULL){
                if(u->id == id){
                    noUser = 0;
                    fprintf(capList, "%s %s %s %s\n", others[0], u->name, u->group, others[1]);
                    message = "create success!";
                    send(id, "create success!", 15, 0);
                    break;
                }
            }
            if(noUser == 1){
                fprintf(capList, "%s %s %s %s\n", others[0], "test", "test", others[1]);
                message = "create success!";
                send(id, "create success!", 15, 0);
            }
        }
    }

    else if(behavior == READ){
        extern id;
        //int rights[RIGHT_LEN];
        struct user* u;
        while((u = root->next) != NULL){
            if(u->id = id){
                break;
            }
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
                /*if(strcmp(u->name, userName) == 0){
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
                }*/
                canAccess = 1;

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
                                //write(id, "EoF", 3);
                            }
                            else{
                                if(sendFlag == 0){send(id, "has file", 8, 0);}
		                        numbytes = write(id, buff, numbytes);
		                        printf("\rSending %d bytes",numbytes);
		                        fflush(stdout);
		                    }
		                }
		                else{
		                    //write(id, "EoF", 3);
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

    }

    else if(behavior == WRITE){
        extern id;
        struct user* u;
        while((u = root->next) != NULL){
            if(u->id = id){
                break;
            }
        }

        char* readLine;
        char* fileName;
        ssize_t len = 0;
        int flag = 0;
        int canAccess = 0;
        while(getline(&readLine, &len, capList) != -1){
            fileName = strtok(readLine, " ");
            printf("%s\n", fileName);
            printf("%s\n", others[0]);
            if(strcmp(others[0], fileName) == 0){
                flag = 1;

                char* userName = strtok(NULL, " ");
                char* groupName = strtok(NULL, " ");
                char* rights = strtok(NULL, " ");
                //printf("%s\n", rights);
                /*if(strcmp(u->name, userName) == 0){
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
                }*/
                canAccess = 1;

                if(canAccess == 1){
                    send(id, "has file!", 9, 0);
                    int numbytes;
                    char buff[256];
                    FILE* file;
                    if(strcmp(others[1], "o") == 0){
                        file = fopen(fileName, "wb");
                    }
                    else if(strcmp(others[1], "a") == 0){
                        file = fopen(fileName, "ab");
                    }

                    numbytes = read(id, buff, sizeof(buff));
		            numbytes = fwrite(buff, sizeof(char), numbytes, file);
		            printf("download %d bytes\n", numbytes);

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
    }

    else if(behavior == MODIFY){
        extern id;
        //int rights[RIGHT_LEN];
        struct user* u;
        while((u = root->next) != NULL){
            if(u->id = id){
                break;
            }
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
                if(strcmp(userName, "123") == 0){
                    status = 1;
                    fprintf(capWrite, "%s %s %s %s\n", others[0], u->name, u->group, others[1]);
                    message = "write success!";
                }
                else if(u == NULL){
                    status = 1;
                    fprintf(capWrite, "%s %s %s %s\n", others[0], "test", "test", others[1]);
                    message = "write success!";
                }
                else{
                    status = -1;
                    message = "You can't change!";
                    system("rm target");
                    break;
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

        fclose(capWrite);

    }

    else if(behavior == LOGIN){

    }

    else if(behavior == LOGOUT){

    }

    else{

    }

    fclose(capList);

    return message;

}
