// Name: Ori Ben Zaken  ID: 311492110

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

/**
 * prints error in sys call to stderr.
 */
void printErrorInSysCallToSTDERR() {
    fprintf(stderr,"Error in system call\n");
}

/**
 * return the index in buffer of the next non-white-space character in the file.
 * @param file file descriptor
 * @param buffer reading buffer
 * @param curr_index current index in the buffer (expected character in that index to be white space)
 * @return index in buffer of the next non-white-space character in the file.
 *          if reading from file was over and a non-white-space character is found - return FILE_TERMINATED
 *          if error occurred while reading from file - return ERROR
 */
int getNextNonWhiteSpaceChar(int file, char buffer[BUFFER_SIZE], int curr_index) {
    int read_bytes;
    while (1) {
        curr_index++;
        // if buffer still contains bytes to read
        if (buffer[curr_index] != '\0') {
            // character is not a white space
            if (buffer[curr_index] != ' ' && buffer[curr_index] != '\n') {
                return curr_index;
            } else {
                continue;
            }
            // try to read again from file
        } else if ((read_bytes = read(file, buffer, BUFFER_SIZE)) > 0) {
            buffer[read_bytes] = '\0';
            // in order to assign 0 to curr_index in the next iteration
            curr_index = -1;
            continue;
        } else {
            break;
        }
    }
    // done reading all bytes from the file
    if (read_bytes == 0) {
        return FILE_TERMINATED;
    }
    // Error in read sys call
    printf("getNextNonWhiteSpaceChar: Error while reading from file.\n");
    printErrorInSysCallToSTDERR();
    return ERROR;
}

/**
 * comparing between two files.
 * @param file_path1 path of the first file.
 * @param file_path2 path of the second file.
 * @return 1 - files are identical.
 *         2 - files are similar - differs only in white spaces (space, new line) and in uppercase/lowercase.
 *         3 - files are different.
 *         -1 - error occurred in one of the sys calls.
 */
int compareFiles(char file_path1[], char file_path2[]) {
    printf("First file: %s\nSecond file: %s\n", file_path1, file_path2);
    int result_code = IDENTICAL;
    int file1, file2;
    char buff1[BUFFER_SIZE + 1];
    char buff2[BUFFER_SIZE + 1];
    // number of bytes which were read from each file.
    int read_bytes_file1, read_bytes_file2;

    // open first file
    file1 = open(file_path1, O_RDONLY);
    if (file1 < 0) {
        printf("Error: failed opening file 1\n");
        printErrorInSysCallToSTDERR();
        return ERROR;
    }

    //open the second file
    file2 = open(file_path2, O_RDONLY);
    if (file2 < 0) {
        printf("Error: failed opening file 2\n");
        printErrorInSysCallToSTDERR();
        return ERROR;
    }

    int i = 0, j = 0;
    read_bytes_file1 = read(file1, buff1, BUFFER_SIZE);
    read_bytes_file2 = read(file2, buff2, BUFFER_SIZE);
    // two files are not empty
    if (read_bytes_file1 > 0 && read_bytes_file2 > 0) {
        // put NULL in the end of the stream. read sys call doesn't do it automatically
        buff1[read_bytes_file1] = '\0';
        buff2[read_bytes_file2] = '\0';
        while (buff1[i] != '\0' && buff2[j] != '\0') {
            // characters are identical
            if (buff1[i] == buff2[j]) {
                i++;
                j++;
                // one of the characters from the first file or the second file, or both of them is a white space
            } else if (buff1[i] == '\n' || buff1[i] == ' ' || buff2[j] == '\n' || buff2[j] == ' ') {
                result_code = SIMILAR;
                // white space is from the first file. get next non-white-space character
                if (buff1[i] == '\n' || buff1[i] == ' ') {
                    i = getNextNonWhiteSpaceChar(file1, buff1, i);
                    // reading from file 1 was over
                    if (i == FILE_TERMINATED) {
                        break;
                        // error occurred while reading from the file.
                    } else if (i == ERROR) {
                        result_code = ERROR;
                    }
                    // white space is from the first file. get next non-white-space character
                } else {
                    j = getNextNonWhiteSpaceChar(file2, buff2, j);
                    // reading from file 2 was over
                    if (j == FILE_TERMINATED) {
                        break;
                    } else if (j == ERROR) {
                        // error occurred while reading from the file.
                        result_code = ERROR;
                        break;
                    }
                }
                // characters are the same in lowercase, in case of latin letters
            } else if (tolower(buff1[i]) == tolower(buff2[j])) {
                result_code = SIMILAR;
                i++;
                j++;
                // characters completely different and none of the is a white space
                // files are different - no need to keep comparing
            } else {
                result_code = DIFFERENT;
                break;
            }
            // went through all bytes in buff1
            if (buff1[i] == '\0') {
                if ((read_bytes_file1 = read(file1, buff1, BUFFER_SIZE)) > 0) {
                    i = 0;
                    buff1[read_bytes_file1] = '\0';
                } else {
                    break;
                }
            }

            // went through all bytes in buff2
            if (buff2[j] == '\0') {
                if ((read_bytes_file2 = read(file2, buff2, BUFFER_SIZE)) > 0) {
                    j = 0;
                    buff2[read_bytes_file2] = '\0';
                } else {
                    break;
                }
            }
        }
        // at least one of the files is empty or error occurred in the first read sys call
    } else {
        // one of the file is empty but not both of them
        if ((read_bytes_file1 == 0 || read_bytes_file2 == 0) && !(read_bytes_file1 == 0 && read_bytes_file2 == 0)) {
            result_code = SIMILAR;
        } else if (read_bytes_file1 < 0 || read_bytes_file2 < 0){
            printf("Error: failed reading from file\n");
            printErrorInSysCallToSTDERR();
            result_code = ERROR;
        }
    }

    // check if one of the file is still open and my contain white spaces.
    // relevant only if the files is currently similar.
    if (buff1[i] != '\0' && result_code == SIMILAR) {
        if (getNextNonWhiteSpaceChar(file1, buff1, i) != FILE_TERMINATED) {
            result_code = DIFFERENT;
        }
    } else if (buff2[j] != '\0' && result_code == SIMILAR) {
        if (getNextNonWhiteSpaceChar(file2, buff2, j) != FILE_TERMINATED) {
            result_code = DIFFERENT;
        }
    }

    if (close(file1) == -1) {
       printf("Error while closing file 1\n");
       printErrorInSysCallToSTDERR();
    }
    if (close(file2) == -1) {
        printf("Error while closing file 2\n");
        printErrorInSysCallToSTDERR();
    }
    return result_code;
}

/**
 * comparing two files. recieves files path as command line arguments.
 * @param argc number of command line arguments including the name of the program.
 * @param argv command line arguments
 * @return 1 - files are identical.
 *         2 - files are similar - differs only in white spaces (space, new line) and in uppercase/lowercase.
 *         3 - files are different.
 *         -1 - error occurred in one of the sys calls.
 */
int main(int argc, char *argv[]) {
    int result_code;
    if (argc > 3) {
        printf("Too many arguments.\n");
    } else if (argc < 3) {
        printf("Expected two arguments.\n");
    } else {
        result_code = compareFiles(argv[1], argv[2]);
        switch (result_code) {
            case IDENTICAL:
                printf("\t-> Files are identical.\n");
                break;
            case SIMILAR:
                printf("\t-> Files are similar.\n");
                break;
            case DIFFERENT:
                printf("\t-> Files are different.\n");
                break;
            default:
                break;
        }
    }
    return result_code;
}