#pragma once

//GLOBAL VARIABLES
extern uint32_t status_mouse; //global variable that with the contents of the buffer
extern struct packet pp;
extern bool error_mouse;


//PROTOTYPES

/**
 * @brief Subscribes mouse interrupts
 *
 * @param bit_no mouse identifier
 * @return Negative if failure
 */
int mouse_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts
 * @return Negative if failure
 */
int mouse_unsubscribe_int();

/**
 * @brief Mouse interrupt handler
 */
void (mouse_ih)(void);

/**
 * @brief Creates the packet with the right values
 */
void packet_create();

/**
 * @brief Writes in the out buff any register
 *
 * @param write Write register to write
 * @return Negative if failure
 */
int set_mouse(uint8_t write);

/**
 * @brief Enables the mouse and stream mode
 * 
 * @return Negative if failure
 */
int mouse_write_int();

/**
 * @brief Disables mouse
 *
 * @return Negative if failure
 */
int disable_int();

/**
 * @brief Gets the mouse position and checks if it's in the frame
 */
void currentMousePosition();

/**
 * @brief Gets mouse horizontal coordinate
 *
 * @return Horizontal mouse coordinate
 */
int get_mouseX();

/**
 * @brief Gets mouse vertical coordinate
 *
 * @return Vertical mouse coordinate
 */
int get_mouseY();
