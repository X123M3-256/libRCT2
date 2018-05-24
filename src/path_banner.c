#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

error_t path_banner_decode(path_banner_t* path_banner,chunk_t* chunk)
{
error_t error;
//Load header
    if(chunk->length<0xC)return ERROR_PREMATURE_END_OF_CHUNK;
path_banner->scroll=chunk->data[6];
path_banner->flags=chunk->data[7];
path_banner->build_fee=*((int16_t*)(chunk->data+8));

uint32_t length;
uint32_t pos=0xC;
//Load string table
error=string_table_decode(&(path_banner->name),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)return error;
pos+=length;
//Load group info
error=object_header_decode(&(path_banner->object_header),chunk->data+pos,length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(path_banner->name));
    return error;
    }
pos+=16;
//Load images
error=image_list_decode(&(path_banner->sprites),chunk->data+pos,chunk->length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(path_banner->name));
    return error;
    }
return ERROR_NONE;
}
/*error_t path_banner_encode(path_banner_t* path_banner,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(path_banner->name));
uint32_t sprites_length=image_list_get_encoded_length(&(path_banner->sprites));
uint32_t length=0x1E +name_table_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
data[6]=path_banner->cursor_sel;
data[7]=path_banner->flags;
data[8]=path_banner->clearance;
data[9]=path_banner->effects;
*((int16_t*)(data+10))=path_banner->build_fee;
data[13]=path_banner->scrolling;

uint32_t pos=0xE;
string_table_encode(&(path_banner->name),data+pos);
pos+=name_table_length;
object_header_encode(&(path_banner->object_header),data+pos);
pos+=16;
image_list_encode(&(path_banner->sprites),data+pos);
chunk_encode(chunk,encoding,data,length);
free(data);
return ERROR_NONE;
}
*/
void path_banner_destroy(path_banner_t* path_banner)
{
string_table_destroy(&(path_banner->name));
image_list_destroy(&(path_banner->sprites));
}
