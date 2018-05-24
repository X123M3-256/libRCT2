#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "object.h"

uint32_t scenery_group_member_info_get_encoded_length(scenery_group_member_info_t* member_info)
{
return member_info->num_members*16+1;
}

error_t scenery_group_member_info_encode(scenery_group_member_info_t* member_info,uint8_t* data)
{
	for(int i=0;i<member_info->num_members;i++)
	{
	object_header_encode(member_info->members+i,data+16*i);
	}
data[member_info->num_members*16]=0xFF;
return ERROR_NONE;
}


error_t scenery_group_member_info_decode(scenery_group_member_info_t* member_info,uint8_t* data,uint32_t data_length,uint32_t* length)
{
member_info->num_members=0;

int pos=0;
	while(pos<data_length&&data[pos]!=0xFF)
	{
	member_info->num_members++;
	pos+=16;
	}
	if(pos>=data_length)return ERROR_PREMATURE_END_OF_CHUNK;

member_info->members=malloc_or_die(member_info->num_members*sizeof(object_header_t));
	for(int i=0;i<member_info->num_members;i++)
	{
	error_t error=object_header_decode(member_info->members+i,data+16*i,data_length-16*i);
		if(error!=ERROR_NONE)
		{
		free(member_info->members);
		return error;
		}
	}
*length=pos+1;
return ERROR_NONE;
}

error_t scenery_group_decode(scenery_group_t* scenery_group,chunk_t* chunk)
{
error_t error;
//Load header
	if(chunk->length<0x10E)return ERROR_PREMATURE_END_OF_CHUNK;

//Load header
scenery_group->num_members=chunk->data[0x106];
scenery_group->unknown_1=chunk->data[0x107];
scenery_group->priority=chunk->data[0x108];
scenery_group->unknown_2=chunk->data[0x109];
scenery_group->entertainers=*((uint32_t*)(chunk->data+0x10A));

uint32_t pos=0x10E;

//Load string table
uint32_t length;
error=string_table_decode(&(scenery_group->name),chunk->data+pos,chunk->length-pos,&length);
	if(error!=ERROR_NONE)return error;
pos+=length;

//Load member info
error=scenery_group_member_info_decode(&(scenery_group->member_info),chunk->data+pos,length-pos,&length);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery_group->name));
	return error;
	}
pos+=length;

//Load images
error=image_list_decode(&(scenery_group->sprites),chunk->data+pos,chunk->length-pos);
	if(error!=ERROR_NONE)
	{
	string_table_destroy(&(scenery_group->name));
	free(scenery_group->member_info.members);
	return error;
	}
return ERROR_NONE;
}
error_t scenery_group_encode(scenery_group_t* scenery_group,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded data
uint32_t name_table_length=string_table_get_encoded_length(&(scenery_group->name));
uint32_t member_info_length=scenery_group_member_info_get_encoded_length(&(scenery_group->member_info));
uint32_t sprites_length=image_list_get_encoded_length(&(scenery_group->sprites));
uint32_t length=0x10E +name_table_length+member_info_length+sprites_length;

//Allocate memory
chunk->encoding=encoding;
chunk->data=malloc_or_die(length);
chunk->length=length;
memset(chunk->data,0,length);

//Write header
chunk->data[0x106]=scenery_group->num_members;
chunk->data[0x107]=scenery_group->unknown_1;
chunk->data[0x108]=scenery_group->priority;
chunk->data[0x109]=scenery_group->unknown_2;
*((uint32_t*)(chunk->data+0x10A))=scenery_group->entertainers;

uint32_t pos=0x10E;
string_table_encode(&(scenery_group->name),chunk->data+pos);
pos+=name_table_length;

scenery_group_member_info_encode(&(scenery_group->member_info),chunk->data+pos);
pos+=member_info_length;

image_list_encode(&(scenery_group->sprites),chunk->data+pos);
return ERROR_NONE;
}

void scenery_group_destroy(scenery_group_t* scenery_group)
{
string_table_destroy(&(scenery_group->name));
image_list_destroy(&(scenery_group->sprites));
free(scenery_group->member_info.members);
}
