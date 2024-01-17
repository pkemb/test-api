#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>

void show_cwd(char *msg);
int list_dir(char *path);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s path\n", argv[0]);
        return -1;
    }

    char *path = argv[1];
    show_cwd("chroot之前");
    int ret = chroot(path);
    if (ret != 0) {
        printf("chroot fail, %s\n", strerror(errno));
        return -1;
    }
    show_cwd("chroot之后");
    chdir("/");
    show_cwd("chdir之后");

    printf("根目录下面的文件：\n");
    list_dir("/");
    return 0;
}

void show_cwd(char *msg)
{
    char *cwd = get_current_dir_name();
    printf("%s：", msg);
    if (cwd == NULL) {
        printf("get_current_dir_name fail: %s\n", strerror(errno));
        return;
    }
    printf("%s\n", cwd);
    free(cwd);
}

int list_dir(char *path)
{
    DIR *dir;
    struct dirent *ptr;
    // char base[1000];

    if ((dir=opendir(path)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
            printf("d_name: %s\n", ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            printf("d_name: %s\n", ptr->d_name);
        else if(ptr->d_type == 4)    ///dir
        {
            printf("d_name: %s\n", ptr->d_name);
            // memset(base,'\0',sizeof(base));
            // strcpy(base,path);
            // strcat(base,"/");
            // strcat(base,ptr->d_name);
            // readFileList(base);
        }
    }
    closedir(dir);
    return 0;
}
