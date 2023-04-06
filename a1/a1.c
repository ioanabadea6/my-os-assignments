#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void list(const char *path, int dim, char *nume){
    DIR *dir;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL){
        printf("ERROR\n");
        printf("invalid directory path\n");
    }else{
        printf("SUCCESS\n");
    }
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") != 0){
            snprintf(filePath, 512,"%s/%s\n", path, entry->d_name);
            if(lstat(filePath, &statbuf)==0){
                if((S_ISREG(statbuf.st_mode) && dim < statbuf.st_size) || dim == 0){
                    if(strcmp(entry->d_name + strlen(entry->d_name)-strlen(nume), nume)==0  || nume ==NULL)
                        printf("%s\n", filePath);
                }
            }
        }
    }
    closedir(dir);  
}

void list_recursive(const char *path, int dim, char *nume){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;


    dir = opendir(path);
    if(dir == NULL){
        printf("ERROR\n");
        printf("invalid directory path");
        return;
    }
    else{
        printf("SUCCESS\n");
    }
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") != 0){
            snprintf(filePath, 512,"%s/%s", path, entry->d_name);
            if(lstat(filePath, &statbuf) == 0){
                if((S_ISREG(statbuf.st_mode) && dim < statbuf.st_size) || dim == 0){
                    if(strcmp(entry->d_name + strlen(entry->d_name)-strlen(nume), nume)==0  || nume ==NULL)
                        printf("%s\n", filePath);
                    if(S_ISDIR(statbuf.st_mode)){
                        list_recursive(filePath,dim,nume);
                    }
                
                }
            }
        }
    }
    closedir(dir);    
}

void findall(const char *path){
    DIR *dir;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL){
        printf("ERROR\n");
        printf("invalid directory path");
    }
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
            if(lstat(filePath,&statbuf)==0){

            }
        }
    }
    closedir(dir);
}

void extract(const char *path, int sectiune, int linie){
 /*   int fd =-1;
    int nr_linii;
    //int nr_sec;
    int size;
    int l=0;
    char c;
    int crtPos;

    fd = open(path, O_RDONLY);
    if(fd<0){
        printf("ERROR\ninvalid file|section|line\n");
        return;
    }
    else{
        printf("SUCCESS");
        size = lseek(fd, 0, SEEK_END);
        lseek(fd,0,SEEK_SET);
        for(int i = 0;i<size;i++){
            if(read(fd,&c,1) !=1){
                break;
            }
            if(c=="\n"){
                crtPos = lseek(fd, 0, SEEK_CUR);
                nr_linii++;
                lseek(fd ,crtPos, SEEK_SET);
            }
        }
        if(linie<nr_linii){
            for(int i=size-2;i>0;i--){
                crtPos=lseek(fd, i-size, SEEK_END);
                if(read(fd, &c, 1) != 1){
                    break;
                }
                if(c=="\n"){
                    if(l<linie)
                    printf("%s", c);
                    if(c==" "){
                        printf(" ");
                    }
                    lseek(fd, crtPos, SEEK_SET);
                }
                l++;
            }
        }
        else{
            printf("ERROR\ninvalid line");
        }
    }
    close(fd);*/
}

int parse(const char *path){
    char nume[10];
    char *magic;
    int version;
    int sectiuni;
    int tip_sectiune;
    int offset;
    int size;

    char *sect_type;
    int sect_offset;
    int sect_size;
    int sect_nr;

    int fd = -1;
    fd = open(path,O_RDONLY);
    if(fd < 0){
        printf("ERROR\n");
        printf("invalid path\n");
        return -1;
    }
    else {
        lseek(fd, -6, SEEK_END);
        read(fd, &magic,4);
        lseek(fd, -2, SEEK_CUR);
        read(fd, &version,1);
        lseek(fd, 0, SEEK_CUR);
        read(fd,&sectiuni,4);
                
        if(strcmp(magic,"rB4K")==0){
            printf("ERROR\n");
            printf("wrong magic\n");
            return -1;
        }
        if(version<65 || version>112){
            printf("ERROR\n");
            printf("version\n");
            return -1;
        }
        if(sectiuni<2 || sectiuni>19){
            printf("ERROR\n");
            printf("sect_nr\n");
            return -1;
        } 

        for(int i=0;i<sectiuni;i++){
            read(fd, &size, 4);
            read(fd, &offset,4);
            read(fd, &tip_sectiune, 4);
                if(tip_sectiune!=12 || tip_sectiune!=67){
                printf("ERROR\n");
                printf("sect_types");
                return -1;
            }
            read(fd, &nume, 10);
        }
                            

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
    return 0;
}

int main(int argc, char **argv){
    if(argc>=2){
        if(strcmp(argv[1],"variant")==0){
            printf("95788\n");
        }  
        char *path;
        int dim;
        char *nume; 
        
        if(strcmp(argv[1],"list")==0){ 
            // list simplu   
            if(strncmp(argv[2],"path=", 5)==0){
                path=argv[2]+5;
                nume=NULL;
                dim=0;
                list(path, dim, nume);             
            }

            //dimensiune
            if(strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[3],"path=", 5)==0) {
                path = argv[3] + 5;
                dim = atoi(argv[2] +13);
                nume = NULL;
                list(path,dim,nume);
            } 

            //nume
            if(strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[3],"path=", 5)==0) {
                path = argv[3] + 5;
                dim = 0;
                nume =argv[2] + 15;
                list(path,dim,nume);
            }   

            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[4],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                path = argv[5] + 5;
                dim = atoi(argv[3] +13);
                nume =argv[4] + 15;
                list_recursive(path,dim,nume);
            }
            
            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[4],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                path = argv[5] + 5;
                dim = atoi(argv[4] +13);
                nume =argv[3] + 15;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[4],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                path = argv[5] + 5;
                dim = atoi(argv[2] +13);
                nume =argv[4] + 15;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[4],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                path = argv[5] + 5;
                dim = atoi(argv[2] +13);
                nume =argv[3] + 15;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[4],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                path = argv[5] + 5;
                dim = atoi(argv[4] +13);
                nume =argv[2] + 15;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[4],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                path = argv[5] + 5;
                dim = atoi(argv[3] +13);
                nume =argv[2] + 15;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[4],"path=", 5)==0){
                path = argv[4] + 5;
                dim = atoi(argv[3] +13);
                nume =NULL;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[4],"path=", 5)==0){
                path = argv[4] + 5;
                dim = atoi(argv[2] +13);
                nume =NULL;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"name_ends_with=", 15)==0 && strncmp(argv[4],"path=", 5)==0){
                path = argv[4] + 5;
                dim = 0;
                nume =argv[3] + 15;
                list_recursive(path,dim,nume);
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[4],"path=", 5)==0){
                path = argv[4] + 5;
                dim = 0;
                nume =argv[2] + 15;
                list_recursive(path,dim,nume);
            }
        }     
        
        if(strcmp(argv[1],"parse")==0){
            path = argv[2]+5;
            parse(path);
        }

        if(strcmp(argv[1],"extract")==0 && strncmp(argv[3],"section=", 8)==0 && strncmp(argv[4],"line=", 5) && strncmp(argv[2],"path",5)){
            path = argv[2] + 5;
            int sectiune = atoi(argv[3] +8);
            int linie = atoi(argv[4]+5);
            extract(path, sectiune, linie);
        }

        if(strcmp(argv[1],"findall")==0){
            path = argv[2] + 5;
            findall(path);
        }   
    }
    return 0;
}
