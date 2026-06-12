#include "other_test.h"
#include <iostream>
#include <string>
#include <string.h>

using namespace std;



void write_disk_to_full_test( const char *path )
{
    FILE* fp = fopen( path, "wb+");
    int count = 0;
    while (true)
    {
        if (count % 100000 == 0)
            printf("count = %d\n", count);
        count++;
        char buf[] = "1234";
        int res = fwrite(buf, 4, 1, fp);
        if (res <= 0)
            break;
    }

    fclose(fp);
}





void ref_test()
{
    int a = 10;
    int b = 15;
    int &c = (a = b);
    c -= 100;
    cout << a << " " << b << " " << c << "\n";
    // 所以 operator = 回傳的會是 reference.
    // a 的值同 c 的值, c 是 a 的ref.
}




void raw_string_test()
{
    std::string str = R"(\w{2}\s*\d{5}(-\d{4})?)";
    cout << str << endl;
}



/*
N = 3
block=4
shift=2


0   3   6   9   12
0   1   2   3   4

1   4   7   10   13
5   6   7   8    9

2   5   8   11   14
10  11  12  13   14




block=5
shift=0

0   3   6   9   12
0   1   2   3   4

1   4   7   10   13
5   6   7   8    9

2   5   8   11   14
10  11  12  13   14

15
15




count = 0
i = 0; i < N; i++
    j = i; j < SIZE; j += N;
        j mapping to count
        count++

index = 0; index < SIZE; index++
    block = SIZE / N
    shift = SIZE % N
    if index in 0 <= <= block
        map = index*N
    if index in block+1 <= <= block*2+1
        map = (index-block-1)*N + 1
    if index in block*2+2 <= <= SIZE
        map = (index-block*2-2)*N + 2
*/




void seperate_large_file( const char* path )
{
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

    // 有錯, offset計算公式錯
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

    return;


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


    fclose(src_fp);
}













#if 1
// 這個 sizeof = 4.  
struct BitFieldObj
{
    unsigned int a : 1;    
    unsigned int b : 1;
    unsigned int c : 1;
    unsigned int d : 1;
};
#else
// 這個 sizeof = 16.   4byte * 4
struct BitFieldObj
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
};
#endif


struct BitFieldZero
{
    unsigned int a : 1;    
    unsigned int b : 1;
    unsigned int   : 0;  // 加上這行, BitFieldObj 的 size 會變成 8. 強制底下的變數位移到下一個4byte.  
    unsigned int c : 1;
    unsigned int d : 1;
};


/*
zero-width bit field

Bit fields with a length of 0 must be unnamed. Unnamed bit fields cannot be referenced or initialized. A zero-width bit field can cause the next field 
to be aligned on the next container boundary where the container is the same size as the underlying type of the bit field.
看敘述也是記憶體對齊用的做法.
https://hackmd.io/wF3UvixFQUWfOYDEyPW9_w
*/
void bit_field_test()
{
    /* 
        windows, vs2019, 只有 4 byte.  
        這是 bit field的作用.
    */
    cout << "sizeof BitFieldObj = " << sizeof(BitFieldObj) << endl;  

    BitFieldObj obj;
    unsigned char value = 0x0A;
    memcpy( &obj, &value, 1 );

    /* 
        印出 1 0 1 0   1010 = A (16進位)  
        bit field的作用, 只佔 1 bit. 讓 struct 結構可以壓縮到 1 byte.
        如果 a 作用同 bool, 可以只用 1 bit 表示 true/false
        embedded, linux kernel比較有機會遇到
    */
    cout << obj.d << " " << obj.c << " " << obj.b << " " << obj.a << endl;


    // 加上zero-width bit field後執行結果跟預期的不同,有空在研究了.
    cout << "sizeof BitFieldZero = " << sizeof(BitFieldZero) << endl;

    // 底下的code, debug遇到crash. 有空再研究吧.
#if 0
    BitFieldZero zero;
    unsigned short zv = 0;
    memset( &zero, 0, 8 );

    zero.a = 0;
    zero.b = 1;
    zero.c = 1;
    zero.d = 0;
    memcpy( &zv, &zero, 8 );
    printf( "%x", zv );
#endif
}
