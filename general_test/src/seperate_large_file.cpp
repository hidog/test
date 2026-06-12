#include <iostream>

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

#if 0
    for( index = 0; index <= SIZE; index++ )
    {
        offset = SIZE / N;
        if( 0 <= index && index <= offset )
            map = index*N;
        else if( offset+1 <= index && index <= offset*2+1 )
            map = (index-offset-1)*N + 1;
        else
            map = (index-offset*2-2)*N +2;
        printf( "%d => %d \n", index, map );
    }
    for( i = SIZE+1; i <= INPUT_SIZE; i++ )
        printf("%d => %d\n", i, i);
#endif
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

#if 0
    int64_t result = input*PRIME

    int INPUT_SIZE = 15;
    int N = 3;
    int SIZE = ((INPUT_SIZE+1)/N)*N;
    int count = 0;
    int i, j;
    int index;
    int offset;
    int map;

    count = 0;
    for( i = 0; i < N; i++ )
    {
        for( j = i; j <= SIZE; j += N )
        {
            printf( " %d => %d \n", j, count );
            count++;
        }
    }
    for( i = SIZE+1; i <= INPUT_SIZE; i++ )
        printf("%d => %d\n", i, i);
    printf("\n\n\n");
#endif
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










void seperate_large_file( const char* path )
{
    char dst_root_path[1000] = "I:\\temp\\test\\";
    constexpr int64_t block_size = 10000;

    FILE* src_fp = fopen(path, "rb");
    if (src_fp == NULL)
    {
        printf("open fail.\n");
        return;
    }

    // 取得檔案大小
    fseek( src_fp, 0, SEEK_END );
    __int64 file_size = _ftelli64( src_fp );

    printf( "file size : %lld\n", file_size);
    printf( "%lld\n", file_size/block_size);

    int64_t repeat_size = file_size/block_size;
    int64_t index;
    size_t read_size;

    fseek( src_fp, 0, SEEK_SET );
    uint8_t buffer[block_size] = {0};

    for( index = 0; index <= repeat_size; index++ )
    {
        printf("solve: %lld\n", index);

        read_size = fread( buffer, sizeof(uint8_t), block_size, src_fp );
        if( read_size != block_size )
            printf("error\n");

        char dst_path[1000];
        int64_t mapping = slf_f(index, repeat_size, 2069);
        sprintf( dst_path, "%s\\db_%lld.dat", dst_root_path, mapping );
        FILE* dst_fp = fopen(dst_path, "wb+");
        fwrite( buffer, sizeof(uint8_t), block_size, dst_fp );
        fclose(dst_fp);
    }

    char dst_path[1000];
    sprintf( dst_path, "%s\\db_tail.dat", dst_root_path );
    FILE* dst_fp = fopen(dst_path, "wb+");
    int64_t remain_size = file_size % block_size;
    int tmp = remain_size;
    fwrite( buffer, sizeof(uint8_t), tmp, dst_fp );
    fclose(dst_fp);

    fclose(src_fp);
}




