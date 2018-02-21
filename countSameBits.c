#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

int countSameBits(unsigned char *buffer1, unsigned char *buffer2, int len); 
int countZeros(unsigned char value);

int main(int argc, char *argv[]) 
{
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;
    FILE *fptr;
    unsigned char *buffer = NULL;
    unsigned char *flippedBuffer = NULL;
    unsigned char *bufferHash = NULL;
    unsigned char *flippedBufferHash = NULL;
    long bufferSize;
    size_t newLength;
    char lastChar;

    if (argv[1] == NULL) {
        printf("Usage: count-same-bits filename\n");
        exit(1);
    }

    fptr = fopen(argv[1], "rb");

    if (fptr == NULL) {
        printf("file points to null");
        exit(1);
    }

    if(fseek(fptr, 0L, SEEK_END) != 0) {
        printf("SEEK_END not found");
        exit(1);
    }

    bufferSize = ftell(fptr);
    
    if(bufferSize == -1) {

    }

    buffer = malloc(sizeof(unsigned char) * (bufferSize + 1));

    if(fseek(fptr, 0L, SEEK_SET) != 0) {
        printf("SEEK_SET not found");
        exit(1);
    }

    newLength = fread(buffer, sizeof(unsigned char), bufferSize, fptr);

    if(ferror(fptr) != 0) {
        printf("error reading file");
        exit(1);
    }

    fclose(fptr);
    flippedBuffer = malloc(sizeof(unsigned char) * (newLength + 1));
    lastChar = buffer[newLength - 1];
    lastChar = lastChar^1;
    memcpy(flippedBuffer, buffer, newLength);
    flippedBuffer[newLength - 1] = lastChar;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("md5");
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, buffer, newLength);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_destroy(mdctx);

    bufferHash = malloc(sizeof(unsigned char) * md_len);
    printf("MD5 hash for input file [H1] -> ");
    for (i = 0; i < md_len; i++) {
        bufferHash[i] = md_value[i];
        printf("%02x", bufferHash[i]);
    }
    printf("\n");

    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, flippedBuffer, newLength);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_destroy(mdctx);

    flippedBufferHash = malloc(sizeof(unsigned char) * md_len);

    printf("Flipping last bit ...");
    printf("\n");
    printf("MD5 hash for input file [H2] -> ");

    for (i = 0; i < md_len; i++) {
        flippedBufferHash[i] = md_value[i];
        printf("%02x", flippedBufferHash[i]);
    }
    printf("\n");
    printf("Count of similar bits : %d", countSameBits(bufferHash, flippedBufferHash, md_len));
    printf("\n");

    exit(0);
}

int countSameBits(unsigned char *buffer1, unsigned char *buffer2, int len) 
{  
    int count = 0;
    for(int i = 0; i < len; i++) {
        unsigned char temp = buffer1[i]^buffer2[i];
        count += countZeros(temp);
    }

    return count;
}

int countZeros(unsigned char value)
{
    int c;
    for (c = 0; value; c++) {
        value &= value - 1;
    }
    return (8 - c);
}