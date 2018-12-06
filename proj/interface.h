#pragma once

//GLOBAL VARIABLES
extern uint16_t y;
extern int res_y, res_x;
extern unsigned char* double_buffer;

//PROTOTYPES
//////////////////////////////////////////////////////////////////

int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p);

void double_buffer_to_video_mem();
