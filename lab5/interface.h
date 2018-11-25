#pragma once

extern uint16_t x;
extern uint16_t y;
extern int new_counter;
extern bool keep;

void pix_map(const char *xpm[], uint16_t x, uint16_t y);

void draw_rectangle(uint16_t x, uint16_t y, uint16_t len, uint32_t color,uint16_t height);

void draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step, uint32_t mode);

int pix_map_move_pos(const char *xpm[], uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

int pix_map_move_neg(const char *xpm[], uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

int color_mode(unsigned int i, unsigned int j, uint8_t no_rectangles, uint32_t first, uint8_t step, uint32_t mode);

int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p);

int get_controller_info(vg_vbe_contr_info_t * info_p);
