#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "debug.h"

int main(int argc, char **argv)
{
    int opt = -1;
    char* prompt = "308sh>";

    while ((opt = getopt(argc, argv, "hp:")) != -1) {
        switch (opt) {
            case 'p':
                prompt = optarg;
                break;
            case 'h':
            default:
                exit(-1);
                break;
        }
    }

    printf("%s\n", prompt);

    return 0;
}