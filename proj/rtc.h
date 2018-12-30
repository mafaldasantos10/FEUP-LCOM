#pragma once

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define MINUTE 0x02
#define HOUR 0x04
#define SECOND 0x00
#define BIT(n) (0x01<<(n))
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_UIP BIT(7)
#define BCD_MODE BIT(2)

int wait_valid_rtc(uint32_t reg);

int get_hour();

int get_min();

int get_sec();
