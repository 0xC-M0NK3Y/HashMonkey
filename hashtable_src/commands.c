#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "hashmonkey.h"

entrie_t *parse_arg(char *arg) {
    char        *ptr;
    char        *type;
    entrie_t    *entrie;
    type_e      t;

    if (arg == NULL)
        return fprintf(stderr, "SYNTAX ERROR NEAR \"\n"), NULL;
    arg++;
    ptr = arg;
    arg = strchr(arg, '"');
    if (arg == NULL)
        return fprintf(stderr, "SYNTAX ERROR NEAR \"\n"), NULL;
    *arg = 0;
    arg++;
    while (isspace(*arg))
        arg++;
    if (*arg == 0)
        return fprintf(stderr, "SYNTAX ERROR MISSING TYPE\n"), NULL;
    type = arg;
    if (strncmp(type, "CHAR", 4) == 0) {
        t = CHAR;
        arg += 4;
    } else if (strncmp(type, "INT", 3) == 0) {
        t = INT;
        arg += 3;
    } else {
        return fprintf(stderr, "SYNTAX ERROR UNKNOWN TYPE\n"), NULL;
    }
    while (1) {
        if (*arg && !isspace(*arg) && *arg != ';')
            return fprintf(stderr, "SYNTAX ERROR AFTER TYPE\n"), NULL;
        if (*arg == ';')
            break;
        if (*arg == 0)
            return fprintf(stderr, "SYNTAX ERROR MISSING ;\n"), NULL;    
        arg++;
    }
    arg++;
    if (*arg)
        return fprintf(stderr, "SYNTAX ERROR AFTER ;\n"), NULL;

    entrie = malloc(sizeof(*entrie));
    if (entrie == NULL)
        return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), NULL;
    memset(entrie, 0, sizeof(*entrie));
    entrie->type = t;

    switch (entrie->type) {
    size_t tmp;
    case CHAR:
        tmp = strlen(ptr)+1;
        entrie->value = malloc(tmp);
        if (entrie->value == NULL)
            return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), free(entrie), NULL;
        memcpy(entrie->value, ptr, tmp);
        if (((char *)entrie->value)[0] == 0 || strlen((char *)entrie->value) == 0)
            return fprintf(stderr, "SYNTAX ERROR NOT A VALID STRING\n"), free_entrie(entrie), NULL;
        break;
    case INT:
        entrie->value = (int *)malloc(sizeof(int));
        if (entrie->value == NULL)
            return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), free(entrie), NULL;
        *(int *)entrie->value = atoi(ptr);
        if (*ptr == '-')
            ptr++;
        while (isdigit(*ptr))
            ptr++;
        if (*ptr)
            return fprintf(stderr, "SYNTAX ERROR NOT AN INTEGER\n"), free_entrie(entrie), NULL;
        break;
    default: break;
    }
    entrie->next = NULL;
    return entrie;
}

int execute_put(hashtable_t *ht, char *arg) {
    entrie_t    *entrie = NULL;
    uint8_t     *index = NULL;
    entrie_t    **ptr = NULL;

    entrie = parse_arg(arg);
    if (entrie == NULL)
        return -1;
    switch (entrie->type) {
    case INT: index = hashmonkey((uint8_t *)entrie->value, sizeof(int)); break;
    case CHAR: index = hashmonkey((uint8_t *)entrie->value, strlen(entrie->value)); break;
    default: break;
    }
    if (index == NULL)
        return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), free_entrie(entrie), -1;
    ptr = &ht->entries[*(uint16_t *)index];
    if (*ptr == NULL) {
        *ptr = entrie;
    } else {
        entrie_t *p = *ptr;
        while (p->next != NULL)
            p = p->next;
        p->next = entrie;
    }
    free(index);
    printf("DATA INSERTED\n");
    ht->size++;
    return 1;
}

int execute_get(const hashtable_t *ht, char *arg) {
    entrie_t *entrie = NULL;
    uint8_t  *index = NULL;
    entrie_t *ptr = NULL;

    entrie = parse_arg(arg);
    if (entrie == NULL)
        return -1;
    switch (entrie->type) {
    case INT: index = hashmonkey((uint8_t *)entrie->value, sizeof(int)); break;
    case CHAR: index = hashmonkey((uint8_t *)entrie->value, strlen(entrie->value)); break;
    default: break;
    }
    if (index == NULL)
        return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), free_entrie(entrie), -1;
    ptr = ht->entries[*(uint16_t *)index];
    if (ptr == NULL) {
        printf("Not in database\n");
    } else {
        if (ptr->next == NULL) {
            print_entrie(ptr);
        } else {
            while (ptr)
            {
                if (is_same_entrie(entrie, ptr))
                    print_entrie(ptr);
                ptr = ptr->next;
            }
        }
    }
    free(index);
    return 1;
}

int execute_remove(hashtable_t *ht, char *arg) {
    entrie_t *entrie = NULL;
    uint8_t  *index = NULL;
    entrie_t **ptr = NULL;

    entrie = parse_arg(arg);
    if (entrie == NULL)
        return -1;
    switch (entrie->type) {
    case INT: index = hashmonkey((uint8_t *)entrie->value, sizeof(int)); break;
    case CHAR: index = hashmonkey((uint8_t *)entrie->value, strlen(entrie->value)); break;
    default: break;
    }
    if (index == NULL)
        return fprintf(stderr, "CRITICAL ERROR OUT OF RAM\n"), free_entrie(entrie), -1;
    ptr = &ht->entries[*(uint16_t *)index];
    if (*ptr == NULL) {
        printf("Not in database\n");
    } else {
        if ((*ptr)->next == NULL) {
            free_entrie(*ptr);
            *ptr = NULL;
            printf("Removed 1 value\n");
            ht->size--;
        } else {
            int         nb = 0;
            entrie_t    *tmp;

            while (*ptr)
            {
                if (is_same_entrie(entrie, *ptr)) {
                    tmp = (*ptr)->next;
                    free_entrie(*ptr);
                    *ptr = tmp;
                    nb++;
                    continue;
                }
                *ptr = (*ptr)->next;
            }
            printf("Removed %d value(s)\n", nb);
            ht->size -= nb;
        }
    }
    free_entrie(entrie);
    free(index);
    return 1;
}

int execute_cmd(hashtable_t *ht, char *cmd) {

    if (strncmp(cmd, "PUT", 3) == 0) {
        if (execute_put(ht, strchr(cmd, '"')) < 0)
            return -1;
    } else if (strcmp(cmd, "SHOW;") == 0) {
        print_hashtable(ht);
    } else if (strncmp(cmd, "GET", 3) == 0) {
        if (execute_get(ht, strchr(cmd, '"')) < 0)
            return -1;
    } else if (strncmp(cmd, "REMOVE", 6) == 0) {
        if (execute_remove(ht, strchr(cmd, '"')) < 0)
            return -1;
    } else if (strcmp(cmd, "EXIT;") == 0) {
        return 0;
    } else {
        fprintf(stderr, "SYNTAX ERROR UNKNOWN COMMAND\n");
    }

    return 1;
}