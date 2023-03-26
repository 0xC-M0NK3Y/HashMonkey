#ifndef STRUCTS_H
# define STRUCTS_H

#include <stdint.h>
#include <stddef.h>

typedef enum type {
    CHAR,
    INT
}   type_e;

typedef struct entrie {
    void            *value;
    struct entrie   *next;
    type_e          type;
}   entrie_t;


typedef struct hashtable {
    entrie_t    *entries[UINT16_MAX];
    size_t      size;
}   hashtable_t;


#endif /* structs.h */