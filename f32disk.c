/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: nevmnd
 *
 * Created on 5 января 2019 г., 12:53
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define DISK_SIZE 0x1400000

int fd = -1;
char *diskpath;


int disk_open(char *diskpath) {
    
    fd = open(diskpath, O_CREAT|O_EXCL|O_SYNC|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1){
        if (errno == EEXIST) {
            //puts("Disk file exists!");
        }
        else {
            puts("Disk file cannot be opened");
            return 1;
        }
    }
    else {
        ftruncate(fd, DISK_SIZE);
    }
    close(fd);
    return 0;
}

int disk_format() {
    
    return 0;
}

int mkdir() {
    
    return 0;
}

int mkfile() {
    
    return 0;
}

int dir() {
    
    return 0;
}

int ch_dir() {
    
    return 0;
}

void shell_input() {
    char input_buf[255] = {0};
    char *pwd = "/";
    char *cmd;
    char *param;
    char *ptr;
    
    printf("\nWelcome back, user!\n\n");
    while(1) {
        printf("user@localhost:%s>", pwd);
        fgets(input_buf, sizeof(input_buf), stdin);
        cmd = strtok(input_buf, " ");
        param = strtok(NULL, " ");
        if (strtok(NULL, " ") != NULL)
            puts("More than one parameter specified, others will be omitted");
        //puts(cmd);
        //puts(param);
        if(!strncmp(input_buf, "format", 6)) {
            puts("format");
            if(disk_format())
                puts("Disk cannot be formatted");
        }
        if(!strncmp(cmd, "ls", 2)) {
            puts("ls");
            dir();
            continue;
        }
        if(!strncmp(cmd, "cd", 2)) {
            puts("cd");
            ch_dir();
            continue;
        }
        if(!strncmp(cmd, "mkdir", 5)) {
            puts("mkdir");
            if(mkdir())
                puts("Directory cannot be created");
            continue;
        }
        if(!strncmp(cmd, "touch", 5)) {
            puts("touch");
            if(mkfile())
                puts("File cannot be created");
            continue;
        }
        if(!strncmp(cmd, "exit", 4)) {
            puts("Program exiting...");
            break;
        }
        puts("Unknown command");
    }
}

int main(int argc, char** argv) {


    if (argc == 2)
        diskpath = argv[1];
    else {
        puts("Wrong parameters specified");
        return 1;
    }
    
    if(disk_open(diskpath))
        return 1;
    
    shell_input();

    return 0;
}

