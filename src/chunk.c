#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "object.h"


static uint32_t rle_decoded_length(uint8_t* bytes,uint32_t length)
{
uint32_t decoded_length=0;
/*Decode RLE data*/
uint32_t pos=0;
    while(pos<length)
    {
        /*encoded_bytes is left unsigned for the sake of consistency, so test negativity by looking at the high order bit*/
        if(bytes[pos]&0x80)
        {
        uint8_t num_to_repeat=1-bytes[pos];
        pos+=2;
        decoded_length+=num_to_repeat;
        }
        else
        {
        uint16_t num_to_copy=(uint16_t)(bytes[pos])+1;
        pos+=num_to_copy+1;
        decoded_length+=num_to_copy;
        }
    }
return decoded_length;
}
static int rle_decode(uint8_t* encoded_bytes,uint32_t encoded_length,uint8_t* decoded_bytes,uint32_t decoded_length)
{
/*Decode RLE data*/
uint32_t encoded_pos=0;
uint32_t decoded_pos=0;
    while(encoded_pos<encoded_length)
    {
        /*encoded_bytes is left unsigned for the sake of consistency, so test negativity by looking at the high order bit*/
        if(encoded_bytes[encoded_pos]&0x80)
        {
        uint8_t num_to_repeat=1-encoded_bytes[encoded_pos++];
            if(encoded_pos>=encoded_length||decoded_pos+num_to_repeat>decoded_length)
            {
            printf("repeat fail %d-%d %d-%d copying %d bytes\n",encoded_pos,encoded_length,decoded_pos,decoded_length,num_to_repeat);
            return 1;
            }
        memset(decoded_bytes+decoded_pos,encoded_bytes[encoded_pos],num_to_repeat);
        encoded_pos++;
        decoded_pos+=num_to_repeat;
        }
        else
        {
        uint16_t num_to_copy=(uint16_t)(encoded_bytes[encoded_pos++])+1;
            if(encoded_pos+num_to_copy>encoded_length||decoded_pos+num_to_copy>decoded_length)
            {
            printf("copy fail\n");
            return 1;
            }
        memcpy(decoded_bytes+decoded_pos,encoded_bytes+encoded_pos,num_to_copy);
        encoded_pos+=num_to_copy;
        decoded_pos+=num_to_copy;
        }
    }
    if(encoded_pos!=encoded_length||decoded_pos!=decoded_length)return 1;
return 0;
}

static uint32_t count_repeated_bytes(uint8_t* bytes,uint32_t length)
{
    if(length==0)return 0;
uint32_t pos=0;
uint8_t first_byte=bytes[0];
    do
    {
    pos++;
    }while(pos<length&&bytes[pos]==first_byte&&pos<125);
return pos;
}
static uint32_t count_differing_bytes(uint8_t* bytes,uint32_t length)
{
    if(length==0)return 0;
uint32_t pos=0;
    while(pos<length&&(pos+1==length||bytes[pos]!=bytes[pos+1])&&pos<125)
    {
    pos++;
    }
return pos;
}
static uint32_t rle_encoded_length(uint8_t* bytes,uint32_t length)
{
uint32_t encoded_length=0;

uint32_t pos=0;
    while(pos<length)
    {
    int repeated_bytes=count_repeated_bytes(bytes+pos,length-pos);
        if(repeated_bytes>=2)
        {
        encoded_length+=2;
        pos+=repeated_bytes;
        }
        else
        {
        int differing_bytes=count_differing_bytes(bytes+pos,length-pos);
        encoded_length+=differing_bytes+1;
        pos+=differing_bytes;
        }
    }
return encoded_length;
}
static uint32_t rle_encode(uint8_t* decoded_bytes,uint32_t decoded_length,uint8_t* encoded_bytes)
{
/*Encode RLE data*/
uint32_t encoded_pos=0;
uint32_t decoded_pos=0;
    while(decoded_pos<decoded_length)
    {
    int repeated_bytes=count_repeated_bytes(decoded_bytes+decoded_pos,decoded_length-decoded_pos);
        if(repeated_bytes>=2)
        {
        encoded_bytes[encoded_pos]=~((uint8_t)repeated_bytes)+2;
        encoded_bytes[encoded_pos+1]=decoded_bytes[decoded_pos];
        encoded_pos+=2;
        decoded_pos+=repeated_bytes;
        }
        else
        {
        int differing_bytes=count_differing_bytes(decoded_bytes+decoded_pos,decoded_length-decoded_pos);
        encoded_bytes[encoded_pos]=(uint8_t)differing_bytes-1;
        memcpy(encoded_bytes+encoded_pos+1,decoded_bytes+decoded_pos,differing_bytes);
        encoded_pos+=differing_bytes+1;
        decoded_pos+=differing_bytes;
        }
    }
return 0;
}

