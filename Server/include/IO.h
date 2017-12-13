#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>
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

#define FILE_CREATE 0
#define FILE_EXIST 1
#define FILE_ERROR -1

#define OWNER_READ 0
#define OWNER_WRITE 1
#define GROUP_READ 2
#define GROUP_WRITE 3
#define OTHER_READ 4
#define OTHER_WRITE 5

#define RIGHT_LEN 6


typedef struct user{
    char* name;
    char* group;
    int id;
    struct user* next;
};

struct user* root;

int id;


int init(); // init

int initFile(char* fileName); // init file if they are not exist.

int strtokInput(char* input, char* others[]);

char* doCommandLine(int behavior, char* others[]);

int rightCheck(char* rights);

#endif // IO_H
