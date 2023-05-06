#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>

#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

sem_t *logSem = NULL;
sem_t *logSem2 = NULL;
sem_t logSem3;
sem_t cond;
sem_t cond1, cond2;

int count = 0;

typedef struct {
    int noThread;
    sem_t *logSem;
} TH_STRUCT;

void *thread_function(void *argc){
    TH_STRUCT *s = (TH_STRUCT*) argc;
    switch (s->noThread){
        case 0: 
                sem_wait(logSem);
                break;
        case 1: 
                info(BEGIN, 6, 2);
                info(END, 6, 2);
                break;
        case 2: 
                sem_wait(logSem);
                info(BEGIN, 8, 1);
                info(END, 8, 1);
                info(BEGIN, 6, 3);
                info(END, 6, 3);
                sem_post(logSem2);
                break;
        case 3: 
                info(BEGIN, 6, 1);  
                info(BEGIN, 6, 4); 
                sem_post(logSem);           
                info(END, 6, 4);
                info(END, 6, 1);
                break;
        default:
                info(BEGIN, 6, 0);
                info(END, 6, 0);
                break;
    }
    return NULL;
}

void *thread_function2(void *argc){
    TH_STRUCT *s = (TH_STRUCT*) argc;
    switch (s->noThread){
        case 0:
                
               // info(BEGIN, 8, 1);
                sem_wait(logSem);
               // info(END, 8, 1);
                sem_post(logSem2);
                break;
        case 1:
                
                info(BEGIN, 8, 3);
                info(END, 8, 3);
                
                
                break;
        case 2:
                sem_wait(logSem2);
                info(BEGIN, 8, 2);
                info(END, 8, 2);
                
                break;
        case 3:
                info(BEGIN, 8, 4);
                info(END, 8, 4);
        default: 
                break;
    }
    return NULL;
}

void *thread_function3(void*argc){
    TH_STRUCT* s = (TH_STRUCT*) argc;
    sem_wait(&logSem3);



    info(BEGIN, 2, s->noThread+1);


    info(END, 2, s->noThread+1);



    sem_post(&logSem3);

    
    return NULL;
}

int main(){
    pid_t pid2 = -1, pid3 = -1, pid4 = -1, pid5 = -1, pid6 = -1, pid7 = -1, pid8 = -1;
   
    
    
    TH_STRUCT thread[4];
    pthread_t t6[4];
    TH_STRUCT thread8[4];
    pthread_t t8[4];
    TH_STRUCT thread2[42];
    pthread_t t2[42];

    init();

    logSem=sem_open("sem1", O_CREAT, 0644, 1);
    if(logSem == NULL){
        return -1;
    }

    logSem2=sem_open("sem2", O_CREAT, 0644, 1);
    if(logSem2 == NULL){
        return -1;
    }

    if(sem_init(&logSem3, 0, 6) != 0) {
        perror("Could not init the semaphore");
        return -1;
    }

    if(sem_init(&cond, 0, 1) != 0) {
        perror("Could not init the semaphore");
        return -1;
    }

    if(sem_init(&cond1, 0, 0) != 0) {
        perror("Could not init the semaphore");
        return -1;
    }
        if(sem_init(&cond2, 0, 0) != 0) {
        perror("Could not init the semaphore");
        return -1;
    }

    info(BEGIN, 1, 0);
  
    pid2 = fork();  
    if(pid2 == 0){
        info(BEGIN, 2, 0);

        for(int i =0;i<42;i++){
            thread2[i].noThread = i;
            thread2[i].logSem = &logSem3;
            if(pthread_create(&t2[i], NULL, thread_function3, &thread2[i]) != 0){
                perror("Error creating thread!");
                return -1;
            }
          
        }

        for(int i=0;i<42;i++){
            pthread_join(t2[i],NULL);
        }

        pid4 = fork();
        if(pid4 == 0){
            info(BEGIN, 4, 0);
            info(END, 4, 0);
            exit(1);
        }
        waitpid(pid4, 0, 0);

        pid5 = fork();
        if(pid5 == 0){
            info(BEGIN, 5, 0);
            info(END, 5, 0);
            exit(1);
        }
        waitpid(pid5, 0, 0);

        pid7 = fork();
        if(pid7==0){
            info(BEGIN, 7, 0);
            info(END, 7, 0);
            exit(1);
        }
        waitpid(pid7, 0, 0);

        info(END, 2, 0);
        exit(1);
    }      
    waitpid(pid2, 0, 0);
        

    pid3 = fork();
    if(pid3 == 0){
        info(BEGIN, 3, 0);

        pid6 = fork();
        if(pid6 == 0){
            info(BEGIN, 6, 0);

            for(int i =0;i<4;i++){
                thread[i].noThread = i;
                thread[i].logSem = logSem;
                if(pthread_create(&t6[i], NULL, thread_function, &thread[i]) != 0){
                    perror("Error creating thread!");
                    return -1;
                }
            }

            for(int i=0;i<4;i++){
                pthread_join(t6[i],NULL);
            }

            info(END, 6, 0);
            exit(1);
        }
        waitpid(pid6, 0, 0);
            
        sem_post(logSem);

        pid8 = fork();
        if(pid8 == 0){
            info(BEGIN, 8, 0);

            for(int i =0;i<4;i++){
                thread8[i].noThread = i;
                thread8[i].logSem = logSem2;
                if(pthread_create(&t8[i], NULL, thread_function2, &thread8[i]) != 0){
                    perror("Error creating thread!");
                    return -1;
                }
            }

            for(int i=0;i<4;i++){
                pthread_join(t8[i],NULL);
            }
            info(END, 8, 0);
            exit(1);
        }
        waitpid(pid8, 0, 0);
            
        info(END, 3, 0);
        exit(1);
    }

    waitpid(pid3, 0, 0); 

    
    info(END, 1, 0);

    sem_close(logSem);
    sem_close(logSem2);
    sem_destroy(&logSem3);
    sem_destroy(&cond);
    sem_destroy(&cond1);
    sem_destroy(&cond2);

    sem_unlink("sem1");

//    sem_close(logSem2);

    return 0;
}
