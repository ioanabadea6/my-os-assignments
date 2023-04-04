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
        return ;
    }
    printf("SUCCESS\n");
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") != 0){
            snprintf(filePath, 512,"%s/%s\n", path, entry->d_name);
            if(stat(filePath, &statbuf) == 0 && S_ISREG(statbuf.st_mode) && dim < statbuf.st_size){
                printf("%s\n", filePath);       
            }
            if(strcmp(entry->d_name + strlen(entry->d_name)-strlen(nume), nume)==0  || nume == NULL){
                printf("%s\n", filePath);
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
        return;
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
                else{
                    printf("%s\n", filePath);
                }
            }
        }
    }
    
    closedir(dir);   
}


void findall(const char *path){
  /*  DIR *dir;
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
            snprintf(filepath, 512,"%s/%s", path, entry->d_name);
            if(lstat(filePath, %statbuf) == 0){
                if()
            }
        }
    }*/
   // closedir(dir);
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

int parse(int fd){
    char *sect_type;
    int sect_offset;
    int sect_size;
    int sect_nr;

    char nume[10];
    char *magic;
    int version;
    int sectiuni;
    int tip_sectiune;
    int offset;
    int size;

    lseek(fd, -6, SEEK_END);
    read(fd, &magic,4);                
    if(strcmp(magic,"rB4K")==0){
        printf("ERROR\n");
        printf("wrong magic\n");
        return -1;
    }

    //il punem dupa magic
    lseek(fd, -2, SEEK_CUR);
    read(fd, &version,1);
    if(version<65 || version>112){
        printf("ERROR\n");
        printf("wrong version\n");
        return -1;
    }

    //la inceputul sectiunii
    lseek(fd, 0, SEEK_CUR);
    read(fd,&sectiuni,4);
    if(sectiuni<2 || sectiuni>19){
        printf("ERROR\n");
        printf("wrong sect_nr\n");
        return -1;
    } 

    for(int i=0;i<sectiuni;i++){
        read(fd, &size, 4);
        read(fd, &offset,4);
        read(fd, &tip_sectiune, 4);
        if(tip_sectiune!=12 || tip_sectiune!=67){
            printf("ERROR\n");
            printf("sect_types\n");
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
    
    return 0;
}

int main(int argc, char **argv){
    if(argc>=2){
        if(strcmp(argv[1],"variant")==0){
            printf("95788\n");
        }  

        if(strcmp(argv[1],"list")==0){ 
            // list simplu   
            if(strncmp(argv[2],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[2]-5)));
                int dim;
                char *nume;                 
                strcpy(path,argv[2]+5);
                nume=NULL;
                dim=0;
                list(path, dim, nume); 
                free(path);
                free(nume);    
            }

            //dimensiune
            if(strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[3],"path=", 5)==0) {
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[3])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*strlen(argv[2])); 
                strcpy(path,argv[3]+5);
                path = argv[3] + 5;
                dim = atoi(argv[2] +13);
                nume = NULL;
                list(path,dim,nume);
                free(path);
                free(nume);  
            } 

            //nume
            if(strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[3],"path=", 5)==0) {
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[3])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*strlen(argv[2])); 
                strcpy(path,argv[3]+5);
                dim = 0;
                strcpy(nume,argv[2] + 15);
                list(path,dim,nume);
                free(path);
                free(nume);  
            }   

        /*    if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[4],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[4])-15)); 
                strcpy(path,argv[5]+5);
                dim = atoi(argv[3] +13);
                strcpy(nume,argv[4] + 15);
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }
            
            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[4],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[3])-15)); 
                strcpy(path,argv[5]+5);
                dim = atoi(argv[4] +13);
                strcpy(nume,argv[3] + 15);
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[4],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[4])-15)); 
                strcpy(path,argv[5]+5);
                dim = atoi(argv[2] +13);
                strcpy(nume, argv[4] + 15);
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[4],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[3],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[3])-15)); 
                strcpy(path,argv[5]+5);
                dim = atoi(argv[2] +13);
                strcpy(nume, argv[3] + 15);
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[4],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[2])-15)); 
                strcpy(path,argv[5]+5);
                dim = atoi(argv[4] +13);
                strcpy(nume,argv[2] + 15);
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[4],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[5],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[2])-15)); 
                strcpy(path,argv[5]+5);
                dim = atoi(argv[3] +13);
                strcpy(nume,argv[2] + 15);
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"size_greater=", 13)==0 && strncmp(argv[4],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[4])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[4])-15)); 
                strcpy(path,argv[4]+5);
                dim = atoi(argv[3] +13);
                nume =NULL;
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"size_greater=", 13)==0 && strncmp(argv[4],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[4])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[4])-15)); 
                strcpy(path,argv[4]+5);
                dim = atoi(argv[2] +13);
                nume =NULL;
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[2],"recursive")==0 && strncmp(argv[3],"name_ends_with=", 15)==0 && strncmp(argv[4],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[5])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[3])-15)); 
                strcpy(path,argv[4]+5);
                dim = 0;
                nume =argv[3] + 15;
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }

            if(strcmp(argv[3],"recursive")==0 && strncmp(argv[2],"name_ends_with=", 15)==0 && strncmp(argv[4],"path=", 5)==0){
                char *path=(char*)malloc(sizeof(char)*(strlen(argv[4])-5));
                int dim;
                char *nume=(char*)malloc(sizeof(char)*(strlen(argv[2])-15)); 
                strcpy(path,argv[4]+5);
                dim = 0;
                nume =argv[2] + 15;
                char filePath1[512];
                struct stat statbuf1;
                if(lstat(filePath1, &statbuf1) == 0){
                    if(S_ISDIR(statbuf1.st_mode)==0){
                        list_recursive(path,dim,nume);
                    }
                    else{
                        printf("ERROR\n");
                        printf("invalid directory path\n");                        
                    }
                }
                free(path);
                free(nume);  
            }
            */
        }     
        
        if(strcmp(argv[1],"parse")==0){
            char *path=(char*)malloc(sizeof(char)*(strlen(argv[2])-5)); 
            strcpy(path,argv[2]+5);

            int fd =-1;
            fd = open(path,O_RDONLY);
            if(fd < 0){
                printf("ERROR\n");
                return -1;
            }
            else{
            parse(fd);
            }
            close(fd);
        }
/*
        if(strcmp(argv[1],"extract")==0 && strncmp(argv[3],"section=", 8)==0 && strncmp(argv[4],"line=", 5) && strncmp(argv[2],"path",5)){
            path = argv[2] + 5;
            int sectiune = atoi(argv[3] +8);
            int linie = atoi(argv[4]+5);
            extract(path, sectiune, linie);
        }

        if(strcmp(argv[1],"findall")==0){
            path = argv[2] + 5;
            findall(path);
        }  */ 
    }
    return 0;
}

