#ifndef SEPERATE_LARGE_FILE_H
#define SEPERATE_LARGE_FILE_H

#include <stdint.h>


struct SlfSetting
{
    int prime;
    int repeat_size;
    int block_size;
    int tail_size;
    char filename[1000];
};


/*
    slf_f is encrypto function, use for seperate and encrypto.
    slf_g is decrypto function, use for merge and decrypto.
*/
int64_t slf_f( int64_t input, int64_t full_size, int prime );  
int64_t slf_g( int64_t input, int64_t full_size, int prime );

void slf_test();

// seperate
void seperate_large_file( const char* src_root, const char* dst_root, SlfSetting& setting );
int analysize_file( const char* src_path, SlfSetting& setting );
int seperate_body( const char* dst_root, FILE *fp, SlfSetting& setting );
int seperate_tail( const char* dst_root, FILE *fp, SlfSetting& setting );
int output_info( const char* dst_root, SlfSetting& setting );

// merge
void merge_large_file( const char* src_root, const char* dst_root, SlfSetting& setting );
int slf_load_info( const char* src_root, SlfSetting& setting );
int merge_body( const char* src_root, FILE *dst_fp, SlfSetting& setting );
int merge_tail( const char* src_root, FILE *dst_fp, SlfSetting& setting );

void test_seperate();
void test_seperate2();

#endif