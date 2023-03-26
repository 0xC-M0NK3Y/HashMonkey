#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include "hashmonkey.h"

#define MIN_RANDOM_BYTES    10
#define MAX_RANDOM_BYTES    200

#define VERBOSE_PRINT(v, ...) (v == 1)?({fprintf(stderr, __VA_ARGS__);fputc('\n', stderr);}):({})

typedef struct testset {
    uint8_t *hash;
    uint8_t *value;
    size_t  len;
}   testset_t;

size_t generate_random_bytes(uint8_t **ptr) {

    size_t len = rand() % (MAX_RANDOM_BYTES-MIN_RANDOM_BYTES) + MIN_RANDOM_BYTES;

    *ptr = malloc(len);
    if (*ptr == NULL)
        return 0;

    for (size_t i = 0; i < len; i++) {
        (*ptr)[i] = rand() % UINT8_MAX;
    }
    return len;
}

int same_hash(uint8_t *hash1, uint8_t *hash2) {
    int cmp = 0;

    for (int i = 0; i < 16; i++) {
        if (hash1[i] == hash2[i])
            cmp++;
    }
    return (cmp == 16)?1:0;
}

int same_value(uint8_t *v1, size_t s1, uint8_t *v2, size_t s2) {
    
    size_t cmp = 0;

    if (s1 != s2)
        return 0;
    for (size_t i = 0; i < s1; i++) {
        if (v1[i] == v2[i])
            cmp++;
    }
    return (cmp == s1)?1:0;
}

int compare_testset(const void *ptr1, const void *ptr2) {
    testset_t *p1 = (testset_t *)ptr1;
    testset_t *p2 = (testset_t *)ptr2;

    return memcmp(p1->hash, p2->hash, 16);
}

size_t get_collisions(testset_t *testset, size_t size, size_t *same_values, int verbose) {
    size_t collisions = 0;
    size_t  tmp = 0;
    size_t  j;

    for (size_t i = 0; i < size; i++) {
        j = 1;
        while (i+j < size && same_hash(testset[i].hash, testset[i+j].hash))
        {
            if (same_value(testset[i].value, testset[i].len, testset[i+j].value, testset[i+j].len)) {
                VERBOSE_PRINT(verbose, "Same value |%s|==|%s|", testset[i].value, testset[i+j].value);
                tmp++;
            } else {
                VERBOSE_PRINT(verbose, "Collision value |%s|==|%s|", testset[i].value, testset[i+j].value);
                collisions++;
            }
            j++;
        }
        i += j-1;
    }
    same_values[0] = tmp;

    return collisions;
}
//     1         0

//        98      3  0
// 1111 0000 1111 0000

// 100 40

// return the % of ones at this pos bit position
float get_bit_distribution(int pos, testset_t *testset, size_t size) {
    int     zero = 0;
    int     one = 0;

    for (size_t i = 0; i < size; i++) {
        if ((testset[i].hash[pos/8] >> (pos%8)) & 0x1)
            one++;
        else
            zero++;
    }
    return (float)((float)one/(float)size*(float)100);
}

static float get_average_one(float *bit_distrib) {
    float ret = 0;

    for (int i = 0; i < 128; i++)
        ret += bit_distrib[i];
    return (float)ret/(float)128;
}

void print_results(size_t size, size_t collisions, size_t same_values, float *bit_distrib, char *opt) {
    float average_one = 0;

    printf("\nResults for %s size %ld :\n\n", opt, size);

    printf("Total of %ld collisions (collisions can become exponential for large testsets)\n\n", collisions);
    printf("Total of %ld same values (its not a collision between two values having the same hash)\n\n", same_values);
    if (same_values != 0)
        printf("(Note that same values can disturb the bit distribution !)\n\n");
    printf("Bit dtribution for each bit : \n\n");
    for (int i = 0; i < 128; i++)
        printf("For bit %d : %.2f%% of ones, %.2f%% of zeros\n", i, bit_distrib[i], (float)100-bit_distrib[i]);
    average_one = get_average_one(bit_distrib);
    printf("Average ones : %.2f\nAverage of zeros %.2f\n", average_one, (float)100-average_one);
}

int random_test(size_t size, int verbose) {

    testset_t   *testset = NULL;
    float       bit_distrib[128] = {0};
    size_t      collisions = 0;
    size_t      same_values = 0;

    testset = malloc(size * sizeof(*testset));
    if (testset == NULL)
        return perror("malloc"), -1;

    VERBOSE_PRINT(verbose, "Random mode\n");
    VERBOSE_PRINT(verbose, "generating testset size %ld...", size);

    /* generate random byte and hash them */
    for (size_t i = 0; i < size; i++) {
        testset[i].len = generate_random_bytes(&testset[i].value);
        if (testset[i].len == 0 || testset[i].value == NULL) // by verifying this here
            return perror("malloc"), -1;
        testset[i].hash = hashmonkey(testset[i].value, testset[i].len);
        if (testset[i].hash == NULL)
            return perror("malloc"), -1;    // only malloc can fail in hashmonkey, because it return NULL only on malloc fail, or data == NULL or len == 0
        if (i % 10000 == 0)
            VERBOSE_PRINT(verbose, "generated %ld...", i);
    }
    VERBOSE_PRINT(verbose, "finished generating");
    VERBOSE_PRINT(verbose, "qsort testset...");
    qsort(testset, size, sizeof(*testset), compare_testset);
    /* then get the collisions between hashes */
    VERBOSE_PRINT(verbose, "computing collisions...");
    collisions = get_collisions(testset, size, &same_values, verbose);
    VERBOSE_PRINT(verbose, "computing bit distribution...");
    /* finnaly get distribution of each bit */
    for (int i = 0; i < 128; i++)
        bit_distrib[i] = get_bit_distribution(i, testset, size);
    /* print results */
    print_results(size, collisions, same_values, bit_distrib, "random");
    /* free */
    for (size_t i = 0; i < size; i++) {
        free(testset[i].value);
        free(testset[i].hash);
    }
    free(testset);
    return 1;
}

