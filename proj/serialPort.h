#pragma once

/** @defgroup SerialPort SerialPort
 * @{
 *
 * Functions to handle the SerialPort
 */

/** @brief irq */
#define COM1_IRQ 4

/** @brief Enables the Data Available Interrupt */
#define RD_ENABLE BIT(0)

/** @brief Enables the Receiver Line Status */
#define RLS_ENABLE BIT(2)
/** @brief for tick delay */
#define SP_DELAY    10000 

/** @brief Address of Receiver Buffer Register */
#define RBR               0x3F8
/** @brief Address of Transmitter Holding Register */
#define THR               0x3F8
/** @brief Address of Interrupt Enable Register */
#define IER               0x3F9
/** @brief Address of Interrupt Identification Register */
#define IIR               0x3FA
/** @brief Address of Line Status Register */
#define LSR               0x3FD

/** @brief It is set to one if there's data to be received */
#define RECEIVER_DATA    BIT(0) 

/** @brief Set to 1 if Uart can receive a character */
#define THR_EMPTY          BIT(5) 

/** @brief Receiver Line Status */
#define RLS             (BIT(1)|BIT(2))

/** @brief Recived Data Available */
#define RD               BIT(2) 


/**
 * @brief Gets the char that contains the information passed by the serial port of the x cromossome
 * 
 * @return Char container
 */
uint32_t getContainerx();

/**
 * @brief Gets the char that contains the information passed by the serial port of the y cromossome
 * 
 * @return Char container
 */
uint32_t getContainery();

/**
 * @brief Subscribes the uart interrupts
 * 
 * @param bit_no_uart Uart identifier
 * @return Negative if failure
 */
int serialPort_subscribe_int(uint8_t *bit_no_uart);

/**
 * @brief Unsubscribes the uart interrups
 * 
 * @return Negative if failure
 */
int serialPort_unsubscribe_int(); 

/**
 * @brief Reads the Reciever Buffer Register
 * 
 * @param byte Uint32_t that will have the read information
 * @return Negative if failure
 */
int get_RBR(uint32_t *byte);

/**
 * @brief Writes to Transmitter Holding Register
 * 
 * @param byte Uint32_t that is written
 * @return Negative if failure
 */
int write_to_THR(uint32_t byte);

/**
 * @brief Clears the RBF
 * @return Negative if failure
 */
int clear_RBR();

/**
 * @brief Uart handler
 * 
 * @param byte Uint32_t that will contain the information
 * @return Negative if failure
 */
int serialPort_handler(uint32_t *byte);

/**
 * @brief Waits for the x cromossome to join multiplayer
 * 
 * @param bit_no_timer Timer identifier
 * @param bit_no_kb Keyboard identifier
 * @param bit_no_mouse Mouse identifier
 * @param bit_no_uart Uart identifier
 * @return Negative if failure
 */
int playerY_sync(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

/**
 * @brief Waits for the y cromossome to join multiplayer
 * 
 * @param bit_no_timer Timer identifier
 * @param bit_no_kb Keyboard identifier
 * @param bit_no_mouse Mouse identifier
 * @param bit_no_uart Uart identifier
 * @return Negative if failure
 */
int playerX_sync(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

/**
 * @brief Multiplayer game for x cromossome
 * 
 * @param bit_no_timer Timer identifier
 * @param bit_no_kb Keyboard identifier
 * @param bit_no_mouse Mouse identifier
 * @param bit_no_uart Uart identifier
 * @return negative if failure
 */
int gameMultiX(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

/**
 * @brief Multiplayer game for y cromossome
 * 
 * @param bit_no_timer Timer identifier
 * @param bit_no_kb Keyboard identifier
 * @param bit_no_mouse Mouse identifier
 * @param bit_no_uart Uart identifier
 * @return negative if failure
 */
int gameMultiY(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

/**@}*/
