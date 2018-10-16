#pragma once

/** @defgroup lab2 lab2
 * @{
 *
 * Test and utility functions for the Timer's lab
 */

/**
 * @brief Tests display of timer config
 *
 * Just calls timer_get_conf() followed by timer_display_conf()
 *
 * @param timer Timer whose config to read (Ranges from 0 to 2)
 * @param field  Configuration field to display
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_scan)(bool UNUSED (assembly));
                           
/**
 * @brief Tests change of Timer O interrupt frequency
 *
 * Programs Timer 0 to generate interrupts with input frequency
 *
 * @param timer Timer whose time-base should be changed (Ranges from 0 to 2)
 * @param freq Frequency of interrupts to generate
 * @return Return 0 upon success and non-zero otherwise
 */                         
int (kbd_test_poll)();

/**
 * @brief Tests Timer 0 interrupt handling
 *
 * Subscribes Timer 0 interrupts and prints a message once
 *  per second for the specified time interval
 *
 * @param time Length of time interval while interrupts are subscribed
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbd_test_timed_scan)(uint8_t UNUSED(n));
