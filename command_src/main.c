#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmonkey.h"

int main(int argc, char **argv)
{
    uint8_t *hash;
    char    *hex;

    if (argc != 2)
        return fprintf(stderr, "Usage: %s <something to hash>\n", argv[0]), EXIT_FAILURE;

    hash = hashmonkey((uint8_t *)argv[1], strlen(argv[1]));
    if (hash == NULL)
        return fprintf(stderr, "ERROR OUT OF RAM\n"), EXIT_FAILURE;
    hex = bin2hex(hash);
    if (hex == NULL)
        return fprintf(stderr, "ERROR OUT OF RAM\n"), EXIT_FAILURE;
    printf("%s\n", hex);
    free(hash);
    free(hex);
    return EXIT_SUCCESS;
}