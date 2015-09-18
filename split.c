#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define  SizeStr 117
#define  SizeDelim  17

void Split (char *string, char *delimeters, char ***tok, int *count)
{
    *tok = NULL;
    *count = 0;
    char *s = strtok(string, delimeters);
    char **temp;
    int i,j;
    
    while (s != NULL)
    {
        /*
         * FIXIT:
         * вместо malloc + копирование используйте realloc
         */
        temp = (char**) malloc((unsigned int)sizeof(char*) * (*count + 1));
        for (i = 0; i < *count; i++)
        {
            temp[i] = (*tok)[i];
        }    
        if (*tok != NULL)
        {
            free(*tok);
        }    
        *tok = temp;
        (*tok)[*count] = s;
        (*count)++;
        s = strtok(NULL, delimeters);
    }
    return;
}

int main()
{
    char *delimeters;
    int N = 0, i = 0;
    char *string = (char *) malloc(sizeof(char) * SizeStr);
    delimeters = (char *) malloc(sizeof(char) * SizeDelim);
    char **tok;
    gets(string);
    gets(delimeters);
    Split(string, delimeters, &tok, &N);
    printf("\n");
    for (i = 0; i < N; i++)
    {
        printf("%s\n", tok[i]);
    }
    /*
     * FIXIT:
     * Нужно освободить выделенную вами память
     */
    return 0;
}
