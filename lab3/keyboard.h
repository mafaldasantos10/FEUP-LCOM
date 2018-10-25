#pragma once

/** @defgroup ..........
 * @{
 *
 * Functions for using the ........................
 */
int kbc_scan_ih();
/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
uint8_t kbd_int_handler();

int kbd_scan_poll();

int kbd_poll_cmd();

void kbd_poll();
