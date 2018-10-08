#include <lcom/lcf.h>

#include <lcom/lab2.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t (timer), enum timer_status_field (field)) 
{

 uint8_t st;
 timer_get_conf(timer, &st);
 timer_display_conf(timer, conf, field)

  return 0;
}

int(timer_test_time_base)(uint8_t (timer), uint32_t (freq)) {

  timer_set_frequency(timer, freq);

  return 0;
}

int(timer_test_int)(uint8_t UNUSED (time)) {

  return 1;
}

int(util_get_LSB)(uint16_t (val), uint8_t *(lsb)) {

 uint8_t address = (uint8_t) *lsb;

 address = val & 0x00FF;
  return 0;
}

int(util_get_MSB)(uint16_t (val), uint8_t *(msb)) {
 
 uint8_t address = (uint8_t) *msb;

 address = val & 0xFF00;

  return 0;
}
