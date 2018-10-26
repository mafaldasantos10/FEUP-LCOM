  #include <lcom/lcf.h>
  #include <lcom/timer.h>

  #include <stdint.h>

  #include "i8254.h"

int timer_counter = 0;
static int hook_id_timer = 0 ;
int res;

int (timer_set_frequency)(uint8_t (timer), uint32_t freq) 
{

  uint8_t st, lsb, msb;

  if (freq < 19 || freq > TIMER_FREQ)
  {
    return 1;
  }

  if (timer_get_conf(timer, &st) != OK)
  {
    return 1;
  }

  uint8_t control = TIMER_LSB_MSB | (st & 0x0F);

  switch(timer)
  {
    case 0:
      control |= TIMER_SEL0;
      break;
    case 1:
      control |= TIMER_SEL1;
      break;
    case 2:
      control |= TIMER_SEL2;
      break;
    default:
      return 1;
  }

  //checks if the sys call was valid
  if (sys_outb(TIMER_CTRL, control) != OK)
  {
    return 1;
  }

  uint16_t f_freq = TIMER_FREQ/freq;

  util_get_LSB(f_freq, &lsb);

  util_get_MSB(f_freq, &msb);

  //checks if the sys call was valid
  if (sys_outb(TIMER_0 + timer, lsb) != OK)
  {
    return 1;
  }

  //checks if the sys call was valid
  if (sys_outb(TIMER_0 + timer, msb) != OK)
  {
    return 1;
  }

  return 0;
}


int (timer_subscribe_int)(uint8_t *bit_no) 
{
 *bit_no = hook_id_timer;

  //printf("dd %d\n", *bit_no );

  //checks if the sys call was valid
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) != OK)
  {
    return 1;
  }

  return 0;
}


int (timer_unsubscribe_int)() {

  //checks if the sys call was valid
  if (sys_irqrmpolicy(&hook_id_timer) != OK)
  {
    return 1;
  }

  return 0;
}


void (timer_int_handler)() 
{  
  timer_counter++;
}


int (timer_get_conf)(uint8_t (timer), uint8_t *(st)) 
{
  uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;
  uint32_t status;

  //checks if the sys call was valid
  if (sys_outb(TIMER_CTRL, rb_command) != OK)
  {
    return 1;
  }

  //selects the correct timer
  switch(timer)
  {
    case 0:
      //checks if the sys call was valid
      if (sys_inb(TIMER_0, &status) != OK)
      {
        return 1;
      }
      break;
    case 1:
      //checks if the sys call was valid
      if (sys_inb(TIMER_1, &status) != OK)
      {
        return 1;
      }
      break;
    case 2:
      //checks if the sys call was valid
      if (sys_inb(TIMER_2, &status) != OK)
      {
        return 1;
      }
      break;
    default:
      return 1;
  }

  *st = (uint8_t) status;

  return 0;
}


int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{

  union timer_status_field_val conf;

  if (field == base)
  {
    // *******0
    conf.bcd = (st << 7) >> 7; 
  }

  if (field == initial)
  {
    // ****3,2,1*
    conf.count_mode = (st << 4) >> 5;
  }

  if (field == mode)
  {
    // **5,4****
    conf.in_mode = (st << 2) >> 6;
  }

  if (field == all)
  {
    // 7,6,5,4,3,2,1,0
    conf.byte = st;
  }

  else
  {
    return 1;
  }

  if (timer_print_config(timer, field, conf) != OK)
  {
    return 1;
  }

  return 0;
}
