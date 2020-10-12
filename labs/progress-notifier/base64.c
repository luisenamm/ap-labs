#include <stdio.h>
#include "logger.h"
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

char *substring(char *line,int init,int end);
int mystrlen(char *str);

long total=0,current=0;

static const unsigned char d[]={
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

//https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C
int encode64(const void* data_buf,size_t dataLength,char* result,size_t resultSize){
    const char baseChars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint8_t *data=(const uint8_t *) data_buf;
    size_t resIndex=0, x;
    uint32_t n=0;
    int padCount=dataLength%3;
    uint8_t n0,n1,n2,n3;

    total=dataLength,current=dataLength;

    for(x=0;x<dataLength;x+=3){
        n=((uint32_t)data[x])<<16;

        if((x+1)<dataLength)
            n+=((uint32_t)data[x+1])<<8;
        if((x+2)<dataLength)
            n+=data[x+2];

        n0=(uint8_t) (n>>18) & 63;
        n1=(uint8_t) (n>>12) & 63;
        n2=(uint8_t) (n>>6) & 63;
        n3=(uint8_t)n & 63;

        if(resIndex>=resultSize) return 1;
        result[resIndex++]=baseChars[n0];
        if(resIndex>=resultSize) return 1;
        result[resIndex++]=baseChars[n1];

        if((x+1)<dataLength){
            if(resIndex>=resultSize) return 1;
            result[resIndex++]=baseChars[n2];
        }

        if((x+2)<dataLength){
            if(resIndex>=resultSize) return 1;
            result[resIndex++]=baseChars[n3];
        }

        current-=3;
    }

    if(padCount>0){
        for(;padCount<3;padCount++){
            if(resIndex>=resultSize) return 1;
            result[resIndex++]= '=';
        }
    }
    if(resIndex>=resultSize)return 1;
    result[resIndex]=0;
    return 0;
}

//https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C_2
int decode64 (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    
    total = (long)end - (long)in;

    while (in < end) {

        current = (long)end - (long)in;

        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;
        case INVALID:    return 1;
        case EQUALS:
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++;
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1;
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }

    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1;
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1;
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len;
    return 0;
}

int encodeText(char * file){
    FILE *read,*write;
    char *fileName=substring(file,0,(mystrlen(file)-4));
  
    if((read=fopen(file,"r"))==NULL){
        errorf("Error opening file %s",file);
        return 0;
    }

    write=fopen(strcat(fileName,"-encoded.txt"),"w");

    size_t pos=ftell(read);
    fseek(read,0,SEEK_END);
    size_t length=ftell(read);
    fseek(read,pos,SEEK_SET);

    char *data=malloc(length);
    fread(data,1,length,read);
    char *result=(char*)malloc(sizeof(char)*strlen(data)*4);

    infof("Encoding");

    encode64(data,length,result,sizeof(char)*strlen(data)*4);

    fprintf(write,"%s",result);

    fclose(write);
    fclose(read);
    infof("Done");

    return 0;
}

int decodeText(char* file) {
    FILE *read, *write;
    char *fileName=substring(file,0,(mystrlen(file)-12));
    if ((read = fopen(file,"r")) == NULL){
        errorf("The file %s does not exist", file);
        return 0;
    }

    infof("Decoding");

    write=fopen(strcat(fileName,"-decoded.txt"),"w");

    size_t pos = ftell(read);
    fseek(read, 0, SEEK_END);
    size_t length = ftell(read);
    fseek(read, pos, SEEK_SET);

    char* data = malloc(length);
    fread(data, 1, length, read);

    unsigned char* result = (unsigned char*)malloc(sizeof(char) * length * 4);
    size_t rSize = sizeof(unsigned char *) * length * 4;

    decode64(data,length,result,&rSize);

    fprintf(write,"%s",result);

    fclose(write);
    fclose(read);

    infof("Done");

    return 0;
}


void signalHandler(int signal){
    if(signal==SIGUSR1 || signal==SIGINT)
        infof("Progress: %d%%.\nData remaining: %ld",(int)(100-(((current*1)/(total*1))*100)),current);
}

char *substring(char *line,int init,int end){
    char *word=malloc(sizeof(char)*(end-init+1));
    for(int i=0;i<end-init;i++)
        word[i]=line[i+init];
    word[end-init]='\0';
    return word;
}

int mystrlen(char *str){
    int size = 0;
    while (str[size] != NULL)
        size++;
    return size;
}


int main(int argc,char *argv[]){
    // Place your magic here
    signal(SIGUSR1,signalHandler);
    signal(SIGINT,signalHandler);

    if(argc!=3){
        errorf("Wrong Usage. Try ./base64  --encode/--decode input.txt");
        return 0;
    }
    if(strcmp(argv[1],"--encode")==0){
        encodeText(argv[2]);
        return 0;
    }else if(strcmp(argv[1],"--decode")==0){
        decodeText(argv[2]);
        return 0;
    }else{
        warnf("Wrong option, try again");
        return 0;
    }

    return 0;
}