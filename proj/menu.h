#pragma once

typedef enum {START, MENU, CHARACTER, GAME} st_t;
typedef enum {SINGLE, MULTI, HIGHSCORES, INSTRUCTIONS, EXIT} state_t;
typedef enum {C_Y, C_X} state2_t;


//PROTOTYPES
//////////////////////////////////////////////////////////////////

int menu();

void change_menu_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void change_buttons();

void default_menu();

void append(char* s, char c);

void convert_key();

void default_state();
