#ifndef PATH_BANNER_H_INCLUDED
#define PATH_BANNER_H_INCLUDED
#include <stdint.h>

enum
{
PATH_BANNER_REMAP_1=0x01,
}path_banner_flags_t;

typedef struct
{
uint8_t scroll;
uint8_t flags;
int16_t build_fee;
string_table_t name;
object_header_t object_header;
image_list_t sprites;
}path_banner_t;


error_t path_banner_decode(path_banner_t* path_banner,chunk_t* chunk);
error_t path_banner_encode(path_banner_t* path_banner,uint8_t encoding,chunk_t* chunk);
void path_banner_destroy(path_banner_t* path_banner);


#endif // PATH_BANNER_H_INCLUDED
