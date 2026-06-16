#include <iostream>
#include <Windows.h>
#include "seperate_large_file.h"

using namespace std;




/*
N = 3
block=4

0   3   6   9   12
0   1   2   3   4

1   4   7   10   13
5   6   7   8    9

2   5   8   11   14
10  11  12  13   14

count = 0
i = 0; i < N; i++
    j = i; j < SIZE; j += N;
        j mapping to count
        count++

index = 0; index < SIZE; index++
    block = SIZE / N
    if index in 0 <= <= block
        map = index*N
    if index in block+1 <= <= block*2+1
        map = (index-block-1)*N + 1
    if index in block*2+2 <= <= SIZE
        map = (index-block*2-2)*N + 2
*/





int64_t slf_f(int64_t input, int64_t full_size, int prime )
{
    int64_t size = full_size%prime == 0 ? full_size-1 : (full_size/prime)*prime - 1;

    if( input >= size )
        return input;

    int64_t result = 0;
    int64_t offset = size/prime;
    int64_t n;

    for( n = 0; n < prime; n++ )
    {
        if( offset*n + n <= input && input <= offset*(n + 1) + n)
        {
            result = (input - offset*n - n)*prime +n;
            break;
        }
    }

    return result;
}




int64_t slf_g( int64_t input, int64_t full_size, int prime )
{
    int64_t size = full_size%prime == 0 ? full_size-1 : (full_size/prime)*prime - 1;

    if( input >= size )
        return input;

    int64_t quotient = input/prime;
    int64_t remainder = input%prime;
    int64_t offset = size/prime;

    int64_t n;
    int64_t result = offset*remainder + quotient + remainder;

    return result;
}





void slf_test()
{
    auto func = []( int64_t full_size, int prime )
    {
        int64_t x, y, z;
        for( x = 0; x <= full_size; x++ )
        {
            y = slf_f( x, full_size, prime );
            z = slf_g( y, full_size, prime );
            printf("%lld -> %lld, -> %lld\n", x, y, z);
            if( x != z )
                printf("test fail on x = %lld, y = %lld\n", x, y);
        }
    };    

    func( 59, 2069 );

    func( 15, 3 );
    func( 16, 3 );
    func( 17, 3 );
    func( 18, 3 );
    func( 5000, 3 );
    func( 5001, 3 );
    func( 5002, 3 );
    func( 5003, 3 );

    int64_t fz;
    for( fz = 123; fz <= 123 + 202; fz++ )
        func( fz, 101 );

    for( fz = 2259; fz <= 2259 + 2069*3; fz++ )
        func( fz, 2069);
}





//constexpr int64_t block_size = 1000000;
//constexpr int prime = 2069;





int analysize_file( const char* src_path, SlfSetting& setting )
{
    int block_size = setting.block_size;

    FILE* src_fp = fopen( src_path, "rb" );
    if (src_fp == NULL)
    {
        printf("analysize_file open fail.\n");
        return -1;
    }

    // 取得檔案大小
    fseek( src_fp, 0, SEEK_END );
    __int64 file_size = _ftelli64( src_fp );
    int repeat_size = file_size/block_size;
    fclose( src_fp );

    printf( "file size : %lld\n", file_size );
    printf( "total size %lld\n", file_size/block_size );

    if( repeat_size < setting.prime + 100 )
    {
        printf("analysize_file total size is too small\n");
        return -1;
    }
    if( repeat_size < 0 )
    {
        printf("analysize_file repeat_size = %d error\n", repeat_size);
        return -1;
    }

    setting.repeat_size = repeat_size;

    auto func = []( int64_t full_size, int prime )
    {
        int64_t x, y, z;
        for( x = 0; x <= 20; x++ )
        {
            y = slf_f( x, full_size, prime );
            z = slf_g( y, full_size, prime );
            printf("%lld -> %lld, -> %lld\n", x, y, z);
            if( x != z )
                printf("test fail on x = %lld, y = %lld\n", x, y);
        }
    };
    printf("start mapping test\n");
    func( repeat_size, setting.prime );

    return 1;
}




