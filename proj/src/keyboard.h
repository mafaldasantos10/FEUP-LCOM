#pragma once

/** @defgroup Keyboard Keyboard
 * @{
 *
 * Functions to handle the Keyboard
 */

//GLOBAL VARIABLES
extern uint32_t status; //global variable that contains the scan of the buffer
extern bool error_kbd; // global variable that is true if there is an error


//PROTOTYPES

/**
 * @brief Subscribes the keyboard interrupts
 * @param bit_no Keyboard identifier
 * @return Negative if failure
 */
int kbd_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes keyboard interrupts
 * @return Negative if failure
 */
int kbd_unsubscribe_int();

/**@}*/
