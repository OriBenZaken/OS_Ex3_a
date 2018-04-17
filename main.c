#include <stdio.h>

int compareFiles(char** file_name1, char** file_name2) {
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc >= 3) {
        printf("Too many arguments.\n");
    } else if (argc < 3) {
        printf("Expected two arguments.\n");
    } else {
        int result_code = compareFiles(argv[1], argv[2]);
    }
    return 0;
}