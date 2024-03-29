#pragma once

#include <stddef.h>
#include <stdint.h>

void* memcpy(void *dest, const void *src, size_t len);
void* memmove(void* dest, const void* src, size_t len);
void* memset(void *ptr, int value, size_t num);

// potentially unsafe, do not use with different sized/typed objects
// memcmp(&cat1, &cat2, sizeof(struct cat)) => GOOD
// memcmp(magic_sequence, "ABC", 4) => BAD
// use strncmp whenever possible
int memcmp(const void* cmp1, const void* cmp2, size_t len);

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t len);

char* strcpy(char *dest, const char *src);
char* strncpy(char* dest, const char* src, size_t size);
size_t strlcpy(char *dest,  const char *src, size_t size);

char* strcat(char *dest, const char *src);
char* strncat(char *dest, const char *src, size_t size);
size_t strlcat(char *dest, const char *src, size_t size);

int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t size);

char* strndup(const char* s, size_t size);
char* strdup(const char* s);
