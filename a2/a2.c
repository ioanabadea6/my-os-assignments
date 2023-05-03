#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>

#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int noThread;
    sem_t *logSem;
} TH_STRUCT;

void *thread_function(void *argc){
    TH_STRUCT *s = (TH_STRUCT*) argc;
    switch (s->noThread){
        case 0: 
                sem_wait(s->logSem);
                break;
        case 1: 
                info(BEGIN, 6, 2);
                info(END, 6, 2);
                break;
        case 2: 
                info(BEGIN, 6, 3);
                info(END, 6, 3);
                break;
        case 3: 
                info(BEGIN, 6, 1);  
                info(BEGIN, 6, 4); 
                sem_post(s->logSem);           
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

int main(){
    pid_t pid2 = -1, pid3 = -1, pid4 = -1, pid5 = -1, pid6 = -1, pid7 = -1, pid8 = -1;
   
    sem_t logSem;
    TH_STRUCT thread[4];
    pthread_t t6[4];

    init();

    if(sem_init(&logSem, 0, 1) != 0){
        return -1;
    }

    info(BEGIN, 1, 0);
  
    pid2 = fork();  
    if(pid2 == 0){
        info(BEGIN, 2, 0);

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
                thread[i].logSem = &logSem;
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
            

        pid8 = fork();
        if(pid8 == 0){
            info(BEGIN, 8, 0);
            info(END, 8, 0);
            exit(1);
        }
        waitpid(pid8, 0, 0);
            
        info(END, 3, 0);
        exit(1);
    }

    waitpid(pid3, 0, 0); 

    
    info(END, 1, 0);

    sem_destroy(&logSem);

    return 0;
}
