#ifndef PATH_OBJECT_H_INCLUDED
#define PATH_OBJECT_H_INCLUDED
#include <stdint.h>

enum 
{
PATH_OBJECT_BIN=0x0001,
PATH_OBJECT_BENCH=0x0002,
PATH_OBJECT_CAN_VANDAL=0x0004,
PATH_OBJECT_LIGHT=0x0008,
PATH_OBJECT_JUMPFOUNTAIN=0x0010,
PATH_OBJECT_JUMPSNOWBALL=0x0020,
PATH_OBJECT_UNKNOWN1=0x0040,
PATH_OBJECT_UNKNOWN2=0x0080,
PATH_OBJECT_QUEUE_LINE_TV=0x0100
}path_object_flags_t;

typedef struct 
{
uint16_t flags;
uint8_t subtype;
uint8_t cursor_sel;
int16_t build_fee;
string_table_t name;
object_header_t object_header;
image_list_t sprites;
}path_object_t;


error_t path_object_decode(path_object_t* path_object,chunk_t* chunk);
error_t path_object_encode(path_object_t* path_object,uint8_t encoding,chunk_t* chunk);
void path_object_destroy(path_object_t* path_object);


#endif // PATH_OBJECT_H_INCLUDED
