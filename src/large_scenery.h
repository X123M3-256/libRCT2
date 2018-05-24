#ifndef DAT_LARGE_SCENERY_H_INCLUDED
#define DAT_LARGE_SCENERY_H_INCLUDED
#include <stdint.h>

enum //Large scenery flags
{
LARGE_SCENERY_REMAP_1=0x01,
LARGE_SCENERY_REMAP_2=0x02,
LARGE_SCENERY_3D_TEXT=0x04,
LARGE_SCENERY_SCROLLING=0x08,
LARGE_SCENERY_PHOTOGENIC=0x10,
}large_scenery_flags_t;


//Large scenery optional data
typedef struct
{
int16_t x;
int16_t y;
int16_t base_height;
uint8_t clearance_height;
uint8_t flags;
uint8_t quadrants;
uint8_t walls;
}large_scenery_tile_t;

typedef struct
{
large_scenery_tile_t* tiles;
uint16_t num_tiles;
}large_scenery_tile_info_t;

typedef struct //Large scenery
{
uint8_t cursor_sel;
uint8_t flags;
int16_t build_fee;
int16_t remove_fee;
uint8_t scroll;
string_table_t name;
uint8_t* unknown;
large_scenery_tile_info_t tile_info;
object_header_t object_header;
image_list_t sprites;
}large_scenery_t;


error_t large_scenery_decode(large_scenery_t* large_scenery,chunk_t* chunk);
error_t large_scenery_encode(large_scenery_t* large_scenery,uint8_t encoding,chunk_t* chunk);
void large_scenery_destroy(large_scenery_t* scenery);

#endif // DAT_LARGE_SCENERY_H_INCLUDED
