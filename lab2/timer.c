  #include <lcom/lcf.h>
  #include <lcom/timer.h>

  #include <stdint.h>

  #include "i8254.h"

unsigned int counter = 0;

int (timer_set_frequency)(uint8_t (timer), uint32_t (freq)) 
{

  uint8_t st, lsb, msb;
  timer_get_conf(timer, &st);

  uint8_t control = TIMER_LSB_MSB | (st & 0x0F); //DATA ESTÁ PARADA, NÃO ATUALIZA!!!!

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

  
  int res = sys_outb(TIMER_CTRL, control);

  //checks if the sys call was valid
  if (res != OK)
  {
    printf ("Erro!");
    return 1;
  }

  uint16_t f_freq = TIMER_FREQ/freq;

  util_get_LSB(f_freq, &lsb);

  util_get_MSB(f_freq, &msb);


  int res1 = sys_outb(TIMER_0 + timer, lsb);

  //checks if the sys call was valid
  if (res1 != OK)
  {
    printf ("Erro!");
    return 1;
  }

  int res2 = sys_outb(TIMER_0 + timer, msb);

  //checks if the sys call was valid
  if (res2 != OK)
  {
    printf ("Erro!");
    return 1;
  }

  return 0;

}

int (timer_subscribe_int)(uint8_t * UNUSED (bit_no)) 
{
  return 1;
}

int (timer_unsubscribe_int)() {
    /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() 
{  
  counter++;
}


int (timer_get_conf)(uint8_t (timer), uint8_t *(st)) 
{
  uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_SEL(timer);

  sys_outb(TIMER_CTRL, rb_command);

  uint32_t status = (uint32_t) *st;

  
  int res = sys_outb(TIMER_CTRL, rb_command);

  //checks if the sys call was valid
  if (res != OK)
  {
    printf ("Erro!");
    return 1;
  }

  //selects the correct timer
  if (timer == 0)
  {
    int res2 = sys_inb(TIMER_0, &status);

    //checks if the sys call was valid
    if (res2 != OK)
    {
      printf ("Erro!");
      return 1;
    }
  }

  if (timer == 1)
  {
    int res3 = sys_inb(TIMER_1, &status);

    //checks if the sys call was valid
    if (res3 != OK)
    {
      printf ("Erro!");
      return 1;
    }
  }

  if (timer == 2)
  {
    int res4 = sys_inb(TIMER_2, &status);

    //checks if the sys call was valid
    if (res4 != OK)
    {
      printf ("Erro!");
      return 1;
    }
  }

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

timer_print_config(timer, field, conf);

return 0;

}
