#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER,
    nLock=PTHREAD_MUTEX_INITIALIZER,
    sLock=PTHREAD_MUTEX_INITIALIZER,
    wLock=PTHREAD_MUTEX_INITIALIZER,
    eLock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t northQueue=PTHREAD_COND_INITIALIZER
    , eastQueue=PTHREAD_COND_INITIALIZER
    , southQueue=PTHREAD_COND_INITIALIZER
    , westQueue=PTHREAD_COND_INITIALIZER
    ,northFirst=PTHREAD_COND_INITIALIZER
    , eastFirst=PTHREAD_COND_INITIALIZER
    , southFirst=PTHREAD_COND_INITIALIZER
    , westFirst=PTHREAD_COND_INITIALIZER;

int nCount=0,eCount=0,sCount=0,wCount=0;
void arrive(int b,char* dir,pthread_mutex_t q,pthread_mutex_t first)
{
    printf("BAT %d from %s arrives at crossing\n", b, dir);
    if(strcmp(dir,"NORTH")==0){
        pthread_mutex_lock(&nLock);
        nCount++;
        pthread_mutex_unlock(&nLock);
        if(nCount>1){
            pthread_cond_wait(&northQueue, &q);
            pthread_cond_wait(&westFirst, &first);
        }
    }else if(strcmp(dir,"WEST")==0 ){
        pthread_mutex_lock(&wLock);
        wCount++;
        pthread_mutex_unlock(&wLock);
        if(wCount>1){
            pthread_cond_wait(&westQueue, &q);
            pthread_cond_wait(&southFirst, &first);
        }

    }else if(strcmp(dir,"SOUTH")==0 ){
        pthread_mutex_lock(&sLock);
        sCount++;
        pthread_mutex_unlock(&sLock);
        if(sCount>1){
            pthread_cond_wait(&southQueue, &q);
            pthread_cond_wait(&eastFirst, &first);
        }

    }else if(strcmp(dir,"EAST")==0 ){
        pthread_mutex_lock(&eLock);
        eCount++;
        pthread_mutex_unlock(&eLock);
        if(eCount>1){
            pthread_cond_wait(&eastQueue, &q);
            pthread_cond_wait(&northFirst, &first);
        }

    }


 // code to check traffic in line, use counters, condition variables
}
void cross(int b,char* dir)
{
    pthread_mutex_lock(&lock);
    printf("BAT %d from %s crossing\n", b, dir);
    sleep(1); // it takes one second for a BAT to cross
    pthread_mutex_unlock(&lock);
}
void leave(int b,char* dir)
{
    printf("BAT %d from %s leaving crossing\n", b, dir);
// code to check traffic, use counters, condition variables etc.
    if(strcmp(dir,"NORTH")==0){
        pthread_mutex_lock(&nLock);
        nCount--;
        pthread_mutex_unlock(&nLock);
        pthread_cond_signal(&northQueue);
        pthread_cond_signal(&northFirst);
    }else if(strcmp(dir,"WEST")==0){
        pthread_mutex_lock(&wLock);
        wCount--;
        pthread_mutex_unlock(&wLock);
        pthread_cond_signal(&westQueue);
        pthread_cond_signal(&westFirst);
    }else if(strcmp(dir,"SOUTH")==0){
        pthread_mutex_lock(&sLock);
        sCount--;
        pthread_mutex_unlock(&sLock);
        pthread_cond_signal(&southQueue);
        pthread_cond_signal(&southFirst);
    }else if(strcmp(dir,"EAST")){
        pthread_mutex_lock(&eLock);
        eCount--;
        pthread_mutex_unlock(&eLock);
        pthread_cond_signal(&eastQueue);
        pthread_cond_signal(&eastFirst);
    }
}

struct BAT{
    int num;
    char* dir;
    pthread_mutex_t mutexQueue;
    pthread_mutex_t mutexFirst;

};
void* trythis(void *arg)
{
    struct BAT *tinfo=arg;
    arrive(tinfo->num,tinfo->dir,tinfo->mutexQueue,tinfo->mutexFirst);
    cross(tinfo->num,tinfo->dir);
    leave(tinfo->num,tinfo->dir);
    pthread_exit(NULL);
}
int getLength(char s[100]){
    int l=0;
    while(s[l]!='\0'){
        l++;
    }
    return l;
}
int main()
{
    char arg[100];
    scanf("%s",&arg);
    int length=getLength(arg);
    struct BAT tinfo[length];
    pthread_t threads[length];

    int i;
    for(i=0;i<length;i++){

        tinfo[i].num=i+1;
        if(arg[i]=='n'){
            tinfo[i].dir="NORTH";
        }else if(arg[i]=='s'){
            tinfo[i].dir="SOUTH";
        }else if(arg[i]=='e'){
            tinfo[i].dir="EAST";
        }else if(arg[i]=='w'){
            tinfo[i].dir="WEST";
        }
        if(pthread_create(&threads[i],NULL,&trythis,(void*)&tinfo[i])!=0){
            perror("Error!\n");
        }
    }
    void *ret;
    for(i=0;i<length;i++){
        if (pthread_join(threads[i], &ret) != 0) {
            perror("pthread_create() error");
            exit(3);
  }

    }
    return 0;
}
