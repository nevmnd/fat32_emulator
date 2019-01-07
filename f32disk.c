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
#define DISK_DIR ".disk"

static int fd = -1;
static char *diskpath;
static char shell_cmd[255] = {0};
static int shell_error = 0;
static int disk_formatted = 0;
static char pwd[255] = "/";


int disk_mount(char *diskpath, int u) {
    
    if (u){
        sprintf(shell_cmd, "mkdir %s", DISK_DIR);
        //puts(shell_cmd);
        if(shell_error = system(shell_cmd)) {
            printf("error = %d\n", shell_error);
            if (shell_error != 256)
                return 1;
        }
        sprintf(shell_cmd, "mount -t vfat -o loop %s %s", diskpath, DISK_DIR);
        //puts(shell_cmd);
        if(shell_error = system(shell_cmd))
            printf("error = %d\n", shell_error);
        if (shell_error)
            return 1;
    }
    else {
        sprintf(shell_cmd, "umount %s", DISK_DIR);
        //puts(shell_cmd);
        if(shell_error = system(shell_cmd))
            printf("error = %d\n", shell_error);
        if (shell_error)
            return 1;
        sprintf(shell_cmd, "rmdir %s", DISK_DIR);
        //puts(shell_cmd);
        if(shell_error = system(shell_cmd)){
            printf("error = %d\n", shell_error);
            return 1;
        }
    }
            
    return 0;
}

int disk_open(char *diskpath) {
    
    fd = open(diskpath, O_CREAT|O_EXCL|O_SYNC|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1){
        if (errno == EEXIST) {
            if(!disk_mount(diskpath, 1)) {
                disk_formatted = 1;
            }
        }
        else {
            return 1;
        }
    }
    else {
        if(ftruncate(fd, DISK_SIZE) == -1) {
            return 1;
        }
    }
    close(fd);
    return 0;
}

int disk_format() {
    
    char answer[255];
    
    if(disk_formatted) {
        puts("All data will be lost. Are you sure? (yes/no)");
        fgets(answer, sizeof(answer), stdin);
        //printf("answer = %s\n", answer);
        if(!strncmp(answer, "yes", 3)) {
            puts("Formatting disk...");
            if (!disk_mount(diskpath, 0)){
                disk_formatted = 0;
            }
            else
                return 1;
        }
        else
            return 0;
    } 
    if(!disk_formatted) {
        sprintf(shell_cmd, "mkfs -t vfat %s", diskpath);
        //puts(shell_cmd);
        if(shell_error = system(shell_cmd))
            printf("error = %d\n", shell_error);
        if (shell_error)
            return 1;
        if(!disk_mount(diskpath, 1)) 
            disk_formatted = 1;
        else
            return 1;
    }
    
    return 0;
}

int mkdir(char *folder) {
    
    sprintf(shell_cmd, "mkdir %s%s/%s", DISK_DIR, pwd, folder);
    //puts(shell_cmd);
    if(shell_error = system(shell_cmd))
        printf("error = %d\n", shell_error);
    if (shell_error)
        return 1;

    return 0;
}

int mkfile(char *file) {

    sprintf(shell_cmd, "touch %s%s/%s", DISK_DIR, pwd, file);
    //puts(shell_cmd);
    if(shell_error = system(shell_cmd))
        printf("error = %d\n", shell_error);
    if (shell_error)
        return 1;
    
    return 0;
}

int dir() {

    sprintf(shell_cmd, "ls -1a %s%s", DISK_DIR, pwd);
    //puts(shell_cmd);
    if(shell_error = system(shell_cmd))
        printf("error = %d\n", shell_error);
    if (shell_error)
        return 1;
    
    return 0;
}

int ch_dir(char *path) {
    
    sprintf(shell_cmd, "cd %s%s", DISK_DIR, path);
    //printf("path = %s\n", path);
    //printf("path length = %lu\n", strlen(path));
    if(shell_error = system(shell_cmd))
        printf("error = %d\n", shell_error);
    if (shell_error)
        return 1;
    else {
        memset(pwd, 0, strlen(pwd));
        strncpy(pwd, path, strlen(path)-1);
        //puts(shell_cmd);
        //puts(pwd);
    }
    return 0;
}

int shell_input() {
    char input_buf[255];
    char *cmd;
    char *param;
    
    printf("\nWelcome back, user!\n\n");
    while(1) {
        fprintf(stdout, "%s>", pwd);
        fgets(input_buf, sizeof(input_buf), stdin);
        cmd = strtok(input_buf, " ");
        param = strtok(NULL, " ");
        if (strtok(NULL, " ") != NULL)
            printf("More than one parameter for command \"%s\" specified, others will be omitted\n", cmd);
        //puts(cmd);
        //puts(param);
        if(!strncmp(cmd, "format", 6)) {
            puts("format");
            if(disk_format())
                puts("Disk cannot be formatted. Are you root?");
            else
                puts("Ok");
            continue;
        }
        if(!strncmp(cmd, "exit", 4)) {
            if(disk_formatted) {
                if(disk_mount(diskpath, 0))
                    return 1;
            }
            puts("Program exiting...");
            break;
        }
        if(!disk_formatted) {
            puts("Unknown disk format");
            continue;
        }
        if(!strncmp(cmd, "ls", 2)) {
            dir();
            continue;
        }
        if(!strncmp(cmd, "cd", 2)) {
            if(param != NULL){
                if(ch_dir(param))
                    puts("No such directory");
            }
            else
                puts("You must specify a path");
            continue;
        }
        if(!strncmp(cmd, "mkdir", 5)) {
            if(param != NULL){
                if(mkdir(param))
                    puts("Folder cannot be created");
                else
                    puts("Ok");
            }
            else
                puts("You must specify folder name");
            continue;
        }
        if(!strncmp(cmd, "touch", 5)) {
            if(param != NULL){
                if(mkfile(param))
                    puts("File cannot be created");
                else
                    puts("Ok");
            }
            else
                puts("You must specify file name");
            continue;
        }
        puts("Unknown command");
    }
    
    return 0;
}

int main(int argc, char** argv) {


    if (argc == 2)
        diskpath = argv[1];
    else {
        puts("Wrong parameters specified");
        return 1;
    }
    
    if(disk_open(diskpath)) {
        puts("Disk file cannot be opened. Are you root?");
        return 1;
    }
    
    if(shell_input())
        return 1;

    return 0;
}