int seperate_body( const char* dst_root, FILE *fp, SlfSetting& setting )
{
    uint8_t *buffer = nullptr;
    buffer = new uint8_t[setting.block_size];
    if( buffer == NULL )
    {
        printf("seperate_body error, allocate buffer fail.\n");
        return -1;
    }

    int index = 0, read_size = 0, write_size = 0;
    int block_size = setting.block_size;
    int repeat_size = setting.repeat_size;
    int prime = setting.prime;
    char dst_path[1000];
    int mapping = 0;
    FILE *dst_fp = NULL;

    for( index = 0; index < repeat_size; index++ )
    {
        if( index % 1000 == 0 )
            printf("solve: %d\n", index);

        read_size = fread( buffer, sizeof(uint8_t), block_size, fp );
        if( read_size != block_size )
        {
            printf("seperate_body error, read size error.\n");
            return -1;
        }

        mapping = slf_f(index, repeat_size, prime);

        if( index % 1000 == 0 )
            printf( "index %d -> mapping %d\n",index, mapping );

        sprintf( dst_path, "%s\\db_%d.dat", dst_root, mapping );
        dst_fp = fopen(dst_path, "wb+");
        if( dst_fp == NULL )
        {
            printf("seperate_body open dst_fp fail\n");
            return -1;
        }
        write_size = fwrite( buffer, sizeof(uint8_t), read_size, dst_fp );
        if( write_size != read_size )
        {
            printf("seperate_body write error\n");
            return -1;
        }
        fclose(dst_fp);

        if( index % 10000 == 0 )
            Sleep(1000);
    }

    delete [] buffer;
    return 1;
}





int seperate_tail( const char* dst_root, FILE *fp, SlfSetting& setting )
{
    uint8_t *buffer = nullptr;
    buffer = new uint8_t[setting.block_size];
    if( buffer == NULL )
    {
        printf("seperate_tail error, allocate buffer fail.\n");
        return -1;
    }

    char dst_path[1000];
    int read_size = 0, write_size = 0;
    int block_size = setting.block_size;

    read_size = fread( buffer, sizeof(uint8_t), block_size, fp );
    printf("tail size = %d\n", read_size);
    sprintf( dst_path, "%s\\db_tail.dat", dst_root );
    FILE* dst_fp = fopen(dst_path, "wb+");
    if( dst_fp == NULL )
    {
        printf("seperate_tail open tail file error\n");
        return -1;
    }
    write_size = fwrite( buffer, sizeof(uint8_t), read_size, dst_fp );
    setting.tail_size = write_size;
    if( write_size != read_size )
    {
        printf("seperate_tail write tail error\n");
        return -1;
    }
    fclose(dst_fp);

    delete [] buffer;
    return 1;
}




int output_info( const char* dst_root, SlfSetting& setting )
{
    int res;
    char file_info_path[1000];
    sprintf( file_info_path, "%s\\db_info.inf", dst_root );
    FILE* fp = fopen( file_info_path, "wb+" );
    if( fp == NULL )
    {
        printf("output_info open file error\n");
        return -1;
    }
    setting.prime = 3;
    res = fwrite( &setting, sizeof(setting), 1, fp );
    fclose(fp);

    return 1;
}





void seperate_large_file( const char* src_root, const char* dst_root, SlfSetting& setting )
{
    int res = 0;
    char src_path[1000];
    sprintf( src_path, "%s\\%s", src_root, setting.filename );
    
    res = analysize_file(src_path, setting);
    if( res < 0 )
    {
        printf("analysize_file error.");
        return;
    }

    FILE *src_fp;

    src_fp = fopen( src_path, "rb" );
    if( src_fp == NULL )
    {
        printf("seperate_large_file open src file fail.\n");
        return;
    }

    res = seperate_body( dst_root, src_fp, setting );
    if( res < 0 )
    {
        printf("seperate_large_file seperate body fail\n");
        return;
    }

    res = seperate_tail( dst_root, src_fp, setting );
    if( res < 0 )
    {
        printf("seperate_large_file seperate tail fail\n");
        return;
    }
    fclose(src_fp);

    res = output_info( dst_root, setting );
}





int slf_load_info( const char* src_root, SlfSetting& setting )
{
    int res = 0;
    char path[1000];
    sprintf( path, "%s\\db_info.inf", src_root );
    FILE* fp = fopen( path, "rb" );
    if( fp == NULL )
    {
        printf("slf_load_info load file error\n");
        return -1;
    }
    res = fread( &setting, sizeof(setting), 1, fp );
    if( res <= 0 )
    {
        printf("slf_load_info load info fail\n");
        return -1;
    }
    fclose(fp);

    return 1;
}





