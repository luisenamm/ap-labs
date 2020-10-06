#define _XOPEN_SOURCE 500
#define BUF_LEN (10*(sizeof(struct inotify_event)+100))

#include <stdio.h>
#include "logger.h"
#include <ftw.h>
#include <sys/inotify.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

int inotifyFd,ld, fdLimit=15;
int flags = FTW_CHDIR | FTW_DEPTH | FTW_MOUNT;

char* subDir[300],decriptor[300], lastName;

static int display_info(const char* fpath,const struct stat *sb,int tflag,struct FTW *ftwbuf){
    if(tflag==FTW_D){
        ld=inotify_add_watch(inotifyFd,fpath,IN_CREATE|IN_DELETE|IN_MOVE);
        if(ld==-1)
            ferror("inotify_add_watch");
    }
    return 0;
}

static void displayInotifyEvent(struct inotify_event *i){
    if(i->mask & IN_CREATE)  infof("Created %s\n",i->name);
    if(i->mask & IN_DELETE)  infof("Deleted %s\n",i->name);
    if(i->mask & IN_MOVED_FROM)  infof("Moving from %s\n",i->name);
    if(i->mask & IN_MOVED_TO) infof("Moving to %s\n",i->name);
}


int main(int argc, char *argv[]){
    // Place your magic here
    int flags=0;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char* p;
    struct inotify_event *event;

    inotifyFd = inotify_init();
    if (inotifyFd == -1)
        perror("inotify_init");

    if (argc > 2 && strchr(argv[2], 'd') != NULL)
        flags |= FTW_DEPTH;
    if (argc > 2 && strchr(argv[2], 'p') != NULL)
        flags |= FTW_PHYS;
    
    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1){
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    if (argc < 2 || strcmp(argv[1], "--help") == 0){
        errorf("Usage is: %s <PATHNAME>\n", argv[0]);
        return 0;
    }

    for(;;){
        numRead=read(inotifyFd,buf,BUF_LEN);
        if(numRead==0)
            perror("read() from inotify fd returned 0");
        if(numRead==-1)
            perror("read");
        for(p=buf;p<buf+numRead;){
            event=(struct inotify_event *) p;
            displayInotifyEvent(event);
            p+=sizeof(struct inotify_event)+event->len;
        }
    }

    exit(EXIT_SUCCESS);
    return 0;
}
