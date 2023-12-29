#pragma once
#include <stddef.h>

struct OwnedStr {
    char* str;
    size_t capacity;
};
typedef struct OwnedStr OwnedStr;

OwnedStr OwnedStr_Alloc(const char*);
void OwnedStr_Concate(OwnedStr*,const char*);
void OwnedStr_Free(OwnedStr*);