#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv){
    if (argc !=4){
        printf("Not the correct number of arguments\n");
        return 0;
    }
    char *instruction=argv[1];
    char *string1 =argv[2];
    char *string2=argv[3];
    
    if(strcmp(instruction,"-add")==0){
        char *newString=mystradd(string1,string2);
    }else if(strcmp(instruction,"-find")==0){
        mystrfind(string1,string2);
    }

    return 0;
}