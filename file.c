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
            char *str = strchr(dir_name, '/');
	    /*
	     * FIXIT:
	     * данная ф-я ищет вхождение первого символа в строку.
	     * http://cppstudio.com/post/726/
	     * а нужно последнее вхождение найти
	     * +
	     * неплохо бы проверять, что str != NULL.
	     * 
	     * Делая так, вы не полностью обойдёте дерево файловой системы и можете не найти нужный файл.
	     */
            *str = '\0';
	    /*
	     * Как работает поиск в глубину? Вы находитесь в некоторой директории "dir". 
	     * Внутри неё ещё несколько "dir1", "dir2" ... "dirN". 
	     * Вы в конец текущей приписываете название первой "dir/dir1" и рекурсивно обходите поддерево с корнем в этой директории.
	     * Обойдя его, вы должны рекурсино запуститься от "dir/dir2", но в переменной dir_name у вас уже записано как минимум "dir/dir1".
	     * 
	     * Решение 1: перед рекурсивным запуском вы создаёте новую строку "char nextPath[MAX_PATH];", в которую копируете переменную dir_name, затем пишете "/", потом "use->d_name". Запускаетесь от неё.
	     * Решение 2: после выхода из search, т.е. ровно здесь вы должны стереть конец строки до ближайшего "/", т.е. например, "dir/dir1" перевести "dir". Для этого с конца строки бежите в начало и ищете
	     * первый символ "/". В это место ставите символ конца строки '\0'.
	     *
	     * Можете выбрать любой. 
	     */
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

