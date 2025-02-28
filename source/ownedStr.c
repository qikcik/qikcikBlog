#include "ownedStr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JUST_IN_CASE 16

OwnedStr OwnedStr_Alloc(const char* in_c_str) {
    OwnedStr result;
    result.capacity = strlen(in_c_str);
    result.str = (char*)malloc(sizeof(char)*result.capacity+JUST_IN_CASE);

    strcpy(result.str,in_c_str);

    result.str[result.capacity] = '\0';
    return result;
}

void OwnedStr_Concate(OwnedStr* result, const char* in_str) {

    result->capacity = result->capacity+strlen(in_str);
    char* old_str = result->str;
    result->str = (char*)malloc(sizeof(char)*result->capacity+JUST_IN_CASE);

    strcpy(result->str,old_str);
    strcat(result->str,in_str);
    result->str[result->capacity] = '\0';

    free(old_str);
}

void OwnedStr_ConcateWithSize(OwnedStr* result, const char* in_str,size_t in_size) {

    result->capacity = result->capacity+in_size;
    char* old_str = result->str;
    result->str = (char*)malloc(sizeof(char)*result->capacity+JUST_IN_CASE);

    strcpy(result->str,old_str);
    strncat(result->str,in_str,in_size);
    result->str[result->capacity] = '\0';

    free(old_str);
}

void OwnedStr_Free(OwnedStr* in_str) {
    free(in_str->str);
    in_str->capacity = 0;
    in_str->str = nullptr;
}

OwnedStr OwnedStr_AllocFromFile(FILE *f) {
    OwnedStr result;
    fseek(f, 0, SEEK_END);
    result.capacity = ftell(f);

    fseek(f,  0, SEEK_SET);
    result.str = (char *) malloc(result.capacity+JUST_IN_CASE);
    fread(result.str, 1, result.capacity, f);

    result.str[result.capacity] = '\0';
    return result;
}
