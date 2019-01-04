#pragma once

typedef enum {START, MENU, CHARACTER, GAME} st_t;
typedef enum {SINGLE, MULTI, HIGHSCORES, INSTRUCTIONS, EXIT} state_t;
typedef enum {C_Y, C_X} state2_t;


//PROTOTYPES
//////////////////////////////////////////////////////////////////

/**
 * @brief Interrupt keyboard cycle of the menu
 * @return Negative if failure
 */
int menu();

/**
 * @brief Changes the state in the state machine
 * @param bit_no_timer Timer identifier
 * @param bit_no_kb Keyboard identifier
 * @param bit_no_mouse Mouse identifier
 * @param bit_no_uart Uart identifier
 */
void change_menu_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

/**
 * @brief Does the button animation of the menu, buttons become darker if pressed
 */
void change_buttons();

void default_menu();

void append(char* s, char c);

void convert_key();

void default_state();
