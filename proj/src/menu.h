#pragma once

/** @defgroup Menu Menu
 * @{
 *
 * Functions to manipulate the Menu of the game
 */

/** @brief State of the Game */
typedef enum {START, MENU, CHARACTER, GAME} st_t;
/** @brief State of the Menu */
typedef enum {SINGLE, MULTI, HIGHSCORES, INSTRUCTIONS, EXIT} state_t;
/** @brief State of the Character Menu */
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

/**
 * @brief Draws the default menu, with the curret button being the Single Player
 */
void default_menu();

/**
 * @brief Appends a char to a string
 * 
 * @param s String to whom the char will be appended
 * @param c Char to be appended to the string
 */
void append(char* s, char c);

/**
 * @brief Converts the keyboard scanned code to the repective ASCII character
 */
void convert_key();

/**
 * @brief State machine goes to the default state. State goes to MENU, SinglePlayer is selected and Y Chromosome is selected
 */
void default_state();

/**@}*/