static inline size_t count_lines(uint8_t *buffer, size_t file_size) {
    size_t ret = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (buffer[i] == '\n')
            ret++;
    }
    return ret;
}

int file_test(char *file, int verbose) {

    FILE        *fp = NULL;
    testset_t   *testset = NULL;
    size_t      file_size;
    size_t      size = 0;
    size_t      tmp = 0;
    uint8_t     *buffer = NULL;
    size_t      collisions = 0;
    size_t      same_values = 0;
    float       bit_distrib[128] = {0};

    VERBOSE_PRINT(verbose, "File mode\n");

    fp = fopen(file, "r");
    if (fp == NULL)
        return perror("fopen"), -1;
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    buffer = malloc(file_size);
    if (buffer == NULL)
        return perror("malloc"), -1;
    if (fread(buffer, 1, file_size, fp) != file_size)
        return perror("fread"), -1;
    fclose(fp);

    VERBOSE_PRINT(verbose, "counting lines...");
    size = count_lines(buffer, file_size);
    VERBOSE_PRINT(verbose, "%ld lines", size);
    /* count lines and malloc our testset */
    testset = malloc(size * sizeof(*testset));
    if (testset == NULL)
        return perror("malloc"), -1;
    memset(testset, 0, size * sizeof(*testset));

    VERBOSE_PRINT(verbose, "creating testset...");
    /* create testset by map the buffer into testset without reallocing */
    testset[tmp].value = &buffer[0];
    for (size_t i = 0; i < file_size; i++) {
        if (buffer[i] == '\n') {
            testset[tmp].len = &buffer[i] - testset[tmp].value;
            buffer[i] = '\0';
            tmp++;
            if (i != file_size - 1 && tmp != size)
                testset[tmp].value = &buffer[i+1];
        }
    }
    VERBOSE_PRINT(verbose, "generating hashes");
    /* generate hashes from lines */
    for (size_t i = 0; i < size; i++) {
        if (testset[i].value == NULL || testset[i].len == 0)
            return fprintf(stderr, "Something went wrong while parsing input file on line %ld (probably a blank line) %p %ld\n", i, testset[i].value, testset[i].len), -1;
        testset[i].hash = hashmonkey(testset[i].value, testset[i].len);
        if (testset[i].hash == NULL)
            return perror("malloc"), -1;
        if (i % 10000 == 0)
            VERBOSE_PRINT(verbose, "hashing %ld...", i);
    }
    VERBOSE_PRINT(verbose, "qsort testset...");
    qsort(testset, size, sizeof(*testset), compare_testset);
    VERBOSE_PRINT(verbose, "computing collisions...");
    /* then get the collisions between hashes */
    collisions = get_collisions(testset, size, &same_values, verbose);
    VERBOSE_PRINT(verbose, "computing bit distribution...");
    /* finnaly get distribution of each bit */
    for (int i = 0; i < 128; i++)
        bit_distrib[i] = get_bit_distribution(i, testset, size);
    /* print results */
    print_results(size, collisions, same_values, bit_distrib, file);
    /* free */
    for (size_t i = 0; i < size; i++) {
        free(testset[i].hash);
    }
    free(testset);
    free(buffer);

    return 1;
}

static inline int check_size(char *s) {
    for (int i = 0; s[i]; i++) {
        if (!isdigit(s[i]))
            return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3 && argc != 4)
        return printf("Usage: %s <options>\n\n" \
                      "Option can be '\"random\" <number of hash to test>' or an input file with one string to hash per line (with no blank lines)\n" \
                      "(last line must end with '\\n', otherwise last line will not be counted)\n" \
                      "Depending on the entry testing program ban be long\n" \
                      "This does only test the quality of hash\n", argv[0]), EXIT_FAILURE;
    
    if (argc >= 3 && strcmp(argv[1], "random") == 0) {
        int v = 0;
        if (!check_size(argv[2]))
            return fprintf(stderr, "Bad size value : %s\n", argv[2]), EXIT_FAILURE;
        if (argc == 4 && strcmp(argv[3], "-v") == 0)
            v = 1;
        else if (argc == 4)
            return fprintf(stderr, "Unrocognize option %s\n", argv[3]);
        if (random_test(atoi(argv[2]), v) < 0)
            return fprintf(stderr, "Something went wrong, probably ran out of RAM\n"), EXIT_FAILURE;
    } else if (argc == 2 || (argc == 3 && strcmp(argv[2], "-v") == 0)) {
        int v = 0;
        if (argc == 3)
            v = 1;
        if (file_test(argv[1], v) < 0)
            return fprintf(stderr, "Something went wrong, incorrect file or out of RAM\n"), EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}