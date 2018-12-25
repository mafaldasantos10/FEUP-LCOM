#pragma once

//GLOBAL VARIABLES
extern unsigned char* double_buffer;

//PROTOTYPES
//////////////////////////////////////////////////////////////////

int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p);

void double_buffer_to_video_mem();

int get_horizontal_resolution();

int get_vertical_resolution();
