#pragma once

/** @defgroup RTC RTC
 * @{
 *
 * Functions to handle the RTC
 */

/** @brief RTC's address register */
#define RTC_ADDR_REG 0x70

/** @brief RTC's data register */
#define RTC_DATA_REG 0x71

/** @brief Minute register address  */
#define MINUTE 0x02

/** @brief Hour register address */
#define HOUR 0x04

/** @brief Second register address */
#define SECOND 0x00

/** @brief Day register address */
#define DAY 0x07

/** @brief Month register address */
#define MONTH 0x08

/** @brief Year register address */
#define YEAR 0x09

/** @brief RTC's A register address */
#define RTC_REG_A 0x0A

/** @brief RTC's B register address */
#define RTC_REG_B 0x0B

/** @brief Set to 1 if update in progress. */
#define RTC_UIP BIT(7)

/** @brief Checks if it is in BCD */
#define BCD_MODE BIT(2)

/////////////// Prototypes ///////////////////////

/**
 * @brief Waits for a valid RTC and reads the register of the RTC
 * @param reg Register
 * @return Negative if failure
 */
int read_rtc(uint32_t reg);

/**
 * @brief Gets the hour from RTC
 * @return Negative if failure
 */
int get_hour();

/**
 * @brief Gets the minutes from RTC
 * @return Negative if failure
 */
int get_min();

/**
 * @brief Gets the seconds from RTC
 * @return Negative if failure
 */
int get_sec();

/**
 * @brief Gets the day from RTC
 * @return Negative if failure
 */
int get_day();

/**
 * @brief Gets the month from RTC
 * @return Negative if failure
 */
int get_month();

/**
 * @brief Gets the year from RTC
 * @return Negative if failure
 */
int get_year();

/**
 * @brief Prints the argument number on the screen
 * @param number Number to be printed
 * @param x Coordinate x of the screen where it will be printed
 * @param y Coordinate y of the screen where it will be printed
 */
void print_digit_rtc(int number, int x, int y);

/**
 * @brief Prints the date in the format dd/mm/yy
 * @param day Day to be printed
 * @param month Month to be printed
 * @param year Year to be printed
 * @param x Coordinate x of the screen where it will be printed
 * @param y Coordinate y of the screen where it will be printed
 */
void print_date(int day, int month, int year, int x, int y);

/**
 * @brief Prints the Time in the format hh:mm:ss
 * @param x Coordinate x of the screen where it will be printed
 * @param y Coordinate y of the screen where it will be printed
 */
void print_time(int x, int y);

/**@}*/
