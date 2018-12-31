#include <lcom/lcf.h>
#include <stdint.h>

#include "rtc.h"
#include "i8042.h"
#include "score.h"
#include "interface.h"


int read_rtc(uint32_t reg) 
{
  uint32_t regA = 0, value = 0;

  do {

    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    sys_inb(RTC_DATA_REG, &regA);

  } while ( regA & RTC_UIP);


  if (sys_outb(RTC_ADDR_REG, reg) != OK)
  {
    return 1;
  }

  if (sys_inb(RTC_DATA_REG, &value) != OK)
  {
    return 1;
  }

  return (int)value;
}

int get_hour()
{
  uint32_t hour;
  hour = read_rtc(HOUR); 

  if ((RTC_REG_B & BCD_MODE) == 0)
  hour = (((hour & 0xF0) >> 4) * 10) + (hour & 0x0F);
  
  return hour;
}

int get_min()
{
  uint32_t min;
  min = read_rtc(MINUTE); 

  if ((RTC_REG_B & BCD_MODE) == 0)
  min = (((min & 0xF0) >> 4) * 10) + (min & 0x0F);

  return min;
}

int get_sec()
{
  uint32_t sec;
  sec = read_rtc(SECOND);

  if ((RTC_REG_B & BCD_MODE) == 0)
  sec = (((sec & 0xF0) >> 4) * 10) + (sec & 0x0F);

  return sec;
}

int get_day()
{
  uint32_t day;
  day = read_rtc(DAY);

  if ((RTC_REG_B & BCD_MODE) == 0)
  day = (((day & 0xF0) >> 4) * 10) + (day & 0x0F);

  return day;
}

int get_month()
{
  uint32_t month;
  month = read_rtc(MONTH);

  if ((RTC_REG_B & BCD_MODE) == 0)
  month = (((month & 0xF0)>>4) * 10) + (month & 0x0F);

  return month;
}

int get_year()
{
  uint32_t year;
  year = read_rtc(YEAR);

  if ((RTC_REG_B & BCD_MODE) == 0)
  year = (((year & 0xF0) >> 4) * 10) + (year & 0x0F);

  return year;
}

int pos_x;

void print_digit_rtc(int number, int x, int y)
{
  int digits;

  digits = number_of_digits(number);

  if (digits == 1)
  {
    print_small_digit(number, x, y);
    pos_x -= 25;
    print_small_digit(0, pos_x, y);
    pos_x -= 18;
  }
  else
  {
    print_small_digit(number % 10, x, y);
    number /= 10;
    pos_x -= 23;
    print_small_digit(number, pos_x, y);
    pos_x -= 15;
  }
}

void print_date(int day, int month, int year, int x, int y)
{
  pos_x = x;

  print_digit_rtc(year, pos_x, y);
  drawBitmap(images.slash, pos_x, y, ALIGN_LEFT);
  pos_x -= 22;
  print_digit_rtc(month, pos_x, y);
  drawBitmap(images.slash, pos_x, y, ALIGN_LEFT);
  pos_x -= 22;
  print_digit_rtc(day, pos_x, y);
}

void print_time(int x, int y)
{
  pos_x = x;

  print_digit_rtc(get_sec(), pos_x, y);
  drawBitmap(images.colon, pos_x, y, ALIGN_LEFT);
  pos_x -= 25;
  print_digit_rtc(get_min(), pos_x, y);
  drawBitmap(images.colon, pos_x, y, ALIGN_LEFT);
  pos_x -= 25;
  print_digit_rtc(get_hour(), pos_x, y);
}
