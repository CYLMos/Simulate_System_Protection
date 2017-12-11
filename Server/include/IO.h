#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERROR -1
#define CREATE 1
#define READ 2
#define WRITE 3
#define MODIFY 4

#define FILE_CREATE 0
#define FILE_EXIST 1
#define FILE_ERROR -1

int init(); // init

int initFile(char* fileName); // init file if they are not exist.

int strtokInput(char* input, char* others[]);

#endif // IO_H
