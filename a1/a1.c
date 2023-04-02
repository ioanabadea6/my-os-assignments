#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void list(const char *path, int dim, char *nume, char *arg){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

  //  printf("SUCCESS\n");
    dir = opendir(path);
    if(dir == NULL){
        perror("invalid directory path");
    }
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") == 0){
            if(lstat(filePath,&statbuf)==0){
                if(strncmp(arg, nume, strlen(entry->d_name)-strlen(nume))==0 || nume == NULL){
                    if(S_ISREG(statbuf.st_mode) && dim > statbuf.st_size){
                        printf("%s\n", filePath);
                    }
                    printf("%s\n", filePath);
                }
            }
        }
    }
    closedir(dir);    
}

void list_recursive(const char *path, int dim, char *nume, char *arg){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

  //  printf("SUCCESS\n");
    dir = opendir(path);
    if(dir == NULL){
        perror("invalid directory path");
    }
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") == 0){
            if(lstat(filePath,&statbuf)==0){
                if(S_ISREG(statbuf.st_mode) && dim > statbuf.st_size){
                    if(strncmp(arg, nume, strlen(entry->d_name)-strlen(nume))==0)
                        printf("%s\n", filePath);
                }else{
                    if(S_ISDIR(statbuf.st_mode)){
                        list_recursive(filePath,dim,nume,arg);
                    }
                }
            }
        }
    }
    closedir(dir);    
}

int main(int argc, char **argv){
    if(argc>=2){
        if(strcmp(argv[1],"variant")==0){
            printf("95788\n");
        }  
        char *path;
        int dim;
        char *nume;
       // struct stat s_buf;
        
        if(strcmp(argv[1],"list")==0){       
            if(strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0) {
                path = argv[4] + 5;
                dim = atoi(argv[4] +13);
                nume =argv[3] + 15;
             //   if(stat(path, &s_buf)>=0 && S_ISDIR(s_buf.st_mode)){
             //       printf("SUCCESS");
                    list(path,dim,nume,argv[3]);
             //   }
            }   
            if(strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0) {
                path = argv[4] + 5;
                dim = atoi(argv[3] +13);
                nume =argv[2] + 15;
                list(path,dim,nume, argv[2]);
            }  
            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[4],"name_ends_with=", 15)==0){
                path = argv[5] + 5;
                dim = atoi(argv[3] +13);
                nume =argv[4] + 15;
                list_recursive(path,dim,nume, argv[4]);
            }
            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[4],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0){
                path = argv[5] + 5;
                dim = atoi(argv[4] +13);
                nume =argv[3] + 15;
                list_recursive(path,dim,nume, argv[3]);
            }
           if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[4],"name_ends_with=", 15)==0){
                path = argv[5] + 5;
                dim = atoi(argv[2] +13);
                nume =argv[4] + 15;
                list_recursive(path,dim,nume, argv[4]);
            }
            if(strcmp(argv[4],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0){
                path = argv[5] + 5;
                dim = atoi(argv[2] +13);
                nume =argv[3] + 15;
                list_recursive(path,dim,nume,argv[3]);
            }
            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[4],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0){
                path = argv[5] + 5;
                dim = atoi(argv[4] +13);
                nume =argv[2] + 15;
                list_recursive(path,dim,nume,argv[2]);
            }
            if(strcmp(argv[4],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0){
                path = argv[5] + 5;
                dim = atoi(argv[3] +13);
                nume =argv[2] + 15;
                list_recursive(path,dim,nume,argv[2]);
            }
        }     
        
        if(strcmp(argv[1],"parse")==0){
            char *path = argv[2]+5;
            int fd = -1;
            fd = open(path,O_RDONLY);
            if(fd < 0){
                printf("ERROR\n");
                printf("invalid path\n");
            } else {
                char nume[10];
                char *magic;
                int version;
                int sectiuni;
                int tip_sectiune;
                int offset;
                int size;

                read(fd, &magic,4);
                read(fd, &version,1);
                read(fd,&sectiuni,4);
                
                if(strcmp(magic,"rB4K")==0){
                    printf("ERROR\n");
                    printf("wrong magic");
                }
                if(version<65 || version>112){
                    printf("ERROR\n");
                    printf("version");
                }
                if(sectiuni<2 || sectiuni>19){
                    printf("ERROR\n");
                    printf("sect_nr");
                } 

                for(int i=0;i<sectiuni;i++){
                    read(fd,&nume, 10);
                    read(fd, &tip_sectiune,4);
                    if(tip_sectiune!=12 || tip_sectiune!=67){
                        printf("ERROR\n");
                        printf("sect_types");
                    }
                    read(fd,&offset, 4);
                    read(fd, &size,4);
                }
                            
                char *sect_type;
                int sect_offset;
                int sect_size;
                int sect_nr;

                printf("SUCCESS\n");
                printf("version=%d\n", version);
                read(fd, &sect_nr, 1);
                printf("nr_section=%d\n", sect_nr);

                for(int i=0;i<sect_nr;i++){
                    read(fd, &sect_type, 4);
                    read(fd, &sect_offset, 4);
                    read(fd, &sect_size, 4);
                    printf("section%d: %s %d %d\n", i+1, sect_type, sect_offset, sect_size);
                           
                }
                close(fd);
            }
        }
        if(strcmp(argv[1],"extract")==0){
 /*            char *path = argv[2] + 5;
            int section = argv[3] + 8;
            int line = argv[4] + 5;
            int fd =-1;
            int nr_linii;
            int nr_sec;
            int i =0;
            char *linie;
            fd = open(path, O_RDONLY);
            if(fd<0){
                printf("ERROT\ninvalid file|section|line\n");
                return 0;
            }
            else{
               while(read(fd, %linie[i],1)>0){
                    if(linie[i] == '\n'){
                        nr_linii++;
                    }
                }
*/
           // }
       //     close(fd);
        }
        if(strcmp(argv[1],"findall")==0){
          //  invalid(argv[2]);
        }   
    }
    return 0;
}
