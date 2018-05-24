#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

error_t path_object_decode(path_object_t* path_object,chunk_t* chunk)
{
error_t error;
//Load header
    if(chunk->length<0xE)return ERROR_PREMATURE_END_OF_CHUNK;
path_object->flags=*((uint16_t*)(chunk->data+6));
path_object->subtype=chunk->data[8];
path_object->cursor_sel=chunk->data[9];
path_object->build_fee=*((int16_t*)(chunk->data+10));
path_object->cursor_sel=chunk->data[6];

uint32_t length;
uint32_t pos=0xE;
//Load string table
error=string_table_decode(&(path_object->name),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)return error;
pos+=length;
//Load group info
error=object_header_decode(&(path_object->object_header),chunk->data+pos,length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(path_object->name));
    return error;
    }
pos+=16;
//Load images
error=image_list_decode(&(path_object->sprites),chunk->data+pos,chunk->length-pos);
    if(error!=ERROR_NONE)
    {
    string_table_destroy(&(path_object->name));
    return error;
    }
return ERROR_NONE;
}
/*error_t path_object_encode(path_object_t* path_object,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(path_object->name));
uint32_t sprites_length=image_list_get_encoded_length(&(path_object->sprites));
uint32_t length=0x1E +name_table_length+sprites_length;

//Allocate memory
uint8_t* data=malloc_or_die(length);
memset(data,0,length);

//Write header
data[6]=path_object->cursor_sel;
data[7]=path_object->flags;
data[8]=path_object->clearance;
data[9]=path_object->effects;
*((int16_t*)(data+10))=path_object->build_fee;
data[13]=path_object->scrolling;

uint32_t pos=0xE;
string_table_encode(&(path_object->name),data+pos);
pos+=name_table_length;
object_header_encode(&(path_object->object_header),data+pos);
pos+=16;
image_list_encode(&(path_object->sprites),data+pos);
chunk_encode(chunk,encoding,data,length);
free(data);
return ERROR_NONE;
}*/

void path_object_destroy(path_object_t* path_object)
{
string_table_destroy(&(path_object->name));
image_list_destroy(&(path_object->sprites));
}
