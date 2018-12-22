#pragma once

typedef enum {START, HIGHSCORES, INSTRUCTIONS, EXIT} state_t;

int menu();

void change_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void change_buttons(Bitmap * start_selected,Bitmap * start_not_selected, Bitmap * highscores_not_selected, Bitmap * highscores_selected, Bitmap * instructions_not_selected, Bitmap * instructions_selected, Bitmap * exit_not_selected, Bitmap * exit_selected, Bitmap * menu);
