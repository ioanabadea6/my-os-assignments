#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void list(const char *path, int dim, char *nume)
{
    DIR *dir;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL)
    {
        printf("ERROR\n");
        printf("invalid directory path\n");
        return;
    }
    printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if (stat(filePath, &statbuf) == 0)
            {
                if (nume != NULL)
                {
                    if (strcmp(entry->d_name + strlen(entry->d_name) - strlen(nume), nume) == 0)
                    {
                        if (dim != 0)
                        {
                            if (dim < statbuf.st_size && S_ISREG(statbuf.st_mode))
                            {
                                printf("%s\n", filePath);
                            }
                        }
                        else
                        {
                            printf("%s\n", filePath);
                        }
                    }
                }
                else
                {
                    if (dim != 0)
                    {
                        if (dim < statbuf.st_size && S_ISREG(statbuf.st_mode))
                        {
                            printf("%s\n", filePath);
                        }
                    }
                    else
                    {
                        printf("%s\n", filePath);
                    }
                }
            }
        }
    }
    closedir(dir);
}

void list_recursive(const char *path, int dim, char *nume)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL)
    {
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if (stat(filePath, &statbuf) == 0)
            {
                if (S_ISDIR(statbuf.st_mode))
                {
                    list_recursive(filePath, dim, nume);
                }
                if (nume != NULL)
                {
                    if (strcmp(entry->d_name + strlen(entry->d_name) - strlen(nume), nume) == 0)
                    {
                        if (dim != 0)
                        {
                            if (dim < statbuf.st_size && S_ISREG(statbuf.st_mode))
                            {
                                printf("%s\n", filePath);
                            }
                        }
                        else
                        {
                            printf("%s\n", filePath);
                        }
                    }
                }
                else
                {
                    if (dim != 0)
                    {
                        if (dim < statbuf.st_size && S_ISREG(statbuf.st_mode))
                        {
                            printf("%s\n", filePath);
                        }
                    }
                    else
                    {
                        printf("%s\n", filePath);
                    }
                }
            }
        }
    }

    closedir(dir);
}

typedef struct SH
{
    int sect_size;
    int sect_offset;
    int sect_type;
    char sect_name[11];
} sh;

typedef struct SF
{
    char magic[5];
    int header_size;
    int no_of_sections;
    int version;
    sh *section_header;
} sf;

int parse(const char *path)
{
    int fd = -1;
    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        return -1;
    }

    sf *section_file = (sf *)malloc(sizeof(sf));

    lseek(fd, -6, SEEK_END);
    read(fd, &section_file->header_size, 2);
    read(fd, section_file->magic, 4);
    section_file->magic[4] = 0;

    if (strcmp(section_file->magic, "rB4K") != 0)
    {
        printf("ERROR\n");
        printf("wrong magic\n");
        return -1;
    }

    lseek(fd, -(section_file->header_size), SEEK_END);
    read(fd, &section_file->version, 4);
    if (section_file->version < 65 || section_file->version > 122)
    {
        printf("ERROR\n");
        printf("wrong version\n");
        return -1;
    }

    read(fd, &section_file->no_of_sections, 1);
    if (section_file->no_of_sections < 2 || section_file->no_of_sections > 19)
    {
        printf("ERROR\n");
        printf("wrong sect_nr\n");
        return -1;
    }

    section_file->section_header = (sh *)malloc(sizeof(sh) * section_file->no_of_sections);

    for (int i = 0; i < section_file->no_of_sections; i++)
    {
        read(fd, section_file->section_header[i].sect_name, 10);
        section_file->section_header[i].sect_name[10] = 0;
        read(fd, &section_file->section_header[i].sect_type, 4);
        if (section_file->section_header[i].sect_type != 12 && section_file->section_header[i].sect_type != 67)
        {
            printf("ERROR\n");
            printf("wrong sect_types\n");
            return -1;
        }
        read(fd, &section_file->section_header[i].sect_offset, 4);
        read(fd, &section_file->section_header[i].sect_size, 4);
    }

    printf("SUCCESS\n");
    printf("version=%d\n", section_file->version);
    printf("nr_sections=%d\n", section_file->no_of_sections);

    for (int i = 0; i < section_file->no_of_sections; i++)
    {
        printf("section%d: %s %d %d\n", i + 1, section_file->section_header[i].sect_name, section_file->section_header[i].sect_type, section_file->section_header[i].sect_size);
    }

    free(section_file->section_header);
    free(section_file);

    close(fd);
    return 0;
}

