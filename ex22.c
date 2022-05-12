// Guy Adani 208642884

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>

#define WRITETONEWFILE O_WRONLY | O_CREAT | O_TRUNC | O_APPEND
#define MAXSIZE 150

// TODO: b.out

// Writes error
void writeError(char* errStr) {
    char str[11 + MAXSIZE] = "Error in: ";
    strcat(str, errStr);
    strcat(str, "\n");
    write(2, str, strlen(str));
}

// Writes to the csv file
void writeToCsv(int fd, int num, char* name) {
    char str[MAXSIZE];
    strcpy(str, name);
    switch (num) {
        case 3:
            strcat(str, ",75,SIMILAR\n");
            break;
        case 2:
            strcat(str, ",50,WRONG\n");
            break;
        case 1:
            strcat(str, ",100,EXCELLENT\n");
            break;
        case 0:
            strcat(str, ",0,NO_C_FILE\n");
            break;
        case -1:
            strcat(str, ",10,COMPILATION_ERROR\n");
            break;
        default: ;
    }
    if((write(fd, str, strlen(str))) == -1) {
        writeError("open");
    }
    
}

// Closes the file
void closeFiles(int inputFD[5]) {
    int i;
//    for(i = 0; i < 2; i++) closedir(inputFD[i]);
    for(i = 2; i < 5; i++){
        if(inputFD[i] != 0) close(inputFD[i]);
    }
}

// Compiles the c file in the given path
int compileFile (char* path, char* name) {
    strcat(path, "/");
    strcat(path, name);
    int childPD, retVal;
    if((childPD = fork()) < 0) writeError("fork");
    if (childPD == 0) {
        execlp("gcc", "gcc", "-o", "compiled.out", path, NULL);
        writeError("execlp");
        exit(-1);
    }
    wait(&retVal);
    return WEXITSTATUS(retVal);
}

//TODO: files error check

// Runs the executable file
int runFile(char* input) {
    int inputFD, outputFD, childPD;

    // Set I/O redirection
    if ((dup2((outputFD = open("output.txt", WRITETONEWFILE, 0644)) , 1)) == -1){
        writeError("dup2");
        return -1;
    }
    if ((dup2((inputFD = open(input, O_RDONLY)) , 0)) == -1) {
        writeError("dup2");
        return -1;
    }

    // Run
    if ((childPD = fork()) < 0) writeError("fork");
    if (childPD == 0) {
        execlp("./compiled.out", "./compiled.out", NULL);
        writeError("execlp");
        exit(-1);
    }
    wait(NULL);

    // Close files
    if(close(inputFD)) {
        writeError("close");
        return -1;
    }
    if(close(outputFD)) {
        writeError("close");
        return -1;
    }
}

// Check the output with comp.out
int checkOutput(char* correct){
    int childPD, retVal;
    if((childPD = fork()) < 0) writeError("fork");
    if (childPD == 0) {
        execlp("./comp.out", "./comp.out", "output.txt", correct , NULL);
        writeError("execlp");
        exit(-1);
    }
    wait(&retVal);
    return WEXITSTATUS(retVal);
}

int main(int argc, char *argv[]) {
    int configurationFD, resultsFD;                     // Config. and results file FD, errors FD
    int compResult, flag;                               // result of comp, flag for csv
    char configuraionContent[3][MAXSIZE];               // The input from config. file
    char c;                                             // Used to read config. file
    DIR *dirStr, *inDirStr;                             // The dir streams
    struct dirent *dit, *inDit;                         // The dir iterators
    struct stat dis;                                    // Used to check the inputs
    char path[MAXSIZE];                                 // Used to use paths
    int fds[3] = {0, 0, 0};                             // 0 errors 1 results 2 conf

    // Set error file
    if ((dup2((fds[0] = open("errors.txt", WRITETONEWFILE, 0644)) , 2)) == -1) {
        writeError("dup2");
        closeFiles(fds);
        return -1;
    }

    // Open results file
    if((resultsFD = open("results.csv", WRITETONEWFILE, 0644)) == -1) {
        writeError("open");
        closeFiles(fds);
        return -1;
    } else fds[1] = resultsFD;

    // Open the configuration file
    if((configurationFD = open(argv[1], O_RDONLY)) == -1) {
        writeError("open");
        closeFiles(fds);
        return -1;
    } else fds[2] = configurationFD;

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
        closeFiles(fds);
        return -1;
    }
    if (!S_ISDIR(dis.st_mode)) {
        write(2, "Not a valid directory\n", 22);
        closeFiles(fds);
        return -1;
    }
    if (access(configuraionContent[1], F_OK) != 0) {
        write(2, "Input file not exist\n", 21);
        closeFiles(fds);
        return -1;
    }
    if (access(configuraionContent[2], F_OK) != 0) {
        write(2, "Output file not exist\n", 22);
        closeFiles(fds);
        return -1;
    }

    // Open stream to the directory from the configuration file
    if((dirStr = opendir(configuraionContent[0])) == NULL) {
        writeError("opendir");
        closeFiles(fds);
        return -1;
    }

    // Start going through the student's dirs
    while((dit = readdir(dirStr)) != NULL) {
        // TODO: readdir check

        // If . or .. or not a dir - next file
        if (!strcmp(dit->d_name, ".") || !strcmp(dit->d_name, "..") || dit->d_type != DT_DIR) continue;

        // Open student's dir
        strcpy(path, configuraionContent[0]);
        strcat(path, "/");
        strcat(path, dit->d_name);
        if ((inDirStr = opendir(path)) == NULL) {
            closedir(inDirStr);
            continue;
        }

        // Check for each dir inside the main dir
        flag = 0;
        while ((inDit = readdir(inDirStr)) != NULL) {
            if (!strcmp(inDit->d_name, ".") || !strcmp(inDit->d_name, ".." ) || inDit->d_type == DT_DIR) continue;

            // Check if .c file, if it is - grading it
            int len = strlen(inDit->d_name);
            if (inDit->d_name[len - 1] == 'c' && inDit->d_name[len - 2] == '.') {
                flag = 1;
                if((compileFile(path, inDit->d_name)) != 0) {       // Compilation error
                    writeToCsv(resultsFD, -1 ,dit->d_name);
                } else {
                    runFile(configuraionContent[1]);
                    if((compResult = checkOutput(configuraionContent[2])) != -1) {
                        writeToCsv(resultsFD, compResult ,dit->d_name);
                    }
                }
                break;
            }
        }

        // No c file
        if(!flag) writeToCsv(resultsFD, 0 ,dit->d_name);

        closedir(inDirStr);
    }

    // Close files
    closedir(dirStr);
    closeFiles(fds);
    return 0;
}