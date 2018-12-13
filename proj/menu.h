#pragma once

typedef enum {START, INSTRUCTIONS, HIGHSCORES, EXIT} state_t;
//extern state_t state;

int menu();

void change_state(uint8_t bit_no_kb);

void change_buttons();
