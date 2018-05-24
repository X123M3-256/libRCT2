#ifndef SCENERY_GROUP_H_INCLUDED
#define SCENERY_GROUP_H_INCLUDED
#include <stdint.h>

typedef struct
{
object_header_t* members;
uint32_t num_members;
}scenery_group_member_info_t;

typedef struct{
uint8_t num_members;
uint8_t unknown_1;
uint8_t priority;
uint8_t unknown_2;
uint32_t entertainers;
string_table_t name;
scenery_group_member_info_t member_info;
image_list_t sprites;
}scenery_group_t;


error_t scenery_group_decode(scenery_group_t* scenery_group,chunk_t* chunk);
error_t scenery_group_encode(scenery_group_t* scenery_group,uint8_t encoding,chunk_t* chunk);
void scenery_group_destroy(scenery_group_t* scenery_group);


#endif // SCENERY_GROUP_H_INCLUDED
