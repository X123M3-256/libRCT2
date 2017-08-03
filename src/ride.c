#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "object.h"

/*
void ride_header_write(ride_header_t* header,buffer_t* buffer)
{
int i;
uint8_t header_bytes[0x1C2];
memset(header_bytes,0,0x1C2);

/*Write flags
*((uint32_t*)(header_bytes+8))=header->flags;
/*Write track style & preview index
memset(header_bytes+12,0xFFu,3);
header_bytes[12+header->preview_index]=header->track_style;
/*Write zero cars
header_bytes[18]=header->zero_cars;
/*Write car icon index
header_bytes[19]=header->car_icon_index;
/*Write minimum and maximum car length
header_bytes[15]=header->minimum_cars;
header_bytes[16]=header->maximum_cars;
/*Write track sections
*((uint64_t*)(header_bytes+438))=header->track_sections;

/*Write excitement, intensity, nausea
header_bytes[434]=header->excitement;
header_bytes[435]=header->intensity;
header_bytes[436]=header->nausea;
/*Write max height
header_bytes[437]=header->max_height;
/*Write categories
header_bytes[446]=header->categories[0];
header_bytes[447]=header->categories[1];
/*Write car type information
header_bytes[20]=header->car_types[CAR_INDEX_DEFAULT];
header_bytes[21]=header->car_types[CAR_INDEX_FRONT];
header_bytes[22]=header->car_types[CAR_INDEX_SECOND];
header_bytes[23]=header->car_types[CAR_INDEX_REAR];
header_bytes[24]=header->car_types[CAR_INDEX_THIRD];
/*Write values that are known, but not applicable
header_bytes[17]=0xFF;/*Flat ride "cars"
header_bytes[448]=0xFF;/*First shop item=
header_bytes[449]=0xFF;/*Second shop item

/*Write car types
uint8_t* car_data=header_bytes+26;
    for(i=0;i<NUM_CARS;i++)
    {
    car_t* car=header->cars+i;
    /*Write highest rotation index
    car_data[0]=car->highest_rotation_index;
    /*Write spacing
    *((uint32_t*)(car_data+4))=car->spacing;
    /*Write friction
    *((uint16_t*)(car_data+8))=car->friction;
    /*Write riders
    car_data[11]=car->rider_pairs|car->riders;
    /*Write sprite flags
    *((uint16_t*)(car_data+12))=car->sprites;
    /*Write rider sprites
    car_data[84]=car->rider_sprites;
    /*Write flags
    *((uint32_t*)(car_data+17))=car->flags;
    /*Write "extra frames"
    car_data[21]=car->extra_swing_frames;
    /*Write spin parameters
    car_data[85]=car->spin_inertia;
    car_data[86]=car->spin_friction;
    /*Write sound effects
    car_data[87]=car->running_sound;
    car_data[89]=car->secondary_sound;


    /*Write powered velocity
    car_data[91]=car->powered_acceleration;
    car_data[92]=car->powered_velocity;
    /*Write Z value
    car_data[95]=car->z_value;
    /*Write unknown fields
    car_data[94]=(uint8_t)car->unknown[0];
    *((uint16_t*)(car_data+96))=car->unknown[1];
    car_data[88]=(uint8_t)car->unknown[2];
    car_data[90]=(uint8_t)car->unknown[3];
    car_data[10]=(uint8_t)car->unknown[4];
    car_data[14]=(uint8_t)car->unknown[5];
    car_data[15]=(uint8_t)car->unknown[6];
    car_data[16]=(uint8_t)car->unknown[7];
    car_data[93]=(uint8_t)car->unknown[8];
    /*Move to next car
    car_data+=101;
    }
buffer_write(buffer,header_bytes,0x1C2);
}
void ride_header_free(ride_header_t* header)
{
free(header);
}

void ride_structures_write(ride_structures_t* structures,buffer_t* buffer)
{
int i;
/*Write number of 3 byte structures
buffer_write(buffer,&structures->num_default_colors,1);
/*Write 3 byte structures
    for(i=0;i<structures->num_default_colors;i++)
    {
    buffer_write(buffer,&structures->default_colors[i].colors[0],1);
    buffer_write(buffer,&structures->default_colors[i].colors[1],1);
    buffer_write(buffer,&structures->default_colors[i].colors[2],1);
    }
/*Write 4 byte peep positioning structures
    for(i=0;i<4;i++)
    {
    /*If the length will fit in a byte, write it
        if(structures->peep_positions[i].num<0xFFu)buffer_write(buffer,(uint8_t*)&structures->peep_positions[i].num,1);
    /*Otherwise write as a short
        else
        {
        uint8_t value=0xFFu;
        buffer_write(buffer,&value,1);
        buffer_write(buffer,(uint8_t*)&structures->peep_positions[i].num,2);
        }
    /*Write structure
    buffer_write(buffer,structures->peep_positions[i].positions,structures->peep_positions[i].num);
    }
}
void ride_structures_free(ride_structures_t* structures)
{
int i;
    for(i=0;i<4;i++)free(structures->peep_positions[i].positions);
free(structures);
}

*/

