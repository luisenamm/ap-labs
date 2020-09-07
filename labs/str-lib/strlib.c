#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *str)
{
    int size = 0;
    while (str[size] != NULL)
    {
        size++;
    }
    return size;
}

char *mystradd(char *origin, char *addition)
{
    char *newString = malloc(mystrlen(origin) + mystrlen(addition));
    int size1 = mystrlen(origin);
    int size2 = mystrlen(addition);
    for (int i = 0; i < size1; i++)
    {
        newString[i] = origin[i];
    }

    int i = 0;
    for (int j = size1; j < size2 + size1; j++)
    {
        newString[j] = addition[i];
        i++;
    }
    printf("Initial Lenght: %d\n", mystrlen(origin));
    printf("New String: %s\n", newString);
    printf("New Lenght: %d\n", mystrlen(newString));
    return(0);
}

int mystrfind(char *origin, char *substr){
	int pos=0;
    while (*origin!= NULL) {
        while(*origin==*substr){
            origin++,substr++;
            if(*substr==NULL){
               printf("%s string was found at [%d] position\n",substr,pos);
               return 0;
            }
        }
        substr=&substr[0],pos++,origin++;
    }
    printf("%s string was not found\n",substr);
    return 0;

}