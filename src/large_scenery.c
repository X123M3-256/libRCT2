#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"


error_t large_scenery_tile_info_decode(large_scenery_tile_info_t* tile_info,uint8_t* data,uint32_t data_length,uint32_t* length)
{
tile_info->num_tiles=0;

int pos=0;
	while(pos+1<data_length&&*((uint16_t*)(data+pos))!=0xFFFF)
	{
	tile_info->num_tiles++;
	pos+=9;
	}
	if(pos+1>=data_length)return ERROR_PREMATURE_END_OF_CHUNK;
tile_info->tiles=malloc(tile_info->num_tiles*sizeof(large_scenery_tile_t));
	for(int i=0;i<tile_info->num_tiles;i++)
	{
	tile_info->tiles[i].x=*((int16_t*)(data+9*i));
	tile_info->tiles[i].y=*((int16_t*)(data+9*i+2));
	tile_info->tiles[i].base_height=*((int16_t*)(data+9*i+4));
	tile_info->tiles[i].clearance_height=data[9*i+6];
	tile_info->tiles[i].flags=data[9*i+7];
	tile_info->tiles[i].quadrants=data[9*i+8]>>4;
	tile_info->tiles[i].walls=data[9*i+8]&0xF;
	}
*length=pos+2;
return ERROR_NONE;
}
uint32_t large_scenery_tile_info_get_encoded_length(large_scenery_tile_info_t* tile_info)
{
return tile_info->num_tiles*9+2;
}
void large_scenery_tile_info_encode(large_scenery_tile_info_t* tile_info,uint8_t* data)
{
	for(int i=0;i<tile_info->num_tiles;i++)
	{
	*((int16_t*)(data+9*i))=tile_info->tiles[i].x;
	*((int16_t*)(data+9*i+2))=tile_info->tiles[i].y;
	*((int16_t*)(data+9*i+4))=tile_info->tiles[i].base_height;
	data[9*i+6]=tile_info->tiles[i].clearance_height;
	data[9*i+7]=tile_info->tiles[i].flags;
	data[9*i+8]=tile_info->tiles[i].walls|(tile_info->tiles[i].quadrants<<4);
	}
data[tile_info->num_tiles*9]=0xFF;
data[tile_info->num_tiles*9+1]=0xFF;
}

error_t large_scenery_decode(large_scenery_t* scenery,chunk_t* chunk)
{
error_t error;
//Load header chunk->data
	if(chunk->length<0x1A)return ERROR_PREMATURE_END_OF_CHUNK;


scenery->cursor_sel=chunk->data[6];
scenery->flags=chunk->data[7];
scenery->build_fee=*((int16_t*)(chunk->data+8));
scenery->remove_fee=*((int16_t*)(chunk->data+10));
scenery->scroll=chunk->data[11];

uint32_t pos=0x1A;

//Load string table
uint32_t length;
error=string_table_decode(&(scenery->name),chunk->data+pos,chunk->length-pos,&length);
	if(error!=ERROR_NONE)return error;
pos+=length;

//Load group info
error=object_header_decode(&(scenery->object_header),chunk->data+pos,length-pos);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery->name));
	return error;
	}
pos+=16;

	if(scenery->flags&LARGE_SCENERY_3D_TEXT)
	{
		if(chunk->length<pos+0x40E)
		{
		string_table_destroy(&(scenery->name));
		return ERROR_PREMATURE_END_OF_CHUNK;
		}
	scenery->unknown=malloc(0x40E);
	memcpy(scenery->unknown,chunk->data+pos,0x40E);
	pos+=0x40E;
	}

error=large_scenery_tile_info_decode(&(scenery->tile_info),chunk->data+pos,length-pos,&length);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery->name));
		if(scenery->flags&LARGE_SCENERY_3D_TEXT)free(scenery->unknown);
	return error;
	}
pos+=length;


//Load images
error=image_list_decode(&(scenery->sprites),chunk->data+pos,chunk->length-pos);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery->name));
		if(scenery->flags&LARGE_SCENERY_3D_TEXT)free(scenery->unknown);
	free(scenery->tile_info.tiles);
	return error;
	}

return ERROR_NONE;
}

error_t large_scenery_encode(large_scenery_t* scenery,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(scenery->name));
uint32_t tile_info_length=large_scenery_tile_info_get_encoded_length(&(scenery->tile_info));
uint32_t sprites_length=image_list_get_encoded_length(&(scenery->sprites));
uint32_t length=0x2A+name_table_length+tile_info_length+sprites_length;


//Allocate memory
chunk->encoding=encoding;
chunk->data=malloc_or_die(length);
chunk->length=length;
memset(chunk->data,0,length);



//Write header
chunk->data[6]=scenery->cursor_sel;
chunk->data[7]=scenery->flags;
*((int16_t*)(chunk->data+8))=scenery->build_fee;
*((int16_t*)(chunk->data+10))=scenery->remove_fee;
chunk->data[11]=scenery->scroll;
chunk->data[17]=0xFF;


uint32_t pos=0x1A;
string_table_encode(&(scenery->name),chunk->data+pos);
pos+=name_table_length;
object_header_encode(&(scenery->object_header),chunk->data+pos);
pos+=16;

	if(scenery->flags&LARGE_SCENERY_3D_TEXT)
	{
	memcpy(chunk->data+pos,scenery->unknown,0x40E);
	pos+=0x40E;
	}

large_scenery_tile_info_encode(&(scenery->tile_info),chunk->data+pos);
pos+=tile_info_length;

image_list_encode(&(scenery->sprites),chunk->data+pos);
return ERROR_NONE;
}


void large_scenery_destroy(large_scenery_t* scenery)
{
string_table_destroy(&(scenery->name));
image_list_destroy(&(scenery->sprites));
	if(scenery->flags&LARGE_SCENERY_3D_TEXT)free(scenery->unknown);
free(scenery->tile_info.tiles);
}

