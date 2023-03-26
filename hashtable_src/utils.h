#ifndef UTILS_H
# define UTILS_H

#include "structs.h"

void free_entrie(entrie_t *entrie);
void print_hashtable(const hashtable_t *ht);
void print_entrie(const entrie_t *entrie);
int is_same_entrie(const entrie_t *e1, const entrie_t *e2);
void free_hashtable(hashtable_t *ht);

#endif /* utils.h */