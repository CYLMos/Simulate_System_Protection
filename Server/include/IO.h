#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ERROR -1  //error state
#define CREATE 1  //create state
#define READ 2  //read state
#define WRITE 3  //write state
#define MODIFY 4  //modify state
#define LOGIN 5  //login state
#define LOGOUT 6  //logout state
#define SHOW 7  //show state

#define FILE_CREATE 0  //file create
#define FILE_EXIST 1  //file exist
#define FILE_ERROR -1  //file error

// rights address
#define OWNER_READ 0
#define OWNER_WRITE 1
#define GROUP_READ 2
#define GROUP_WRITE 3
#define OTHER_READ 4
#define OTHER_WRITE 5

#define RIGHT_LEN 6  //rights length

//user structut
typedef struct user{
    char* name;
    char* group;
    int id;
    struct user* next;
} ;

struct user* root; //linked-list header is root

int id;  //socket id


void init(); // init

int initFile(char* fileName); // init file if they are not exist.

int strtokInput(char* input, char* others[]);  //check the command state and spilit it

char* doCommandLine(int behavior, char* others[]);  //do the corresponding behavior with command

int rightCheck(char* rights);  //check the right string is correct

#endif // IO_H
