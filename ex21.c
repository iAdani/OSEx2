// Guy Adani 208642884

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// Closes the files
void closeFiles(int fd1, int fd2) {
    close(fd1);
    close(fd2);
}

// Returns error
void writeError(char* errStr) {
    char str[15] = "Error in: ";
    strcat(str, errStr);
    write(STDOUT_FILENO, str, strlen(str));
}

// Returns the lower case of a char
char toLowerCase(char c) {
    if(c >=65 && c <= 90) c += 32;
    return c;
}

int main(int argc, char* argv[]) {
    int fd1, fd2;                       // file descriptors
    char* error;                        // string for errors
    char c1, c2;                        // the chars read
    int charsRead1 = 1, charsRead2 = 1; // how many chars read
    int identicalFlag = 1;              // to know if identical or similar

    // check how many args
    if (argc != 3) {
        writeError("args");
        return -1;
    }

    // Open the files
    if ((fd1 = open(argv[1], O_RDONLY)) == -1 ||
        (fd2 = open(argv[2], O_RDONLY)) == -1) {
        writeError("open");
        return -1;
    }

    do {
        // Read the "original" chars
        if((charsRead1 = read(fd1, &c1, 1)) == -1) writeError("read");
        if((charsRead2 = read(fd2, &c2, 1)) == -1) writeError("read");

        // Check if identical
        if (c1 != c2) identicalFlag = 0;

        // Ignoring empty chars
        do {
            if((charsRead1 = read(fd1, &c1, 1)) == -1) {
                error = "Error in: read";
                write(STDOUT_FILENO, error, strlen(error));
            }
        } while((c1 == ' ' || c1 == '\n' || c1 == '\t') && charsRead1);
        do {
            if((charsRead2 = read(fd2, &c2, 1)) == -1) {
                error = "Error in: read";
                write(STDOUT_FILENO, error, strlen(error));
            }
        } while((c2 == ' ' || c2 == '\n' || c2 == '\t') && charsRead2);

        // Check is different
        if (toLowerCase(c1) != toLowerCase(c2)) {
            closeFiles(fd1, fd2);
            return 2;
        }

    } while(charsRead1 || charsRead2);

    closeFiles(fd1, fd2);
    if(identicalFlag) return 1;
    return 3;
}
