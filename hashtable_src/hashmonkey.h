#ifndef HASHMONKEY_H
# define HASHMONKEY_H

#include <stdint.h>

uint8_t *hashmonkey(const uint8_t *data, size_t len);
char *bin2hex(const uint8_t *hash);

#endif /* hashmonkey.h */