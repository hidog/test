
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <openssl/aes.h>  

int aes_test() 
{
    AES_KEY aes;  
    unsigned char key[AES_BLOCK_SIZE];        // AES_BLOCK_SIZE = 16  
    unsigned char iv[AES_BLOCK_SIZE];        // init vector  
    char input_string[17] = "1234567890abcdef";
    unsigned char encrypt_string[17];
    char decrypt_string[17] = {0};
    unsigned int len;        // encrypt length (in multiple of AES_BLOCK_SIZE) AES_BLOCK_SIZE = 16
    unsigned int i;
    int ret;

    // set the encryption length  
    len = 16; 

    // Generate AES 128-bit key  
    for( i = 0; i < 16; ++i )
        key[i] = 32 + i;      

    // Set encryption key  
    for( i = 0; i < AES_BLOCK_SIZE; ++i )   
        iv[i] = 0;  
    
    ret =  AES_set_encrypt_key(key, 128, &aes);
    if( ret < 0 ) 
    {
        fprintf(stderr, "Unable to set encryption key in AES\n");  
        return -1;
    }  

    // encrypt (iv will change)
    unsigned char *input_str_ptr = (unsigned char*)input_string;
    AES_cbc_encrypt( input_str_ptr, encrypt_string, len, &aes, iv, AES_ENCRYPT);  

    printf("encrypt string : \n");
    for( i = 0; i < len; i++ )
        printf("%d ", encrypt_string[i] );
    printf("\n");

    // Set decryption key  
    for( i = 0; i < AES_BLOCK_SIZE; ++i )
        iv[i] = 0;
    
    ret = AES_set_decrypt_key( key, 128, &aes );
    if( ret < 0 ) 
    {
        fprintf(stderr, "Unable to set decryption key in AES\n");  
        return -1;
    }  

    // decrypt
    unsigned char *output_ptr = (unsigned char*)decrypt_string;
    AES_cbc_encrypt( encrypt_string, output_ptr, len, &aes, iv, AES_DECRYPT );

    printf( "input str = %s\n", input_string );
    printf( "output str = %s\n", decrypt_string );

    return 0;  
}  