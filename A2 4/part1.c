#include <stdio.h>
#include <stdlib.h>
#define blocksize 2


int main(int argc, char *argv[])
{
    FILE *file, *newfile;
    unsigned char bytes[blocksize] = {0};
    int bytes1, i;

    file = fopen(argv[1], "rb");
    
    while ((bytes1 = fread(bytes, sizeof*bytes, blocksize, file)) == blocksize) {
        printf("0x");
        for (i = 0; i<blocksize; i++)
            printf("%02x", bytes[i]);
        printf("\n");
    }

    fclose(file);
}