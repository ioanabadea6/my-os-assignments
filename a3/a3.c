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

            unsigned char c;
            unsigned char buffer[250];
            unsigned int i=0;
            int shmFD=0;
            char *data = NULL;
            unsigned int size_shm = 0; 
            unsigned int offset = 0;
            unsigned int value = 0;
            char file_name[250];
            int fd_file_name = -1;
            char *data2 = NULL;
            int size2;
            unsigned int offset2 = 0;
            unsigned int no_of_bytes = 0;
            unsigned int section_no = 0;
            unsigned int offset3 = 0;
            unsigned int no_of_bytes3 = 0;

        for(;;){
            i=0;

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
                read(fd2, &size_shm, sizeof(unsigned int));
                shmFD = shm_open("/iljVHa", O_CREAT | O_RDWR, 0664);
                if(shmFD == -1){
                    write(fd1, "CREATE_SHM!", strlen("CREATE_SHM!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                    return 1;
                }
                ftruncate(shmFD, size_shm);
                data = (char*)mmap(0, size_shm, PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
                if(data == (void*)-1){
                    write(fd1, "CREATE_SHM!", strlen("CREATE_SHM!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                    return 1;
                }
                write(fd1, "CREATE_SHM!", strlen("CREATE_SHM!"));
                write(fd1, "SUCCESS!", strlen("SUCCESS!"));
            }
            else if(strcmp((char*)buffer, "WRITE_TO_SHM") == 0){  
                read(fd2, &offset, sizeof(offset)); 
                read(fd2, &value, sizeof(value));
                if(offset < 0 || offset > size_shm || size_shm < offset + 4){
                    write(fd1, "WRITE_TO_SHM!", strlen("WRITE_TO_SHM!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                }
                memcpy(data+offset, &value, sizeof(value));
                write(fd1, "WRITE_TO_SHM!", strlen("WRITE_TO_SHM!"));
                write(fd1, "SUCCESS!", strlen("SUCCESS!"));              
            }
            else if(strcmp((char*)buffer, "MAP_FILE")==0){
                read(fd2, &file_name, strlen(file_name));
                file_name[strlen(file_name)]='\0';
                fd_file_name = open(file_name, O_RDONLY);
                if(fd_file_name == -1){
                    write(fd1, "MAP_FILE!", strlen("MAP_FILE!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                }
                size2=lseek(fd_file_name, 0, SEEK_END);
                data2 = (char*)mmap(NULL, size2, PROT_READ, MAP_SHARED, fd_file_name, 0);
                if(data2 == (void*)-1){
                    write(fd1, "MAP_FILE!", strlen("MAP_FILE!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                }
                write(fd1, "MAP_FILE!", strlen("MAP_FILE!"));
                write(fd1, "SUCCESS!", strlen("SUCCESS!"));
            }
            else if(strcmp((char*)buffer, "READ_FROM_FILE_OFFSET") == 0){
                read(fd2, &offset2, sizeof(offset));
                read(fd2, &no_of_bytes, sizeof(no_of_bytes));
                if(data2 == (void*)-1 || shmFD == -1 || offset2 + no_of_bytes > size2){
                    write(fd1, "READ_FROM_FILE_OFFSET!", strlen("READ_FROM_FILE_OFFSET!"));
                    write(fd1, "ERROR!", strlen("ERROR!"));
                }
                int i=0;
                for(int m=offset;m<offset+no_of_bytes;m++){
                    data[i]=data2[m];
                    i++;
                }
                write(fd1, "READ_FROM_FILE_OFFSET!", strlen("READ_FROM_FILE_OFFSET!"));
                write(fd1, "SUCCESS!", strlen("SUCCESS!"));
            }
            else if(strcmp((char*)buffer, "READ_FROM_FILE_SECTION")==0){
                read(fd1, &section_no, sizeof(section_no));
                read(fd1, &offset3, sizeof(offset));
                read(fd1, &no_of_bytes3, sizeof(no_of_bytes3));
               //  if(section_no < 0)
                write(fd1, "READ_FROM_FILE_SECTION!", strlen("READ_FROM_FILE_SECTION!"));
                write(fd1, "SUCCESS!", strlen("SUCCESS!"));
            }
            else if(strcmp((char*)buffer, "EXIT") == 0){   
                munmap((void*)data, sizeof(char));
                munmap((void*)data2, sizeof(char));
                data = NULL;
                data2 = NULL;
                close(fd2);
                close(fd1);
                close(fd_file_name);
                unlink(PIPE1);
                shm_unlink("/iljVHa");
            }
            else{

                break;
            }

        }
    }


    return 0;
}