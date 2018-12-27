#pragma once

typedef enum {START, HIGHSCORES, INSTRUCTIONS, EXIT} state_t;

int menu();

void change_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void change_buttons();
