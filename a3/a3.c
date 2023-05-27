#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define PIPE1 "RESP_PIPE_95788"
#define PIPE2 "REQ_PIPE_95788"



int main(){

    if(mkfifo(PIPE1, 0600) != 0){
        printf("ERROR\n");
        printf("cannot create the response pipe\n");
        return 1;
    }

    int fd2 = -1;
    fd2 = open(PIPE2, O_RDONLY);

    int fd1 = -1;
    fd1 = open(PIPE1, O_WRONLY);
    if(fd2 == -1){
        printf("ERROR\n");
        printf("cannot open the request pipe\n");
        return 1;
    }
    else{
        printf("SUCCESS\n");

        write(fd1, "HELLO!", strlen("HELLO!"));

      
        for(;;){
            unsigned char c;
            unsigned char buffer[250];
            unsigned int i=0;
            int shmFD=0;
            volatile char *data = NULL;
            unsigned int size_shm = 4899052; 

            read(fd2, &c, sizeof(c));
            while(c != '!'){
                buffer[i] =c;
                i++;
                read(fd2, &c, sizeof(c));
            }
            buffer[i]='\0';

            if(strcmp((char*)buffer, "ECHO") == 0){
                write(fd1, "ECHO!", strlen("ECHO!"));
                write(fd1, "VARIANT!", strlen("VARIANT!"));
                unsigned int nr = 95788;
                write(fd1, &nr, sizeof(nr));
            }
            else if(strcmp((char*)buffer,"CREATE_SHM") == 0){
               // read(fd2, &size_shm, sizeof(unsigned int));
                shmFD = shm_open("/iljVHa", O_CREAT | O_RDWR, 0664);
                if(shmFD == -1){
                    write(fd1, "CREATE_SHM!", strlen("CREATE_SHM!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                }
                ftruncate(shmFD, size_shm);
                data = (char*)mmap(NULL, size_shm, PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
                if(data == (void*)-1){
                    write(fd1, "CREATE_SHM!", strlen("CREATE_SHM!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                }
                write(fd1, "CREATE_SHM!", strlen("CREATE_SHM!"));
                write(fd1, "SUCCESS!", strlen("SUCCESS!"));

            }
            else if(strcmp((char*)buffer, "EXIT") == 0){
                munmap((void*)data, size_shm);
                data = NULL;
                close(fd2);
                close(fd1);
                unlink(PIPE1);
                shm_unlink("/iljVHa");
                //break;
            }
            else{
                break;
            }
        }
    }


    return 0;
}