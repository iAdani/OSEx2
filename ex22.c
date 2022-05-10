// Guy Adani 208642884

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

#define WRITETONEWFILEEND O_WRONLY | O_APPEND | O_CREAT | O_TRUNC
#define MAXSIZE 150

// TODO: close open files
// Closes the files
void closeFiles() {

}

// Returns error
void writeError(char* errStr) {
    char str[11 + MAXSIZE] = "Error in: ";
    strcat(str, errStr);
    strcat(str, "\n");
    write(2, str, strlen(str));
}

void writeStr(char* str) {
    
}

int main(int argc, char *argv[]) {
    int configurationFD;
    char configuraionContent[3][MAXSIZE];
    char c;
    DIR *dip;
    struct dirent *dit;
    struct stat dis;

    if ((dup2((open("./error.txt", WRITETONEWFILEEND, 0644)) , 2)) == -1){
        writeError("dup2");
    }

    // Open the configuration file
    if((configurationFD = open(argv[1], O_RDONLY)) == -1) {
        writeError("open");
        return -1;
    }

    // Read the configuration file content
    int i, j;
    for (i = 0; i < 3; i++) {
        j = 0;
        while ((read(configurationFD, &c, 1)) != 0) {
            if (c == '\n') {
                configuraionContent[i][j++] = '\0';
                break;
            }
            configuraionContent[i][j++] = c;
        }
    }

    // Check the configuration file data
    if (stat(configuraionContent[0], &dis)) {
        write(2, "Not a valid directory\n", 22);
        return -1;
    }
    if (!S_ISDIR(dis.st_mode)) {
        write(2, "Not a valid directory\n", 22);
        return -1;
    }
    if (access(configuraionContent[1], F_OK) != 0) {
        write(2, "Input file not exist\n", 21);
        return -1;
    }
    if (access(configuraionContent[2], F_OK) != 0) {
        write(2, "Output file not exist\n", 22);
        return -1;
    }


    // Open stream to the directory from the configuration file
    if((dip = opendir(configuraionContent[0])) == NULL) {
        writeError("opendir");
        return -1;
    }

    while((dit = readdir(dip)) != NULL) {
        if(!strcmp(dit->d_name, ".") || !strcmp(dit->d_name, "..")) continue;
        char path[MAXSIZE];
        strcpy(path, configuraionContent[0]);
        strcat(path, "/");
        strcat(path, dit->d_name);
        stat(path, &dis);

        if (!S_ISDIR(dis.st_mode)) {
            continue;
        }




    }

    // Closes the stream
    if(closedir(dip) == -1) {
        writeError("closedir");
        return -1;
    }
    return 0;
}