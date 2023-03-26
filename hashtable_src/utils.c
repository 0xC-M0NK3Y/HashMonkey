#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"

void free_entrie(entrie_t *entrie) {
    if (entrie->value)
        free(entrie->value);
    free(entrie);
}

void free_hashtable(hashtable_t *ht) {

    entrie_t *ptr1;
    entrie_t *ptr2;

    for (int i = 0; i < UINT16_MAX; i++) {
        ptr1 = ht->entries[i];
        while (ptr1 != NULL) 
        {
            free(ptr1->value);
            ptr2 = ptr1->next;
            free(ptr1);
            ptr1 = ptr2;   
        }
    }
}

char *get_type(type_e type) {
    switch (type) {
    case INT: return "INT";
    case CHAR: return "CHAR";
    default: break;
    }
    return "UNKNOWN TYPE";
}

void print_entrie(const entrie_t *entrie) {
    switch (entrie->type) {
    case INT: printf("%d %s\n", *(int *)entrie->value, get_type(entrie->type)); break;
    case CHAR: printf("%s %s\n", (char *)entrie->value, get_type(entrie->type)); break;
    default: printf("BAD ENTRY DETECTED\n"); break;
    }
}

int is_same_entrie(const entrie_t *e1, const entrie_t *e2) {
    size_t s1;
    size_t s2;

    if (e1->type != e2->type)
        return 0;
    switch (e1->type) {
    case INT:
        if (*(int *)e1->value == *(int *)e2->value)
            return 1;
        break;
    case CHAR:
        s1 = strlen((char *)e1->value);
        s2 = strlen((char *)e2->value);

        if (s1 != s2)
            return 0;
        if (strncmp((char *)e1->value, (char *)e2->value, s1) == 0)
            return 1;
        break;
    default: return 0;
    }
    return 0;
}

void print_hashtable(const hashtable_t *ht) {

    for (int i = 0; i < UINT16_MAX; i++) {
        if (ht->entries[i] != NULL) {
            entrie_t *ptr = ht->entries[i]->next;

            print_entrie(ht->entries[i]);
            while (ptr)
            {
                print_entrie(ptr);
                ptr = ptr->next;
            }
        }
    }
    printf("%ld total entries\n", ht->size);
}