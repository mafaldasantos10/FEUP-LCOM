#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t UNUSED(timer), uint32_t UNUSED(freq)) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *UNUSED(bit_no)) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);
}
 //if
int (timer_get_conf)(uint8_t (timer), uint8_t *(st)) {
 
 uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_SEL(timer);

 sys_outb(TIMER_CTRL, rb_command);

 //checks if the sys call was valid
 int res = sys_outb(TIMER_CTRL, rb_command);

 if (res != OK)
 {
  printf ("Erro!");
  return 1;
 }

 sys_inb(TIMER_0 + timer, st);

 //checks if the sys call was valid
 int res2 = sys_inb(TIMER_0 + timer, st);

  if (res2 != OK)
 {
  printf ("Erro!");
  return 1;
 }

  return 0;
}

int (timer_display_conf)(uint8_t (timer), uint8_t (st),
                        enum timer_status_field (field)) {

conf.bcd = 
timer_print_config(timer, field, conf)


  return 1;
}