//}



error_t default_colors_decode(color_scheme_t** default_colors,uint16_t* num_default_colors,uint8_t* data,uint32_t data_length,uint32_t* length_out)
{
//TODO: Check bounds
*num_default_colors=(data[0]==0xFF?32:data[0]);
*default_colors=malloc(*num_default_colors*sizeof(color_scheme_t));
uint32_t pos=1;
	for(int i=0;i<*num_default_colors;i++)
    {
    (*default_colors)[i].colors[0]=data[pos++];
    (*default_colors)[i].colors[1]=data[pos++];
    (*default_colors)[i].colors[2]=data[pos++];
    }
*length_out=1+(*num_default_colors)*3;
return ERROR_NONE;
}
uint32_t default_colors_get_encoded_length(color_scheme_t* default_colors,uint16_t num_default_colors)
{
return 1+num_default_colors*3;
}
void default_colors_encode(color_scheme_t* default_colors,uint16_t num_default_colors,uint8_t* data)
{
data[0]=(num_default_colors==32?0xFF:num_default_colors);

uint32_t pos=1;
	for(int i=0;i<num_default_colors;i++)
    {
    data[pos++]=default_colors[i].colors[0];
    data[pos++]=default_colors[i].colors[1];
    data[pos++]=default_colors[i].colors[2];
    }
}


error_t peep_positions_decode(uint8_t** peep_positions,uint16_t* num_peep_positions,uint8_t* data,uint32_t data_length,uint32_t* length_out)
{
// TODO: Check bounds
uint16_t length=data[0];
uint32_t pos=1;
    //If the length can't fit in a byte, the length is instead stored in the two bytes following
    if(length==0xFF)
    {
    length=*((uint16_t*)(data+1));
    pos+=2;
    }
*num_peep_positions=length;
*peep_positions=malloc(length);
memcpy(*peep_positions,data+pos,length);
*length_out=pos+length;
return ERROR_NONE;
}
uint32_t peep_positions_get_encoded_length(uint8_t* peep_positions,uint32_t num_peep_positions)
{
return (num_peep_positions>255)?(num_peep_positions+3):(num_peep_positions+1);
}
void peep_positions_encode(uint8_t* peep_positions,uint32_t num_peep_positions,uint8_t* data)
{
uint32_t pos=0;
    if(num_peep_positions<255)
    {
    data[0]=(uint8_t)num_peep_positions;
    pos++;
    }
    else
    {
    data[0]=0xFF;
    *((uint16_t*)(data+1))=num_peep_positions;
    }
memcpy(data+pos,peep_positions,num_peep_positions);
}


