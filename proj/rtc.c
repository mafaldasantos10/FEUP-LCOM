#include <lcom/lcf.h>
#include <stdint.h>

#include "rtc.h"

int wait_valid_rtc(uint32_t reg) 
{
  uint32_t regA = 0, value;

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
  hour = wait_valid_rtc(HOUR); 

  if(1)
  hour = (((hour & 0xF0)>>4)*10) + (hour & 0x0F);
  
  return hour;
}

int get_min()
{
  uint32_t min;
  min = wait_valid_rtc(MINUTE); 

  if((RTC_REG_B & BCD_MODE) == 0)
  min = (((min & 0xF0)>>4)*10) + (min & 0x0F);

  return min;
}

int get_sec()
{
  uint32_t sec;
  sec = wait_valid_rtc(SECOND);

  if((RTC_REG_B & BCD_MODE) == 0)
  sec = (((sec & 0xF0)>>4)*10) + (sec & 0x0F);

  return sec;
}
