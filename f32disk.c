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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#define DISK_SIZE 0x1400000     //size of disk to create in hex (default 20MB)
#define DISK_DIR ".disk"        //name of directory to mount disk 

static int fd;                  //file descriptor
static char *diskpath;          //path to disk
static char fparam[255] = {0};  //function parameters buffer
static int error = 0;           
static int disk_formatted = 0;  //"disk is formatted" flag
static char pwd[255] = "/";     //path on disk
static char cwd[255] = {0};     //working directory of the program

/* Helper function to mount/unmount disk*/
int disk_mount(char *diskpath, int u) {
    
    struct stat st;
    
    if (u){                                             //case 1 - make dir and mount, case 0 - unmount and remove dir
        if (!stat(DISK_DIR, &st)) {
            if (!S_ISDIR(st.st_mode))
                return 1;
        }
        else
            if(error = mkdir(DISK_DIR, 0744)) {
                return 1;
            }

        sprintf(fparam, "mount -t vfat -o loop %s %s", diskpath, DISK_DIR);
        if(error = system(fparam))
            return 1;
    }
    else {
        sprintf(fparam, "umount %s", DISK_DIR);
        if(error = system(fparam)) {
            return 1;
        }
        if(error = rmdir(DISK_DIR)){
            return 1;
        }
    }
            
    return 0;
}

/* Helper function to open disk. Calls disk_mount() on success*/
int disk_open(char *diskpath) {
    
    fd = open(diskpath, O_CREAT|O_EXCL|O_SYNC|O_RDWR, S_IRUSR|S_IWUSR);     //opening disk file
    if (fd == -1){
        if (errno == EEXIST) {
            if(!disk_mount(diskpath, 1)) {
                disk_formatted = 1;
            }
            else
                return 1;
        }
        else {
            return 1;
        }
    }
    else {
        if(ftruncate(fd, DISK_SIZE) == -1)                                  //if disk does not exist, creating
            return 1;
    }
    close(fd);
    
    return 0;
}

/* Function for command "format"*/
int disk_format() {
    
    char answer[255];                                           //buffer for user answer
    
    if(disk_formatted) {                                        //used if we trying to format not empty disk
        puts("All data will be lost. Are you sure? (yes/no)");
        fgets(answer, sizeof(answer), stdin);
        if(!strncmp(answer, "yes", 3)) {
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
        sprintf(fparam, "mkfs -t vfat %s", diskpath);
        if(error = system(fparam))
            return 1;
        if(!disk_mount(diskpath, 1)) 
            disk_formatted = 1;
        else
            return 1;
    }
    
    return 0;
}

/* Function for command "mkdir"*/
int mk_dir(char *folder) {
    
    struct stat st;
    
    if (strlen(pwd) == 1)
        sprintf(fparam, "%s%s%s", cwd, pwd, folder);            //if root directory, fparam should be another
    else
        sprintf(fparam, "%s%s/%s", cwd, pwd, folder);
    error = stat(fparam, &st);                                  //checking if directory already exists
    if (error == -1) {
        if(error = mkdir(fparam, 0744))
            return 1;
    }
    else
        if (error == 0)
            return 2;

    return 0;
}

/* Function for command "touch"*/
int mkfile(char *file) {

    if (strlen(pwd) == 1)
        sprintf(fparam, "%s%s%s", cwd, pwd, file);              //if root directory, fparam should be another
    else
        sprintf(fparam, "%s%s/%s", cwd, pwd, file);
    fd = open(fparam, O_CREAT|O_EXCL|O_SYNC|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1){
        if (errno == EEXIST) 
            return 1;
        else
            return 2;
    }
    close(fd);
    
    return 0;
}

/* Function for command "ls"*/
int dir() {

    DIR *dir;
    struct dirent *de;
    
    sprintf(fparam, "%s%s", cwd, pwd);
    if ((dir = opendir(fparam)) == NULL)
        return 1;
    while (de = readdir(dir))
        puts(de->d_name);
    closedir(dir);
    
    return 0;
}

/* Function for command "cd"*/
int ch_dir(char *path) {
    
    struct stat st;

    sprintf(fparam, "%s%s", cwd, path);
    if (!stat(fparam, &st)) {
        if (S_ISDIR(st.st_mode))                            //if directory exists, change pwd
            strcpy(pwd, path);
        else
            return 1;
    }
    else {
       return 1;
    }
    
    return 0;
}

/* Function that displays shell. Calls appropriate function on input*/
int shell_input() {
    
    char input_buf[255];
    char *cmd;                                              //command inputed
    char *param;                                            //command parameter
    
    printf("\nWelcome back, user!\n\n");
    printf("Following commands are supported:\n\n"
            "\tformat\t\t\t- format disk\n"
            "\ttouch <file>\t\t- create file\n"
            "\tmkdir <directory>\t- create directory\n"
            "\tcd <path>\t\t- change current path\n"
            "\tls\t\t\t- show content of the current directory\n"
            "\texit\t\t\t- exit program\n"
            );
    while(1) {
        fprintf(stdout, "%s>", pwd);
        fgets(input_buf, sizeof(input_buf), stdin);
        cmd = strtok(input_buf, " ");                       //dividing input to command and parameter
        param = strtok(NULL, " ");
        if(param != NULL) {
            *(param+strlen(param)-1)=0;                     //deleting exceeding symbol
        }
        if (strtok(NULL, " ") != NULL) {
            printf("More than one parameter for command \"%s\" specified\n", cmd);
            continue;
        }
        if(!strncmp(cmd, "format", 6)) {
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
            puts("Unknown disk format. Try \"format\"");
            continue;
        }
        if(!strncmp(cmd, "ls", 2)) {
            if(dir())
                puts("Cannot display content of the directory");
            continue;
        }
        if(!strncmp(cmd, "cd", 2)) {
            if(param != NULL){
                if(ch_dir(param))
                    printf("No directory named \"%s\"\n", param);
            }
            else
                puts("You must specify a path");
            continue;
        }
        if(!strncmp(cmd, "mkdir", 5)) {
            if(param != NULL){
                error = mk_dir(param);
                if(error == 1)
                    printf("Cannot create directory \"%s\". No access\n", param);
                else if (error == 2)
                    printf("Directory/file \"%s\" already exists\n", param);
                else
                    puts("Ok");
            }
            else
                puts("You must specify directory name");
            continue;
        }
        if(!strncmp(cmd, "touch", 5)) {
            if(param != NULL){
                error = mkfile(param);
                if(error == 1)
                    printf("File/directory \"%s\" already exists\n", param);
                else if (error == 2)
                    printf("Cannot create file \"%s\". No access\n", param);
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


    if (argc == 2)                                          //checking arguments
        diskpath = argv[1];
    else {
        puts("Wrong parameters specified");
        puts("Usage: f32disk <path_to_disk>");
        return 1;
    }
    getcwd(cwd, 255);                                       //get current working directory
    sprintf(cwd, "%s/%s", cwd, DISK_DIR);
    if(disk_open(diskpath)) {                               //opening disk...
        puts("Disk file cannot be opened. Are you root?");
        return 1;
    }
    if(shell_input())                                       //opening shell 
        return 1;

    return 0;
}

