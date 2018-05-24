#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <image.h>

#define NUM_STRING_TABLES 3
#define NUM_LANGUAGES 16

typedef enum //Languages
{
LANGUAGE_ENGLISH_UK=0,
LANGUAGE_ENGLISH_US=1
}language_t;

typedef enum
{
OBJECT_ORIGIN_DEFAULT,
OBJECT_ORIGIN_EXPANSION,
OBJECT_ORIGIN_CUSTOM
}object_origin_t;

typedef enum
{
OBJECT_RIDE=0,
OBJECT_SMALL_SCENERY=1,
OBJECT_LARGE_SCENERY=2,
OBJECT_WALL=3,
OBJECT_PATH_BANNER=4,
OBJECT_PATH=5,
OBJECT_PATH_OBJECT=6,
OBJECT_SCENERY_GROUP=7,
OBJECT_PARK_ENTRANCE=8,
OBJECT_WATER=9,
OBJECT_SCENARIO_TEXT=10
}object_type_t;

typedef enum
{
ERROR_NONE,
ERROR_FILE_OPERATION_FAILED,
ERROR_CHECKSUM_MISMATCH,
ERROR_INVALID_ENCODING,
ERROR_INVALID_LANGUAGE,
ERROR_PREMATURE_EOF,
ERROR_PREMATURE_END_OF_CHUNK,
ERROR_SCANLINE_OUT_OF_BOUNDS,
ERROR_INVALID_CHUNK,
ERROR_INVALID_OBJECT,
}error_t;

typedef struct //image list
{
image_t* images;
uint32_t num_images;
}image_list_t;

typedef struct //String table
{
uint8_t* strings[NUM_LANGUAGES];
}string_table_t;

typedef struct
{
uint32_t flags;
uint8_t name[8];
}group_info_t;

#include "chunk.h"


typedef struct
{
uint32_t flags;
uint8_t name[8];
uint32_t checksum;
chunk_t chunk;
}object_t;




void* malloc_or_die(size_t size);

error_t string_table_decode(string_table_t* string_table,uint8_t* data,uint32_t data_length,uint32_t* length);
uint32_t string_table_get_encoded_length(string_table_t* table);
void string_table_encode(string_table_t* table,uint8_t* data);
void string_table_destroy(string_table_t* table);

error_t group_info_decode(group_info_t* group_info,uint8_t* data,uint32_t data_length);
void group_info_encode(group_info_t* table,uint8_t* data);

error_t image_list_decode(image_list_t* image_list,uint8_t* data,uint32_t data_length);
uint32_t image_list_get_encoded_length(image_list_t* list);
void image_list_encode(image_list_t* list,uint8_t* data);
void image_list_destroy(image_list_t* list);

error_t object_read(object_t* object,FILE* file);
error_t object_write(object_t* object,FILE* file);

//char* object_get_string(object_t* object,string_table_index_t table_num,language_t language);
//void object_set_string(object_t* object,string_table_index_t table_num,language_t language,char* new_string);
/*void RenderSprites(object_t* file,Animation* animations[5]);*/
void object_destroy(object_t* object);

char* error_string(error_t error);

#include "ride.h"
#include "small_scenery.h"
#include "large_scenery.h"
#include "wall.h"
#include "path_banner.h"
#include "path.h"
#include "path_object.h"
#include "scenery_group.h"
#include "park_entrance.h"

#endif /*BACKEND_H_INCLUDED*/
