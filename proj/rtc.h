#pragma once

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define MINUTE 0x02
#define HOUR 0x04
#define SECOND 0x00
#define DAY 0x07
#define MONTH 0x08
#define YEAR 0x09
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_UIP BIT(7)
#define BCD_MODE BIT(2)

int read_rtc(uint32_t reg);

int get_hour();

int get_min();

int get_sec();

int get_day();

int get_month();

int get_year();

void print_digit_rtc(int number, int x, int y);

void print_date(int x, int y);

void print_time(int x, int y);
