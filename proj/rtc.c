#include <lcom/lcf.h>
#include <stdint.h>

#include "rtc.h"

int read_rtc(uint32_t reg) 
{
  uint32_t regA = 0, value = 0;

  do {

    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    sys_inb(RTC_DATA_REG, &regA);

  } while ( regA & RTC_UIP);


  if(sys_outb(RTC_ADDR_REG, reg) != OK)
  {
    return 1;
  }

  if(sys_inb(RTC_DATA_REG, &value) != OK)
  {
    return 1;
  }

  return (int)value;
}

int get_hour()
{
  uint32_t hour;
  hour = read_rtc(HOUR); 

  if((RTC_REG_B & BCD_MODE) == 0)
  hour = (((hour & 0xF0)>>4)*10) + (hour & 0x0F);
  
  return hour;
}

int get_min()
{
  uint32_t min;
  min = read_rtc(MINUTE); 

  if((RTC_REG_B & BCD_MODE) == 0)
  min = (((min & 0xF0)>>4)*10) + (min & 0x0F);

  return min;
}

int get_sec()
{
  uint32_t sec;
  sec = read_rtc(SECOND);

  if((RTC_REG_B & BCD_MODE) == 0)
  sec = (((sec & 0xF0)>>4)*10) + (sec & 0x0F);

  return sec;
}

int get_day()
{
  uint32_t day;
  day = read_rtc(DAY);

  if((RTC_REG_B & BCD_MODE) == 0)
  day = (((day & 0xF0)>>4)*10) + (day & 0x0F);

  return day;
}

int get_month()
{
  uint32_t month;
  month = read_rtc(MONTH);

  if((RTC_REG_B & BCD_MODE) == 0)
  month = (((month & 0xF0)>>4)*10) + (month & 0x0F);

  return month;
}

int get_year()
{
  uint32_t year;
  year = read_rtc(YEAR);

  if((RTC_REG_B & BCD_MODE) == 0)
  year = (((year & 0xF0)>>4)*10) + (year & 0x0F);

  return year;
}
