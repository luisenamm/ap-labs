#include <stdio.h>
#include "logger.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 2000

int NUM_BUFFERS;
char *RES_FILE;

long **buffers;
pthread_mutex_t *mutexes;
pthread_t threads[NUM_THREADS];
long *result; 

struct argsThread{
    int row;
    int col;
    long *matA;
    long *matB;
};

long *readMatrix(char* filename);
long *getColumn(int col,long* matrix);
long *getRow(int row,long* matrix);
int getLock();
int releaseLock(int lock);
long dotProduct(long* vec1,long* vec2);
long *multiply(long* matA,long* matB);
int saveResultMatrix(long* result);

long *readMatrix(char* filename){
    long *matrix=(long *)malloc(2000*2000*sizeof(long));

    int mf,n,pos=0, concatPos=0;;
    if((mf=open(filename,O_RDONLY))==-1){
        errorf("Cannot open matrix");
    }
    char actualChar;
    char *charToConcat=(char*) malloc(sizeof(long)*sizeof(char));

    while((n=read(mf,&actualChar,1)>0)){
        if(actualChar!='\n'){
            charToConcat[concatPos++]=actualChar;
        }else{
            charToConcat[concatPos++]='\0';
            concatPos=0;
            matrix[pos++]=atol(charToConcat);
        }
    }
    if(concatPos>0){
        matrix[pos++]=atol(charToConcat);
    }
    close(mf);
    return matrix;
}

long *getColumn(int col,long* matrix){
    long *colVal=(long*)malloc(2000*sizeof(long));
    long pos=0;
    for(int i=col;i<2000*2000;i+=2000){
        colVal[pos++]=matrix[i];
    }
    return colVal;
}

long *getRow(int row,long* matrix){
    long *rowVal=(long*)malloc(2000*sizeof(long));
    long pos=0;
    for(int i=2000*row;i<2000*row+2000;i++){
        rowVal[pos++]=matrix[i];
    }
    return rowVal;
}

int getLock(){
    for(int i=0;i<NUM_BUFFERS;i++){
        if(pthread_mutex_trylock(&mutexes[i])==0)
            return i;
    }
    return -1;
}

int releaseLock(int lock){
    return pthread_mutex_unlock(&mutexes[lock]);
}

long dotProduct(long* vec1,long* vec2){
    long res=0;
    for(int i=0;i<2000;i++)
        res+=vec1[i]*vec2[i];
    return res;
}

long mult(struct argsThread *data){
    int bufA=-1,bufB=-1;
    while(bufA==-1 || bufB==-1){
        if(bufA==-1){
            bufA=getLock();
        }if(bufB==-1){
            bufB=getLock();
        }
    }
    buffers[bufA]=getRow(data->row,data->matA);
    buffers[bufB]=getColumn(data->col,data->matB);
    long res=dotProduct(buffers[bufA],buffers[bufB]);
    free(buffers[bufA]);
    free(buffers[bufB]);
    free(data);
    releaseLock(bufA);
    releaseLock(bufB);
    return res;    
}

long *multiply(long* matA,long* matB){
    infof("Start the multiplication");
    long *matrix=malloc(NUM_THREADS*NUM_THREADS*sizeof(long));
    for(int i=0;i<NUM_THREADS;i++){
        for(int j=0;j<NUM_THREADS;j++){
            struct argsThread *ar=malloc(sizeof(struct argsThread));
            ar->row=i;
            ar->col=j;
            ar->matA=matA;
            ar->matB=matB;
            pthread_create(&threads[j],NULL,(void * (*) (void *))mult,(void *)ar);
        }
        for(int j=0;j<NUM_THREADS;j++){
            void *result;
            pthread_join(threads[j],&result);
            matrix[NUM_THREADS*j+i]=(long)result;
        }
    }
    infof("Matrix multiplication completed");
    return matrix;
}

int saveResultMatrix(long* result){
    FILE *file;
    file = fopen(RES_FILE, "w+");
    if(file==NULL){
        errorf("Cannot create results file");
        return -1;
    }
    int size=NUM_THREADS*NUM_THREADS;
    for(int i = 0; i < size; i++) {
        fprintf(file, "%ld\n", result[i]);
    }
    fclose(file);
    return 0;
}

int main(int argc,char **argv){
    // Place your magic here
    if(argc!=5){
        errorf("Wrong number of parameters");
        return -1;
    } 
    
    if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-out") == 0){
        NUM_BUFFERS = atoi(argv[2]);
        RES_FILE = argv[4];
    }
    else if (strcmp(argv[1], "-out") == 0 && strcmp(argv[3], "-n") == 0){
        RES_FILE = argv[2];
        NUM_BUFFERS = atoi(argv[4]);
    }else{
        errorf("Wrong usage of the program");
        return -1;
    }

    buffers = malloc(NUM_BUFFERS * sizeof(long *));
    mutexes = malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));

    for(int i = 0; i < NUM_BUFFERS; i++)
        pthread_mutex_init(&mutexes[i], NULL);

    long *matA = readMatrix("matA.dat");
    long *matB = readMatrix("matB.dat");

    long *res = multiply(matA, matB);

    infof("Saving results");
    saveResultMatrix(res);

    free(matA);
    free(matB);
    free(mutexes);
    free(buffers);

    return 0;
}
