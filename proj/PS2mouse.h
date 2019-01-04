#pragma once

//GLOBAL VARIABLES
extern uint32_t status_mouse; //global variable that with the contents of the buffer
extern struct packet pp;
extern bool error_mouse;


//PROTOTYPES

/**
 * @brief subscribes mouse interrupts
 *
 * @param interrupt mask
 * @return negative if failure
 */
int mouse_subscribe_int(uint8_t *bit_no);

/**
 * @brief unsubscribes mouse interrupts
 *
 * @return negative if failure
 */
int mouse_unsubscribe_int();

/**
 * @brief mouse interrupt handler
 */
void (mouse_ih)(void);

/**
 * @brief creates the packet with the right values
 */
void packet_create();

/**
 * @brief writes in the out buff any register
 *
 * @param register to write
 * @return negative if failure
 */
int set_mouse(uint8_t write);

/**
 * @brief enables the mouse and stream mode
 *
 * @return negative if failure
 */
int mouse_write_int();

/**
 * @brief disables mouse
 *
 * @return negative if failure
 */
int disable_int();

/**
 * @brief gets the mouse position and checks if it's in the frame
 */
void currentMousePosition();

/**
 * @brief gets mouse horizontal coordinate
 *
 * @return horizontal mouse coordinate
 */
int get_mouseX();

/**
 * @brief gets mouse vertical coordinate
 *
 * @return vertical mouse coordinate
 */
int get_mouseY();
