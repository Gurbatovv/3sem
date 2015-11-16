#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

/*
 * Засчитано. Но...
 * 1) Называйте константы единообразно, например, так SIZE_STR, SIZE_DELIM ...
 * 2) Называйте переменные со строчной буквы (у вас иногда проскакивает с заглавной int Num)
 */

#define  SizeStr 117
#define  SizeDelim  17
#define Word_number 100

void Split (char *string, char *delimeters, char ***tok, int *count)
{
    char *str;
    
    str = strtok(string, delimeters);
    *count = 0;
    while (str != NULL)
    {
        (*tok)[*count] = str;
        str = strtok(NULL, delimeters);
        (*count)++;
    }
    return;
}

int main()
{
    char *delimeters;
    char *string;
    char **tok;
    int Num, i;
    
    string = (char *) malloc(sizeof(char) * SizeStr);
    delimeters = (char *) malloc(sizeof(char) * SizeDelim);
    tok = (char **) malloc(sizeof(char *) * Word_number);
    
    
    gets(string);
    gets(delimeters);
    Split(string, delimeters, &tok, &Num);
    
    for (i = 0; i < Num; i++)
    {
        printf("%s\n", tok[i]);
    }
    
    free(delimeters);
    free(string);
    free(tok);
    
    return 0;
}
