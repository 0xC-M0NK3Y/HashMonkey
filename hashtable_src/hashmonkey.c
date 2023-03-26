#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

// "abfd5e9ra65za65z5ad5a56z6e5aze65"
// = 128bits en hexa

const uint8_t iv[256] = {0xd6, 0xda, 0xee, 0xdd, 0xaf, 0x8c, 0xb8, 0x89, 0x88, 0x58, 0xbc, 0x88, 0x8b, 0xd4, 
                      0x6, 0x61, 0x97, 0xa7, 0x42, 0x31, 0xb, 0x51, 0x20, 0x3b, 0x15, 0x80, 0xc7, 0xe0, 
                      0xc, 0xc4, 0x3d, 0xa7, 0x7f, 0xc5, 0xf1, 0xf7, 0x38, 0x72, 0x68, 0x16, 0xce, 0x2f, 
                      0xd1, 0x86, 0x7a, 0x59, 0xe1, 0x20, 0xc4, 0x4d, 0x79, 0xa4, 0x48, 0x39, 0xf1, 0xe8, 
                      0x77, 0xb0, 0x53, 0x80, 0xe7, 0x38, 0x62, 0x8f, 0x45, 0xc8, 0xfc, 0x1b, 0x8d, 0x88, 
                      0xf0, 0x7a, 0x62, 0xc4, 0x41, 0x2f, 0x64, 0x27, 0x66, 0x74, 0x6b, 0xe2, 0x5b, 0x6a, 
                      0x64, 0xad, 0x66, 0xd4, 0x18, 0x2e, 0x4, 0x82, 0xfb, 0x57, 0xc7, 0x23, 0xce, 0x93, 
                      0xd5, 0x73, 0xd, 0x14, 0x17, 0xc6, 0x71, 0xa2, 0x4e, 0x62, 0x2f, 0x46, 0x13, 0xa9, 
                      0x74, 0x3c, 0x49, 0x1, 0x4f, 0x15, 0x47, 0xf8, 0x9e, 0xb4, 0x85, 0x52, 0x5f, 0xeb, 
                      0x62, 0x3d, 0x28, 0xfb, 0x71, 0x2f, 0x50, 0x6, 0x67, 0x3c, 0xe4, 0xe, 0xb9, 0x1a, 
                      0x7b, 0x48, 0x5f, 0xd6, 0x3d, 0x9f, 0x15, 0x81, 0xa, 0xad, 0x41, 0xe5, 0xf7, 0xfc, 
                      0x73, 0x73, 0x66, 0x39, 0x6c, 0x24, 0xfe, 0x64, 0xf3, 0x19, 0x1c, 0xec, 0x52, 0x3, 
                      0x1f, 0x4d, 0x44, 0xe8, 0x86, 0xc1, 0xbe, 0xb8, 0xb9, 0xde, 0xb, 0xe4, 0xa9, 0x26, 
                      0x83, 0xab, 0x2f, 0xde, 0x83, 0xc8, 0x8f, 0xc1, 0xe0, 0xcf, 0xe7, 0x84, 0x3f, 0x3a, 
                      0xa9, 0x9a, 0x9d, 0x8b, 0x93, 0x1c, 0x3d, 0x1, 0x55, 0xcf, 0x3e, 0xc, 0x23, 0xb, 0xdf, 
                      0xaf, 0xae, 0x86, 0xc0, 0x90, 0x3a, 0x8a, 0x13, 0x82, 0xfd, 0xcb, 0xf8, 0x82, 0x25, 
                      0x4a, 0x6e, 0x55, 0x46, 0xe8, 0xb9, 0x65, 0xe2, 0x9d, 0x97, 0x7b, 0x5e, 0xf8, 0x6f, 
                      0xf9, 0xe4, 0x2b, 0x8f, 0xee, 0xfb, 0xb7, 0x16, 0x39, 0x9b, 0x30, 0x3, 0xd3, 0x4b, 
                      0xf9, 0x41, 0x22};

static inline uint8_t randt(uint8_t t) {
    return (((t >> 3) ^ 0x54) | ((t << 4) ^ 0xA1)) & (((t >> 1) ^ 0xE4) | ((t << 2) ^ 0x89));
} 

char *bin2hex(const uint8_t *hash) {
    char *ret;

    ret = malloc(33);
    if (ret == NULL)
        return NULL;
    memset(ret, 0, 33);

    uint8_t *ptr = (uint8_t *)hash;
    for (int i = 0; i < 16; i++) {
        uint8_t tmp = ptr[i];

        if (tmp / 0x10 > 0x9)
            ret[i*2] = tmp/0x10-10 + 'A';
        else 
            ret[i*2] = tmp/0x10 + '0';

        if (tmp % 0x10 > 0x9)
            ret[i*2+1] = tmp%0x10-10 + 'A';
        else 
            ret[i*2+1] = tmp%0x10 + '0';   
    }
    return ret;
}

uint8_t *hashmonkey(const uint8_t *data, size_t len) {

    uint8_t t = 0x45;
    uint8_t *hash;

    if (data == NULL)
        return NULL;
    if (len == 0)
        return NULL;

    hash = malloc(16);
    if (hash == NULL)
        return NULL;
    memset(hash, 0, 16);

    t = randt(t);

    for (size_t i = 0; i < len; i++, len--) {
        char c1 = data[i];
        char c2;
        uint8_t x1 = iv[(c1 << 1)^t % 256];
        uint8_t x2;
        
        if (len-1 != i)
            c2 = data[len-1];
        else
            c2 = iv[(i+((c1^t^0x69) << 1))%256];

        x2 = iv[(c2+(c1^t))%256];

        for (int i = 0; i < 8; i++) {
            if (((c1 >> i) & 0x1) == 1) {
                uint8_t *p = (uint8_t *)hash;
                for (int j = 0; j < 16; j++) {
                    ((i%2)==0)?(p[j]^=((x1 << 8)|~x2)):(p[j]^=((x2 << 8)|~x1));
                    x1 ^= x2 ^ iv[((i^x1)<<1)%256] ^ c1;
                    x2 ^= t;
                }
            } else {
                uint8_t *p = (uint8_t *)hash;
                for (int j = 0; j < 16; j++) {
                    ((i%2)==0)?(p[j]^=((~x1 << 8)|(x2^c1))):(p[j]^=((~x2 << 8)|(x1^c1)));
                    x2 ^= x1 ^ iv[((i^x2)<<1)%256] ^ c2;
                    x1 ^= t;
                }
            }
            t = randt(t);
            if (((c2 >> i) & 0x1) == 1) {
                uint8_t *p = (uint8_t *)hash;
                for (int j = 0; j < 16; j++) {
                    ((i%2)==0)?(p[j]^=((~x1 << 8)|x2)):(p[j]^=((~x2 << 8)|x1));
                    x1 ^= x1 ^ iv[((i^x2)<<1)%256] ^ c2;
                    x2 ^= t;
                }
            } else {
                uint8_t *p = (uint8_t *)hash;
                for (int j = 0; j < 16; j++) {
                    ((i%2)==0)?(p[j]^=((~x1 << 8)|(x2^c1))):(p[j]^=((~x2 << 8)|(x1^c1)));
                    x2 ^= x2 ^ iv[((i^x1)<<1)%256] ^ c1;
                    x1 ^= t;
                }
            }
            t = randt(t);
        }
    }
    return hash;
}