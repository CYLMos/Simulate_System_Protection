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

int strtokeInput(char* input){
    char *str = strtok(input, " ");
    int behavior = 0;

    if(strcmp(str, "create") == 0){
        behavior = CREATE;
        //if()
    }

    while(str != NULL){

    }
}
