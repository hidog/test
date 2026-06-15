#ifndef SEPERATE_LARGE_FILE_H
#define SEPERATE_LARGE_FILE_H

#include <stdint.h>

/*
    f, g is a mapping function
    mapping {0,N} to {0,N}, 1-1, onto.
*/
int64_t slf_f( int64_t input, int64_t full_size, int prime );
int64_t slf_g( int64_t input, int64_t full_size, int prime );

void slf_test();

void seperate_large_file(const char* path );
void merge_large_file(const char* path);

void test_seperate();
void test_seperate2();

#endif