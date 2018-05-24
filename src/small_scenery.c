#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"


static error_t animation_indices_decode(animation_indices_t* animation_indices,uint8_t* data,uint32_t data_length,uint32_t* length)
{
uint32_t pos=0;
	while(pos<data_length&&data[pos]!=0xFF)pos++;
	if(pos>=data_length)return ERROR_PREMATURE_END_OF_CHUNK;
animation_indices->num_indices=pos;
animation_indices->indices=malloc(animation_indices->num_indices);
memcpy(animation_indices->indices,data,animation_indices->num_indices);

*length=pos+1;
return ERROR_NONE;
}
static uint32_t animation_indices_get_encoded_length(animation_indices_t* animation_indices)
{
return animation_indices->num_indices+1;
}
static void animation_indices_encode(animation_indices_t* animation_indices,uint8_t* data)
{
memcpy(data,animation_indices->indices,animation_indices->num_indices);
data[animation_indices->num_indices]=0xFF;
}

error_t small_scenery_decode(small_scenery_t* scenery,chunk_t* chunk)
{
error_t error;
//Load header chunk->data
	if(chunk->length<0x1C)
	{
	return ERROR_PREMATURE_END_OF_CHUNK;
	}
scenery->flags=*((uint32_t*)(chunk->data+6));
scenery->height=chunk->data[10];
scenery->cursor_sel=chunk->data[11];
scenery->build_fee=*((int16_t*)(chunk->data+12));
scenery->remove_fee=*((int16_t*)(chunk->data+14));
scenery->anim_delay=*((int16_t*)(chunk->data+20));
scenery->anim_parameters=*((int16_t*)(chunk->data+22));
scenery->anim_frames=*((int16_t*)(chunk->data+24));

uint32_t pos=0x1C;

//Load string table
uint32_t length;
error=string_table_decode(&(scenery->name),chunk->data+pos,chunk->length-pos,&length);
	if(error!=ERROR_NONE)return error;
pos+=length;

//Load group info
error=object_header_decode(&(scenery->group_info),chunk->data+pos,length-pos);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery->name));
	return error;
	}
pos+=16;

//Load animation indices (if present)
	if(scenery->flags&SMALL_SCENERY_ANIMDATA)
	{
	error=animation_indices_decode(&(scenery->animation_indices),chunk->data+pos,length-pos,&length);
		if(error!=ERROR_NONE)
		{
		string_table_destroy(&(scenery->name));
		return error;
		}
	pos+=length;
	}
//Load images
error=image_list_decode(&(scenery->sprites),chunk->data+pos,chunk->length-pos);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery->name));
	return error;
	}

return ERROR_NONE;
}

error_t small_scenery_encode(small_scenery_t* scenery,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(scenery->name));
uint32_t animation_indices_length=scenery->flags&SMALL_SCENERY_ANIMDATA?animation_indices_get_encoded_length(&(scenery->animation_indices)):0;
uint32_t sprites_length=image_list_get_encoded_length(&(scenery->sprites));
uint32_t length=0x2C+name_table_length+animation_indices_length+sprites_length;

//Allocate memory
chunk->encoding=encoding;
chunk->data=malloc_or_die(length);
chunk->length=length;
memset(chunk->data,0,length);

//Write header
*((uint32_t*)(chunk->data+6))=scenery->flags;
chunk->data[10]=scenery->height;
chunk->data[11]=scenery->cursor_sel;
*((int16_t*)(chunk->data+12))=scenery->build_fee;;
*((int16_t*)(chunk->data+14))=scenery->remove_fee;
*((int16_t*)(chunk->data+20))=scenery->anim_delay;
*((int16_t*)(chunk->data+22))=scenery->anim_parameters;
*((int16_t*)(chunk->data+24))=scenery->anim_frames;

uint32_t pos=0x1C;
string_table_encode(&(scenery->name),chunk->data+pos);
pos+=name_table_length;
object_header_encode(&(scenery->group_info),chunk->data+pos);
pos+=16;
	if(scenery->flags&SMALL_SCENERY_ANIMDATA)
	{
	animation_indices_encode(&(scenery->animation_indices),chunk->data+pos);
	pos+=animation_indices_length;
	}
image_list_encode(&(scenery->sprites),chunk->data+pos);
return ERROR_NONE;
}

void small_scenery_destroy(small_scenery_t* scenery)
{
string_table_destroy(&(scenery->name));
	if(scenery->flags&SMALL_SCENERY_ANIMDATA)free(scenery->animation_indices.indices);
image_list_destroy(&(scenery->sprites));
}

