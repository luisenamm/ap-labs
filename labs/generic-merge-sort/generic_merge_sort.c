#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 10000

void mergeSort(void *array[], int l, int r, int (*cmpFun)(void *, void *));
void merge(void *array[], int l, int m, int r, int (*cmpFun)(void *, void *));
int numcmp(const char *s1, const char *s2);
void printMerged(int size,char *data[]);

char *lines[MAX_LINES];

int main(int argc, char** argv){
    int numerical = 0;
    FILE *file;

    if(argc < 2) {
        printf("Usage: ./generic_merge_sort (-n) <filename>\n");
        return 0;
    }else if(argc == 2 && strcmp(argv[1],"-n") == 0){
        printf("Please write the file name \n");
        return 0;
    }

    if(argc == 3 && strcmp(argv[1],"-n") == 0){
        numerical = 1;
    }

    if((file = fopen(argv[2],"r")) == NULL&&(file = fopen(argv[1],"r")) == NULL){
        perror("An error ocurred while opening file");
        return 0;
    }

    char line[256] = {0};
    int n = 0;
    while(fgets(line,100,file)){
        lines[n] = (char*)malloc(strlen(line) + sizeof(char*));
        strcpy(lines[n],line);
        n++;
    }
    if(fclose(file)){
        perror("Error while trying to close file");
    }
    mergeSort((void *) lines, 0, index-1, (int (*)(void*, void*)) (numerical ? numcmp : strcmp));
    printMerged(index,lines);
    return 0;
}

void mergeSort(void *lineptr[], int l, int r, int (*comp)(void *, void *)){
    if (l < r){
        int m = l + (r - l) / 2;
        mergeSort(lineptr, l, m, (*comp));
        mergeSort(lineptr, m + 1, r, (*comp));
        merge(lineptr, l, m, r, (*comp));
    }
}

void merge(void *lines[], int l, int m, int r, int (*comp)(void *, void *)){
    int i, j, k,
        n1 = m - l + 1,
        n2 = r - m;
    char *L[n1], *R[n2];

    for (i = 0; i < n1; i++){
        L[i] = lines[l + i];
    }
    for (j = 0; j < n2; j++){
        R[j] = lines[m + 1 + j];
    }
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2){
        if ((*comp)(L[i], R[j]) < 0){
            lines[k] = L[i];
            i++;
        }
        else{
            lines[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1){
        lines[k] = L[i];
        i++;
        k++;
    }
    while (j < n2){
        lines[k] = R[j];
        j++;
        k++;
    }
}

int numcmp(const char *s1, const char *s2){
    double v1, v2;
    v1 = atof(s1);
    v2 = atof(s2);

    if (v1 < v2)
        return -1;
    else if (v1 > v2)
        return 1;
    else
        return 0;
}

void printMerged(int size,char *data[]){
    for(int i=0;i<size;i++)
        printf("%s",data[i]);
    printf("\n");

}