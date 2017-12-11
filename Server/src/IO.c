#include "IO.h"

int init(){
    char* fileName[] = {"capability lists", "group", "user"};
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
    //FILE* capList = fopen("capability lists", "ra");

    if(strcmp(str, "create") == 0){
        behavior = CREATE;

        if((str = strtok(NULL, " ")) != NULL){
            printf("%s\n", str);
            char* rights = strtok(NULL, " ");
            printf("%s\n", rights);
            if(rights == NULL){return ERROR;}

            others[0] = str;
            others[1] = rights;

            printf("%s\n", others[0]);
            printf("%s\n", others[1]);

            /*char* read;
            ssize_t len = 0;
            int flag = 0;
            while(getline(&read, &len, capList) != -1){
                if(strcmp(str, read) == 0){
                    flag = 1;
                    printf("This file has been exist!\n");
                    return ERROR;
                }
            }

            if(flag == 0){
                char command[50] = "touch ";
                strcat(command, str);
                system(command);
            }*/
        }
        else{
            behavior = ERROR;
        }
    }

    else if(strcmp(str, "read") == 0){
    }

    else if(strcmp(str, "write") == 0){
    }

    else if(strcmp(str, "modify") == 0){
    }

    else{
        behavior = ERROR;
    }

    return behavior;
}
