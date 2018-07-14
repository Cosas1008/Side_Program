#include <iostream>
#include <fstream>
#include <cstdlib>
#include "string.h"
#include <sstream>
#include <cstring>
#include <cstdio>

#define MAXCHAR 1000
int main() {
    FILE *file;
    file = fopen("sample.txt", "r");
    if(file == NULL){
        printf("Error open file");
        exit(0);
    }
    char line[MAXCHAR];
    char s[2] = " ";
    while(fgets(line,MAXCHAR, file) != NULL){
        char* token = strtok(line,s);
        while(token != NULL)
        {
            printf(" %s", token);
            token = strtok(NULL, s); // read the blank char (separator)
        }
        std::cout << "\n";
    }
    std::cout << "Hello, World!" << std::endl;
    fclose(file);
    return 0;
}