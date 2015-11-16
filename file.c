#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NAME 1024
#define MAX_PATH  1024

int Search_files(char* dir_name, int depth, char* file_name)
{
    DIR* dir;
    struct dirent* use;
    
    if (depth < 0)
    {
        return -1;
    }
    
    if ((dir = opendir(dir_name)) == NULL)
    {
        return -1;
    }

    while ((use = readdir(dir)) != NULL)
    {
        if (strcmp(file_name, use->d_name) == 0)
        {
            dir_name = strcat(dir_name, "/");
            dir_name = strcat(dir_name, use->d_name);
            return 0;
        }

        if (strcmp(use->d_name, ".") != 0 && strcmp(use->d_name, "..") != 0)
        {
            dir_name = strcat(dir_name, "/");
            dir_name = strcat(dir_name, use->d_name);
            if (Search_files(dir_name, depth - 1, file_name) >= 0)
            return 0;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    int depth;
    char s[NAME];
    char path[MAX_PATH] = ".";
    
    strcpy(s, argv[1]);
    depth = atoi(argv[2]);
    
    /*
     * запустите программу с параметрами ./a.out file.c 2
     * будет выведена очень длинная строка. дело в том, что после выхода из рекурсивного вызова необходимо обирать название последней директории
     * из строки dir_name
     */

 /*Простите, я не совсем понял как это реализовать*/
    
    if (Search_files(path, depth, s) >= 0)
    {
        printf("%s\n", path);
    }
    else
    {
        printf("No such file in this directory.\n");
    }

    return 0;
}

