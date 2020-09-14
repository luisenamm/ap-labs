#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

struct data{
    char packName[100],
        insDate[100],
        upDate[100],
        remDate[100];
    int numUpgrades;
};

struct data maxPackages[2000];

int readLine(FILE* file,char* row,size_t size);
char *getName(char *row);
char *getdate(char *row);
void analizeLog(char *logFile, char *report);
int packStatus(char *row);
void writeInfo(FILE* src,int* dest);

int main(int argc, char **argv) {
    if (argc !=5 || strcmp(argv[1],"-input") != 0 || strcmp(argv[3],"-report") != 0) {
	    printf("Wrong arguments. Use: ./pacman-analizer.o -input <inputFile> -report <destinationFile>\n");
	    return 0;
    }
    analizeLog(argv[2], argv[4]);
    return 0;
}

int readLine(FILE *file, char *row, size_t size){
    int n,cont=0;
    while ((n = (char) getc(file)) != '\n' && cont<size){
        if (n == EOF){
            break;
        }
        row[cont] = (char) n;
        cont++;
    }
    row[cont] ='\0';
    return cont;
}

char* getName(char *row){
    int cont = 0, startPos = 0, size = 0;
    for(int i=0;i<2;i++){
        for(startPos;row[startPos]!=']';startPos++);
        startPos+=2;
    }
    startPos++;
    for(startPos;row[startPos]!=' ';startPos++);
    startPos++;
    for(int i=startPos+1;row[i]!=' ';i++){
        size++;
    }
    char* name=malloc(size);
    for(int i=startPos;row[i]!=' ';i++,cont++){
        name[cont]=row[i];
    }
    return name;
}

char* getDate(char *row){
    int cont=0,size=0;
    for(int i=0;row[size]!=']';size++);
    size++;
    char* date=calloc(1,size);
    for(int i=0;i<size;i++,cont++)
        date[cont]=row[i];
    return date;
}


void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    // Implement your solution here.
    FILE*  file= fopen(logFile, "r");
    if (file == NULL){
        printf("Error opening log file \n");
        return;
    }
    int reportFile = open(report, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (reportFile < 0) {
        perror("An error ocurred while creating/opening the report file");
        return;
    }
    
    writeInfo(file,reportFile);
    
    if (close(reportFile) < 0){
        perror("Error while trying to close the report file");
        exit(1);
    }

   

    printf("Report is generated at: [%s]\n", report);
}

int packStatus(char* row){
    int startPos=0;
    for(int i=0;i<2;i++){
        for(startPos;row[startPos]!='\0';startPos++)
            if(row[startPos]==']')
                break;
        startPos+=2;
    }
    if (row[startPos] == 'i' && row[startPos + 1] == 'n'){
        return 1;
    }else if (row[startPos] == 'u' ){
        return 2;
    }else if (row[startPos] == 'r' && row[startPos + 1] == 'e' && row[startPos + 2] == 'm'){
        return 3;
    }
    return 0;
}

void writeInfo(FILE* src,int* dest){
    char row[255],temp[10];
    int x, installed=0, removed=0, upgraded=0, current=0,reportFile=dest;
    FILE*  file= src;

    while (x = readLine(file, row, 255) > 0){
        int status = packStatus(row);
        if (status == 1){
            char* name = getName(row);
            char* date = getDate(row);
            strcpy(maxPackages[current].packName, name);
            strcpy(maxPackages[current].insDate, date);
            maxPackages[current].numUpgrades = 0;
            strcpy(maxPackages[current].remDate, "-");
            installed++;
            current++;
        } else if (status == 2){
            char* name = getName(row);
            char* date = getDate(row);
            for (int i = 0; i < 1500; i++){
                if (strcmp(maxPackages[i].packName, name) == 0){
                    strcpy(maxPackages[i].upDate, date);
                    if (maxPackages[i].numUpgrades == 0){
                        upgraded++;
                    }
                    maxPackages[i].numUpgrades++;
                    break;
                }
            }
        } else if (status == 3){
            char* name = getName(row);
            char* date = getDate(row);
            for (int i = 0; i < 1500; i++){
                 if (strcmp(maxPackages[i].packName, name) == 0){
                    strcpy(maxPackages[i].remDate, date);
                }
                break;
            }
            removed++;
        }
    }

    write(reportFile, "Pacman Packages Report\n", strlen("Pacman Packages Report\n"));
    write(reportFile,"----------------------\n",strlen("----------------------\n"));
    write(reportFile, "Installed packages : ", strlen("Installed packages : "));
    sprintf(temp, "%d\n", installed);
    write(reportFile, temp, strlen(temp));
    write(reportFile, "Upgraded packages : ",strlen("Upgraded packages : "));
    sprintf(temp, "%d\n", upgraded);
    write(reportFile, temp, strlen(temp));
    write(reportFile, "Removed packages : ",strlen("Removed packages : "));
    sprintf(temp, "%d\n", removed);
    write(reportFile, temp, strlen(temp));
    write(reportFile, "Current installed : \n",strlen("Current installed : "));
    sprintf(temp, "%d\n", (installed-removed));
    write(reportFile, temp, strlen(temp));
    //-----------------------------------------------------
    //-----------------------------------------------------
    //-----------------------------------------------------
    //-----------------------------------------------------
    //-----------------------------------------------------
    write(reportFile, "\n\nList of packages\n", strlen("\n\nList of packages\n"));
    write(reportFile,"----------------------\n",strlen("----------------------\n"));
    for(int i = 0; i < 1500; i++){
        if(strcmp(maxPackages[i].packName, "") != 0){
            write(reportFile, "- Package name         : ",strlen("- Package name         : "));
            write(reportFile,maxPackages[i].packName, strlen(maxPackages[i].packName));
            write(reportFile, "\n   - Install date      : ",strlen("\n   - Install date      : "));
            write(reportFile,maxPackages[i].insDate, strlen(maxPackages[i].insDate));
            write(reportFile, "\n   - Last update date  : ",strlen("\n   - Last update date  : "));
            write(reportFile,maxPackages[i].upDate, strlen(maxPackages[i].upDate));
            write(reportFile, "\n   - How many updates  : ",strlen("\n   - How many updates  : "));
            sprintf(temp, "%d", maxPackages[i].numUpgrades);
            write(reportFile,temp, strlen(temp));
            write(reportFile, "\n   - Removal date      : ",strlen("\n   - Removal date      : "));
            write(reportFile,maxPackages[i].remDate, strlen(maxPackages[i].remDate));
            write(reportFile, "\n",strlen("\n"));
        } else if (strcmp(maxPackages[i].packName, "") == 0){
            break;
        }
    }
}