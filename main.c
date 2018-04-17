#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define ERROR -1
#define IDENTICAL 3
#define SIMILAR 2
#define DIFFERENT 1
#define BUFFER_SIZE 16
#define FILE_TERMINATED -2

int getNextNonWhiteSpaceChar(int file, char buffer[BUFFER_SIZE], int curr_index) {
    while (1) {
        curr_index++;
        if (buffer[curr_index] != '\0') {
            if (buffer[curr_index] != ' ' && buffer[curr_index] != '\n') {
                return curr_index;
            } else {
                continue;
            }
        } else if (read(file, buffer, BUFFER_SIZE) > 0){
            curr_index = 0;
            continue;
        } else {
            break;
        }
    }
    return FILE_TERMINATED;
}

int isLatinLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int compareLatinLetters(char c1, char c2) {
    return tolower(c1) == tolower(c2);
}

int compareFiles(char file_path1[], char file_path2[]) {
    printf("%s\n%s\n", file_path1, file_path2);
    int result_code = IDENTICAL;
    int file1, file2;
    char buff1[BUFFER_SIZE + 1];
    char buff2[BUFFER_SIZE + 1];

    buff1[BUFFER_SIZE] = '\0';
    buff2[BUFFER_SIZE] = '\0';

    // open first file
    file1 = open(file_path1, O_RDONLY);
    if (file1 < 0) {
        printf("Error: failed opening the first file supplied\n");
        // todo: print message to stderr?
        return ERROR;
    }

    //open the second file
    file2 = open(file_path2, O_RDONLY);
    if (file2 < 0) {
        printf("Error: failed opening the first file supplied\n");
        // todo: print message to stderr?
        return ERROR;
    }

    while(read(file1, buff1, BUFFER_SIZE) > 0 && read(file2 ,buff2, BUFFER_SIZE) > 0) {
                int i, j = 0;
                while (buff1[i] != '\0' && buff2[j] != '\0') {
                    // characters are identical
                    if (buff1[i] == buff2[j]) {
                        i++; j++;
                    } else if (buff1[i] == '\n' || buff1[i] == ' ' || buff2[j] == '\n' || buff2[i] == ' ') {
                        result_code = SIMILAR;
                        if (buff1[i] == '\n' || buff1[i] == ' ') {
                            i = getNextNonWhiteSpaceChar(file1, buff1, i);
                            // todo: check return value
                            continue;
                        } else {
                            j = getNextNonWhiteSpaceChar(file2, buff2, j);
                            // todo: check return value
                            continue;
                        }
                    } else if (tolower(buff1[i]) == tolower(buff2[j])) {
                        result_code = SIMILAR;
                        i++; j++;
                    } else {
                        result_code = DIFFERENT;
                        break;
                    }
                }
        }

    close(file1);
    close(file2);
    return result_code;
}

int main(int argc, char *argv[]) {
    if (argc > 3) {
        printf("Too many arguments.\n");
    } else if (argc < 3) {
        printf("Expected two arguments.\n");
    } else {
        int result_code = compareFiles(argv[1], argv[2]);
        switch (result_code) {
            case IDENTICAL: printf("Files are identical.\n");
                break;
            case SIMILAR: printf("Files are similar.\n");
                break;
            case DIFFERENT: printf("Files are different.\n");
                break;
            default:
                break;
        }
    }
    return 0;
}