error_t ride_decode(ride_t* ride,chunk_t* chunk)
{
//Load header chunk->data
    if(chunk->length<0x1C2)
    {
    return ERROR_PREMATURE_END_OF_CHUNK;
    }

//Load ride header
ride->flags=*((uint32_t*)(chunk->data+8));
ride->track_types[0]=chunk->data[12];
ride->track_types[1]=chunk->data[13];
ride->track_types[2]=chunk->data[14];
ride->min_cars_per_train=chunk->data[15];
ride->max_cars_per_train=chunk->data[16];
ride->flat_ride_cars=chunk->data[17];
ride->zero_cars=chunk->data[18];
ride->tab_vehicle=chunk->data[19];
ride->default_vehicle=chunk->data[20];
ride->front_vehicle=chunk->data[21];
ride->second_vehicle=chunk->data[22];
ride->rear_vehicle=chunk->data[23];
ride->third_vehicle=chunk->data[24];
ride->excitement=*((int8_t*)(chunk->data+434));
ride->intensity=*((int8_t*)(chunk->data+435));
ride->nausea=*((int8_t*)(chunk->data+436));
ride->max_height=chunk->data[437];
ride->track_pieces=*((uint64_t*)(chunk->data+438));
ride->categories[0]=chunk->data[446];
ride->categories[1]=chunk->data[447];
ride->shop_item=chunk->data[448];
ride->shop_item_secondary=chunk->data[449];
//Load vehicle structures
uint8_t* vehicle_data=chunk->data+26;
    for(int i=0;i<4;i++)
    {
    /*Load highest rotation index*/
    ride->vehicles[i].rotation_frame_mask=*((uint16_t*)(vehicle_data));
    ride->vehicles[i].spacing=*((uint32_t*)(vehicle_data+4));
    ride->vehicles[i].friction=*((uint16_t*)(vehicle_data+8));
    ride->vehicles[i].tab_height=*((int8_t*)(vehicle_data+10));
    ride->vehicles[i].num_seats=vehicle_data[11];
    ride->vehicles[i].sprites=*((uint16_t*)(vehicle_data+12));
    ride->vehicles[i].sprite_width=vehicle_data[14];
    ride->vehicles[i].sprite_height_positive=vehicle_data[15];
    ride->vehicles[i].sprite_height_negative=vehicle_data[16];
    ride->vehicles[i].vehicle_animation=vehicle_data[17];
    ride->vehicles[i].flags=*((uint32_t*)(vehicle_data+18));
    ride->vehicles[i].num_rows=vehicle_data[84];
    ride->vehicles[i].spin_inertia=vehicle_data[85];
    ride->vehicles[i].spin_friction=vehicle_data[86];
    ride->vehicles[i].running_sound=vehicle_data[87];
    ride->vehicles[i].var_58=vehicle_data[88];
    ride->vehicles[i].secondary_sound=vehicle_data[89];
    ride->vehicles[i].var_5A=vehicle_data[90];
    ride->vehicles[i].powered_acceleration=vehicle_data[91];
    ride->vehicles[i].powered_max_speed=vehicle_data[92];
    ride->vehicles[i].car_visual=vehicle_data[93];
    ride->vehicles[i].effect_visual=vehicle_data[94];
    ride->vehicles[i].z_value=vehicle_data[95];
    ride->vehicles[i].special_frames=vehicle_data[96];
    vehicle_data+=101;
    }



uint32_t pos=0x1C2;
//Load string tables
uint32_t length;
error_t error=string_table_decode(&(ride->name),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)goto error_1;
pos+=length;
error=string_table_decode(&(ride->description),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)goto error_2;
pos+=length;
error=string_table_decode(&(ride->capacity),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)goto error_3;
pos+=length;
//Load default colors //TODO: Properly free resources if this fails
error=default_colors_decode(&(ride->default_colors),&(ride->num_default_colors),chunk->data+pos,chunk->length-pos,&length);
    if(error!=ERROR_NONE)goto error_4;
pos+=length;
//Load peep loading position structures TODO:Check bounds
    for (int i=0;i<4;i++)
    {
    error=peep_positions_decode(&(ride->vehicles[i].peep_positions),&(ride->vehicles[i].num_peep_positions),chunk->data+pos,chunk->length-pos,&length);
        if(error!=ERROR_NONE)
        {
            for(int j=0;j<i;j++)free(ride->vehicles[i].peep_positions);
        goto error_5;
        };
    pos+=length;
    }
//Load images
error=image_list_decode(&(ride->sprites),chunk->data+pos,chunk->length-pos);
    if(error!=ERROR_NONE)goto error_6;

return ERROR_NONE;
error_6:
    for(int i=0;i<4;i++)free(ride->vehicles[i].peep_positions);
error_5:
free(ride->default_colors);
error_4:
string_table_destroy(&(ride->capacity));
error_3:
string_table_destroy(&(ride->description));
error_2:
string_table_destroy(&(ride->name));
error_1:
return error;
}
error_t ride_encode(ride_t* ride,uint8_t encoding,chunk_t* chunk)
{
//Compute length of encoded chunk->data
uint32_t name_table_length=string_table_get_encoded_length(&(ride->name));
uint32_t description_table_length=string_table_get_encoded_length(&(ride->description));
uint32_t capacity_table_length=string_table_get_encoded_length(&(ride->capacity));
uint32_t default_colors_length=default_colors_get_encoded_length(ride->default_colors,ride->num_default_colors);
uint32_t peep_positions_lengths[4];
    for(uint32_t i=0;i<4;i++)peep_positions_lengths[i]=peep_positions_get_encoded_length(ride->vehicles[i].peep_positions,ride->vehicles[i].num_peep_positions);
uint32_t sprites_length=image_list_get_encoded_length(&(ride->sprites));
uint32_t length=0x1C2+name_table_length+description_table_length+capacity_table_length+default_colors_length+peep_positions_lengths[0]+peep_positions_lengths[1]+peep_positions_lengths[2]+peep_positions_lengths[3]+sprites_length;

//Allocate memory
chunk->encoding=encoding;
chunk->data=malloc_or_die(length);
chunk->length=length;
memset(chunk->data,0,length);

//Write ride header
*((uint64_t*)(chunk->data+438))=ride->track_pieces;
*((uint32_t*)(chunk->data+8))=ride->flags;
chunk->data[12]=ride->track_types[0];
chunk->data[13]=ride->track_types[1];
chunk->data[14]=ride->track_types[2];
chunk->data[15]=ride->min_cars_per_train;
chunk->data[16]=ride->max_cars_per_train;
chunk->data[17]=ride->flat_ride_cars;
chunk->data[18]=ride->zero_cars;
chunk->data[19]=ride->tab_vehicle;
chunk->data[20]=ride->default_vehicle;
chunk->data[21]=ride->front_vehicle;
chunk->data[22]=ride->second_vehicle;
chunk->data[23]=ride->rear_vehicle;
chunk->data[24]=ride->third_vehicle;
*((int8_t*)(chunk->data+434))=ride->excitement;
*((int8_t*)(chunk->data+435))=ride->intensity;
*((int8_t*)(chunk->data+436))=ride->nausea;
chunk->data[437]=ride->max_height;
chunk->data[446]=ride->categories[0];
chunk->data[447]=ride->categories[1];
chunk->data[448]=ride->shop_item;
chunk->data[449]=ride->shop_item_secondary;
//Load vehicle structures
uint8_t* vehicle_data=chunk->data+26;
    for(int i=0;i<4;i++)
    {
    /*Load highest rotation index*/
    *((uint16_t*)(vehicle_data))=ride->vehicles[i].rotation_frame_mask;
    *((uint32_t*)(vehicle_data+4))=ride->vehicles[i].spacing;
    *((uint16_t*)(vehicle_data+8))=ride->vehicles[i].friction;
    *((int8_t*)(vehicle_data+10))=ride->vehicles[i].tab_height;
    vehicle_data[11]=ride->vehicles[i].num_seats;
    *((uint16_t*)(vehicle_data+12))=ride->vehicles[i].sprites;
    vehicle_data[14]=ride->vehicles[i].sprite_width;
    vehicle_data[15]=ride->vehicles[i].sprite_height_positive;
    vehicle_data[16]=ride->vehicles[i].sprite_height_negative;
    vehicle_data[17]=ride->vehicles[i].vehicle_animation;
    *((uint32_t*)(vehicle_data+18))=ride->vehicles[i].flags;
    vehicle_data[84]=ride->vehicles[i].num_rows;
    vehicle_data[85]=ride->vehicles[i].spin_inertia;
    vehicle_data[86]=ride->vehicles[i].spin_friction;
    vehicle_data[87]=ride->vehicles[i].running_sound;
    vehicle_data[88]=ride->vehicles[i].var_58;
    vehicle_data[89]=ride->vehicles[i].secondary_sound;
    vehicle_data[90]=ride->vehicles[i].var_5A;
    vehicle_data[91]=ride->vehicles[i].powered_acceleration;
    vehicle_data[92]=ride->vehicles[i].powered_max_speed;
    vehicle_data[93]=ride->vehicles[i].car_visual;
    vehicle_data[94]=ride->vehicles[i].effect_visual;
    vehicle_data[95]=ride->vehicles[i].z_value;
    vehicle_data[96]=ride->vehicles[i].special_frames;
    vehicle_data+=101;
    }

uint32_t pos=0x1C2;
string_table_encode(&(ride->name),chunk->data+pos);
pos+=name_table_length;
string_table_encode(&(ride->description),chunk->data+pos);
pos+=description_table_length;
string_table_encode(&(ride->capacity),chunk->data+pos);
pos+=capacity_table_length;
default_colors_encode(ride->default_colors,ride->num_default_colors,chunk->data+pos);
pos+=default_colors_length;
    for(uint32_t i=0;i<4;i++)
    {
    peep_positions_encode(ride->vehicles[i].peep_positions,ride->vehicles[i].num_peep_positions,chunk->data+pos);
    pos+=peep_positions_lengths[i];
    }
image_list_encode(&(ride->sprites),chunk->data+pos);
return ERROR_NONE;
}
void ride_destroy(ride_t* ride)
{
string_table_destroy(&(ride->name));
string_table_destroy(&(ride->description));
string_table_destroy(&(ride->capacity));
free(ride->default_colors);
    for(int i=0;i<4;i++)free(ride->vehicles[i].peep_positions);
image_list_destroy(&(ride->sprites));
}

