#ifndef DAT_SMALL_SCENERY_H_INCLUDED
#define DAT_SMALL_SCENERY_H_INCLUDED
#include <stdint.h>

enum //Small scenery flags
{
SMALL_SCENERY_FULL_TILE=0x00000001,
SMALL_SCENERY_OFFSET=0x00000002,
SMALL_SCENERY_FLAT=0x00000004,
SMALL_SCENERY_CAN_ROTATE=0x00000008,
SMALL_SCENERY_ANIMATED=0x00000010,
SMALL_SCENERY_GARDEN=0x00000020,
SMALL_SCENERY_CAN_WATER=0x00000040,
SMALL_SCENERY_DRAW2DIALOG=0x00000080,
SMALL_SCENERY_DIAGONAL=0x00000100,
SMALL_SCENERY_GLASS=0x00000200,
SMALL_SCENERY_REMAP_1=0x00000400,//
SMALL_SCENERY_FOUNTAIN1=0x00000800,
SMALL_SCENERY_FOUNTAIN4=0x00001000,
SMALL_SCENERY_CLOCK=0x00002000,
SMALL_SCENERY_SWAMPGOO=0x00004000,
SMALL_SCENERY_ANIMDATA=0x00008000,
SMALL_SCENERY_DRAW2MAIN=0x00010000,
SMALL_SCENERY_STACKABLE=0x00020000,//
SMALL_SCENERY_NO_WALLS=0x00040000,
SMALL_SCENERY_REMAP_2=0x00080000,
SMALL_SCENERY_NO_SUPPORTS=0x00100000,//
SMALL_SCENERY_DIALOG_FRAMES=0x00200000,
SMALL_SCENERY_SMALL_COG=0x00400000,
SMALL_SCENERY_HALF_TILE=0x01000000,
SMALL_SCENERY_THREE_QUARTER_TILE=0x02000000,
SMALL_SCENERY_REMAP_SUPPORTS=0x04000000,
SMALL_SCENERY_POLE=0x08000000,
}small_scenery_flags_t;


//Small scenery optional data
typedef struct
{
short num_indices;
uint8_t* indices;
}animation_indices_t;

typedef struct //Small scenery
{
uint32_t flags;
uint16_t anim_delay;
uint16_t anim_parameters;
uint16_t anim_frames;
uint8_t height;
uint8_t cursor_sel;
int16_t build_fee;
int16_t remove_fee;
string_table_t name;
animation_indices_t animation_indices;
object_header_t group_info;
image_list_t sprites;
}small_scenery_t;


error_t small_scenery_decode(small_scenery_t* small_scenery,chunk_t* chunk);
error_t small_scenery_encode(small_scenery_t* small_scenery,uint8_t encoding,chunk_t* chunk);
void small_scenery_destroy(small_scenery_t* scenery);

#endif // DAT_SMALL_SCENERY_H_INCLUDED
