#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED
#include <stdint.h>

enum //Wall flags
{
WALL_REMAP_1=0x01,
WALL_GLASS=0x02,
WALL_FLAT=0x04,
WALL_TWOSIDED=0x08,
WALL_DOOR=0x10,
WALL_REMAP_2=0x40,
WALL_REMAP_3=0x80
}wall_flags_t;

typedef struct //Wall
{
uint8_t cursor_sel;
uint8_t flags;
uint8_t clearance;
uint8_t effects;
uint16_t build_fee;
uint8_t scrolling;
string_table_t name;
object_header_t object_header;
image_list_t sprites;
}wall_t;


error_t wall_decode(wall_t* wall,chunk_t* chunk);
error_t wall_encode(wall_t* wall,uint8_t encoding,chunk_t* chunk);
void wall_destroy(wall_t* wall);


#endif // WALL_H_INCLUDED