int merge_body( const char* src_root, FILE *dst_fp, SlfSetting& setting )
{
    int block_size = setting.block_size;
    uint8_t *buffer = nullptr;

    buffer = new uint8_t[block_size];
    if( buffer == NULL )
    {
        printf("merge_body allocate buffer error\n");
        return -1;
    }

    int index;
    int mapping;
    int read_size, write_size;
    int repeat_size = setting.repeat_size;
    int prime = setting.prime;
    char src_path[1000];
    FILE *src_fp = NULL;

    for( index = 0; index < repeat_size; index++ )
    {
        if( index%1000 == 0 )
            printf("solve: %d\n", index);
        mapping = slf_g( index, repeat_size, prime );

        sprintf( src_path, "%s\\db_%d.dat", src_root, mapping );
        src_fp = fopen(src_path, "rb");
        if( src_fp == NULL )
        {
            printf("merge_body open src file error\n");
            return -1;
        }

        read_size = fread( buffer, sizeof(uint8_t), block_size, src_fp );
        if( read_size != block_size )
        {
            printf("merge_body load file error\n");
            return -1;
        }
        fclose(src_fp);

        write_size = fwrite( buffer, sizeof(uint8_t), read_size, dst_fp );
        if( write_size != read_size )
        {
            printf("merge_body write error\n");
            return -1;
        }

        if( index%10000 == 0 )
            Sleep(1000);
    }

    delete [] buffer;
    return 1;
}





int merge_tail( const char* src_root, FILE *dst_fp, SlfSetting& setting )
{
    int block_size = setting.block_size;
    uint8_t *buffer = nullptr;

    buffer = new uint8_t[block_size];
    if( buffer == NULL )
    {
        printf("merge_tail allocate buffer error\n");
        return -1;
    }
    
    char src_path[1000];
    int read_size, write_size;

    sprintf( src_path, "%s\\db_tail.dat", src_root );
    FILE* src_fp = fopen(src_path, "rb");
    if( src_fp == NULL )
    {
        printf("merge_tail open src error\n");
        return -1;
    }    
    read_size = fread( buffer, sizeof(uint8_t), block_size, src_fp );
    if( read_size != setting.tail_size )
    {
        printf("merge_tail read fail\n");
        return -1;
    }
    printf( "tail size = %d", read_size );
    fclose(src_fp);

    write_size = fwrite( buffer, sizeof(uint8_t), read_size, dst_fp );
    if( write_size != read_size )
    {
        printf("merge_tail write error\n");
        return -1;
    }

    delete [] buffer;
}





void merge_large_file( const char* src_root, const char* dst_root, SlfSetting& setting )
{
    if( setting.prime < setting.repeat_size )
    {
        printf("merge_large_file error, setting fail\n");
        return;
    }

    int res = 0;

    char dst_path[1000];
    sprintf( dst_path, "%s\\re_%s", dst_root, setting.filename );
    FILE* dst_fp = fopen( dst_path, "wb+" );
    if( dst_fp == NULL )
    {
        printf("merge_large_file open dst file error\n");
        return;
    }

    res = merge_body( src_root, dst_fp, setting );
    if( res < 0 )
    {
        printf("merge_large_file merge body fail.\n");
        return;
    }
    
    res = merge_tail( src_root, dst_fp, setting );
    if( res < 0 )
    {
        printf("merge_large_file merge tail fail.\n");
        return;
    }

    fclose(dst_fp);
    printf( "merge finish. file: %s\n", dst_path );
}





void test_seperate()
{
    int res = 0, res2 = 0;
    FILE *fp, *fq;
    fp = fopen("I:\\temp\\test2.rar", "rb");
    uint8_t *buff = new uint8_t[30000000];
    res = fread( buff, sizeof(uint8_t), 30000000, fp );

    fq = fopen("I:\\temp\\data1.dat", "wb+");
    res2 = fwrite( buff, sizeof(uint8_t), res, fq );
    fclose(fq);

    res = fread( buff, sizeof(uint8_t), 30000000, fp );

    fq = fopen("I:\\temp\\data2.dat", "wb+");
    res2 = fwrite( buff, sizeof(uint8_t), res, fq );
    fclose(fq);

    delete [] buff;
    fclose(fp);
}



void test_seperate2()
{
    int res = 0, res2 = 0;
    FILE *fp, *fq;
    fp = fopen("I:\\temp\\data1.dat", "rb");
    uint8_t *buff = new uint8_t[30000000];
    res = fread( buff, sizeof(uint8_t), 30000000, fp );
    fclose(fp);

    fq = fopen("I:\\temp\\output.rar", "wb+");
    res2 = fwrite( buff, sizeof(uint8_t), res, fq );   

    fp = fopen("I:\\temp\\data2.dat", "rb");
    res = fread( buff, sizeof(uint8_t), 30000000, fp );
    fclose(fp);

    res2 = fwrite( buff, sizeof(uint8_t), res, fq );

    fclose(fq);
    delete [] buff;
}