error_t chunk_decode(chunk_t* chunk,uint8_t** data,uint32_t* length)
{
    switch(chunk->encoding)
    {
    //Copy encoded data directly to output
    case ENCODING_NONE:
    *length=chunk->length;
    *data=malloc(chunk->length);
    memcpy(*data,chunk->data,chunk->length);
    break;
    case ENCODING_RLE:
    *length=rle_decoded_length(chunk->data,chunk->length);
    *data=malloc_or_die(*length);
        if(rle_decode(chunk->data,chunk->length,*data,*length))
        {
        free(*data);
        return ERROR_INVALID_CHUNK;
        }
    break;
    default:
    return ERROR_INVALID_ENCODING;
    break;
    }
//FILE* test=fopen("DECOMPRESSED.DAT","wb");
//fwrite(*data,1,*length,test);
//fclose(test);
return ERROR_NONE;
}
void chunk_encode(chunk_t* chunk,uint8_t encoding,uint8_t *data,uint32_t length)
{
chunk->encoding=encoding;
    switch(encoding)
    {
    case ENCODING_NONE:
    chunk->length=length;
    chunk->data=malloc(length);
    memcpy(chunk->data,data,length);
    break;
    case ENCODING_RLE:
    chunk->length=rle_encoded_length(data,length);
    chunk->data=malloc(chunk->length);
    rle_encode(data,length,chunk->data);
    break;
    default:
    assert(0);
    break;
    }
}


error_t chunk_read(chunk_t* chunk,FILE* file)
{
chunk_t chunk_enc;
//Check we can read header safely
    if(fread(&(chunk_enc.encoding),1,1,file)!=1)return ERROR_FILE_OPERATION_FAILED;
    if(fread(&(chunk_enc.length),4,1,file)!=1)return ERROR_FILE_OPERATION_FAILED;

chunk_enc.data=malloc(chunk_enc.length);

    if(fread(chunk_enc.data,1,chunk_enc.length,file)!=chunk_enc.length)
    {
    free(chunk_enc.data);
    return feof(file)?ERROR_PREMATURE_EOF:ERROR_FILE_OPERATION_FAILED;
    }

error_t error=chunk_decode(&chunk_enc,&(chunk->data),&(chunk->length));
chunk->encoding=chunk_enc.encoding;
free(chunk_enc.data);
return error;
}
error_t chunk_write(chunk_t* chunk,FILE* file)
{
chunk_t chunk_enc;
chunk_encode(&chunk_enc,chunk->encoding,chunk->data,chunk->length);
//Write header
    if(fwrite(&(chunk_enc.encoding),1,1,file)!=1)return ERROR_FILE_OPERATION_FAILED;
    if(fwrite(&(chunk_enc.length),4,1,file)!=1)return ERROR_FILE_OPERATION_FAILED;
    if(fwrite(chunk_enc.data,1,chunk_enc.length,file)!=chunk_enc.length)return ERROR_FILE_OPERATION_FAILED;
return ERROR_NONE;
}



void chunk_destroy(chunk_t* chunk)
{
free(chunk->data);
}










/*
void buffer_init(buffer_t* buffer)
{
buffer->size=0;
buffer->allocated=1024;
buffer->data=malloc(buffer->allocated*sizeof(uint8_t));
}
void buffer_preallocate(buffer_t* buffer,uint32_t num_bytes)
{
uint32_t required_size=buffer->size+num_bytes;
    if(buffer->allocated<required_size)
    {
        do buffer->allocated*=2;
        while(buffer->allocated<required_size);
    buffer->data=realloc(buffer->data,buffer->allocated*sizeof(uint8_t));
    }
buffer->size+=num_bytes;
}
void buffer_write(buffer_t* buffer,uint8_t* bytes,uint32_t num_bytes)
{
uint32_t cur_size=buffer->size;
buffer_expand(buffer,num_bytes);
memcpy(buffer->data+cur_size,bytes,num_bytes);
}
void buffer_free(buffer_t* buffer)
{
free(buffer->data);
}
*/