void findall(const char *path)
{
    DIR *dir;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL)
    {
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {

            snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if (stat(filePath, &statbuf) == 0)
            {

                if (S_ISDIR(statbuf.st_mode))
                {
                    findall(filePath);
                }
                else if (S_ISREG(statbuf.st_mode))
                {
                    int nr_sec;
                    int fd = -1;
                    fd = open(filePath, O_RDONLY);
                    if (fd == -1)
                    {
                        return;
                    }

                    sf *section_file = (sf *)malloc(sizeof(sf));

                    lseek(fd, -6, SEEK_END);
                    read(fd, &section_file->header_size, 2);
                    read(fd, section_file->magic, 4);
                    if (strcmp(section_file->magic, "rB4K") != 0)
                    {
                        printf("ERROR\n");
                        printf("invalid directory path\n");
                        return;
                    }

                    lseek(fd, -(section_file->header_size), SEEK_END);
                    read(fd, &section_file->version, 4);
                    /* if (section_file->version < 65 || section_file->version > 122)
                     {
                         printf("ERROR\n");
                         printf("invalid directory path\n");
                         return;
                     }
 */
                    read(fd, &section_file->no_of_sections, 1);

                    /*  if (section_file->no_of_sections < 2 || section_file->no_of_sections > 19)
                      {
                          printf("ERROR\n");
                          printf("invalid directory path\n");
                          return;
                      }*/

                    section_file->section_header = (sh *)malloc(sizeof(sh) * section_file->no_of_sections);
                    printf("da\n");
                    for (int i = 0; i < section_file->no_of_sections; i++)
                    {
                        read(fd, section_file->section_header[i].sect_name, 10);
                        section_file->section_header[i].sect_name[10] = 0;
                        read(fd, &section_file->section_header[i].sect_type, 4);
                        /*if (section_file->section_header[i].sect_type != 12 && section_file->section_header[i].sect_type != 67)
                        {
                            printf("ERROR\n");
                            printf("invalid directory path\n");
                            return;
                        }*/
                        read(fd, &section_file->section_header[i].sect_offset, 4);
                        read(fd, &section_file->section_header[i].sect_size, 4);
                    }

                    for (int i = 0; i < section_file->no_of_sections; i++)
                    {
                        char *sir = (char *)malloc(sizeof(char) * section_file->section_header[i].sect_size);
                        int nr_linii = 1;

                        lseek(fd, section_file->section_header[i].sect_offset, SEEK_SET);
                        read(fd, sir, section_file->section_header[i].sect_size);
                        for (int i = 0; i < section_file->section_header[i].sect_size; i++)
                        {
                            if (sir[i] == '\n')
                            {
                                nr_linii++;
                            }
                        }
                        if (nr_linii > 14)
                        {
                            nr_sec++;
                        }
                    }

                    if (nr_sec > 0)
                    {
                        printf("%s\n", filePath);
                    }

                    free(section_file->section_header);
                    free(section_file);

                    close(fd);
                }
            }
        }
    }

    closedir(dir);
}

