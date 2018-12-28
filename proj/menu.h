#pragma once

typedef enum {START, HIGHSCORES, INSTRUCTIONS, EXIT} state_t;
extern bool do_not_change;

int menu();

void change_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void change_buttons();

void default_menu();