void extract(const char *path, int sectiune, int linie)
{
    int fd = -1;

    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERRROR\n");
        printf("invalid file\n");
        return;
    }

    sf *section_file = (sf *)malloc(sizeof(sf));

    lseek(fd, -6, SEEK_END);
    read(fd, &section_file->header_size, 2);
    read(fd, section_file->magic, 4);

    if (strcmp(section_file->magic, "rB4K") != 0)
    {
        return;
    }

    lseek(fd, -(section_file->header_size), SEEK_END);
    read(fd, &section_file->version, 4);

    if (section_file->version < 65 || section_file->version > 122)
    {
        return ;
    }

    read(fd, &section_file->no_of_sections, 1);

    if (sectiune < 0 || sectiune > section_file->no_of_sections)
    {
        printf("ERROR\n");
        printf("wrong sect_nr\n");
        return;
    }

    section_file->section_header = (sh *)malloc(sizeof(sh) * section_file->no_of_sections + 1);

    for (int i = 0; i < section_file->no_of_sections; i++)
    {

        read(fd, section_file->section_header[i].sect_name, 10);
        section_file->section_header[i].sect_name[10] = 0;
        read(fd, &section_file->section_header[i].sect_type, 4);
        if (section_file->section_header[i].sect_type != 12 && section_file->section_header[i].sect_type != 67)
        {
            return;
        }
        read(fd, &section_file->section_header[i].sect_offset, 4);
        read(fd, &section_file->section_header[i].sect_size, 4);
    }

    char *sir = (char *)malloc(sizeof(char) * section_file->section_header[sectiune - 1].sect_size);
    int nr_linii = 1;

    lseek(fd, section_file->section_header[sectiune - 1].sect_offset, SEEK_SET);
    read(fd, sir, section_file->section_header[sectiune - 1].sect_size);
    for (int i = 0; i < section_file->section_header[sectiune - 1].sect_size; i++)
    {
        if (sir[i] == '\n')
        {
            nr_linii++;
        }
    }

    if (linie > nr_linii)
    {
        printf("ERROR\n");
        printf("invalid line\n");
        return;
    }

    printf("SUCCESS\n");

    int nr = 1;
    for (int i = section_file->section_header[sectiune - 1].sect_size; i > 0; i--)
    {
        if (sir[i] == '\n')
        {
            nr++;
        }
        if (nr == linie)
        {
            printf("%c", sir[i - 1]);
        }
    }

    free(section_file->section_header);
    free(section_file);

    close(fd);
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("95788\n");
        }

        if (strcmp(argv[1], "list") == 0)
        {
            char *path, *nume;
            int dim;
            int recursive = 0;
            for (int i = 2; i < argc; i++)
            {
                if (strncmp(argv[i], "path=", 5) == 0)
                {
                    path = (char *)malloc(sizeof(char) * (strlen(argv[i]) - 5));
                    strcpy(path, argv[i] + 5);
                }
                if (strncmp(argv[i], "size_greater=", 13) == 0)
                {
                    dim = atoi(argv[i] + 13);
                }
                if (strncmp(argv[i], "name_ends_with=", 15) == 0)
                {
                    nume = (char *)malloc(sizeof(char) * strlen(argv[i]));
                    strcpy(nume, argv[i] + 15);
                }
                if (strcmp(argv[i], "recursive") == 0)
                {
                    recursive = 1;
                }
            }

            if (recursive)
            {
                struct stat statbuf1;
                if (lstat(path, &statbuf1) == 0)
                {
                    if (S_ISDIR(statbuf1.st_mode))
                    {
                        printf("SUCCESS\n");
                        list_recursive(path, dim, nume);
                    }
                    else
                    {
                        printf("ERROR\n");
                        printf("invalid directory path\n");
                    }
                }
            }
            else
            {
                list(path, dim, nume);
            }

            free(path);
            free(nume);
        }

        if (strcmp(argv[1], "parse") == 0)
        {
            char *path;
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                path = (char *)malloc(sizeof(char) * (strlen(argv[2]) - 5));
                strcpy(path, argv[2] + 5);
            }
            parse(path);
            free(path);
        }

        if (strcmp(argv[1], "extract") == 0)
        {
            char *path;
            int sectiune;
            int linie;
            for (int i = 2; i < argc; i++)
            {
                if (strncmp(argv[i], "path=", 5) == 0)
                {
                    path = (char *)malloc(sizeof(char) * (strlen(argv[i]) - 5));
                    strcpy(path, argv[i] + 5);
                }
                if (strncmp(argv[i], "section=", 8) == 0)
                {
                    sectiune = atoi(argv[i] + 8);
                }
                if (strncmp(argv[i], "line=", 5) == 0)
                {
                    linie = atoi(argv[i] + 5);
                }
            }

            extract(path, sectiune, linie);

            free(path);
        }

        if (strcmp(argv[1], "findall") == 0 && strncmp(argv[2], "path=", 5) == 0)
        {
            char *path = (char *)malloc(sizeof(char) * (strlen(argv[2]) - 5));
            strcpy(path, argv[2] + 5);
            struct stat statbuf1;
            if (lstat(path, &statbuf1) == 0)
            {
                if (S_ISDIR(statbuf1.st_mode))
                {
                    printf("SUCCESS\n");
                    findall(path);
                }
                else
                {
                    printf("ERROR\n");
                    printf("invalid directory path\n");
                }
            }
            free(path);
        }
    }
    return 0;
}
