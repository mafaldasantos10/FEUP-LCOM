// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "interface.h"

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) 
{
  vg_init(mode);
  sleep(delay);
  vg_exit();
  
  return 0;
}

int (video_test_rectangle)(uint16_t UNUSED(mode), uint16_t UNUSED(x), uint16_t UNUSED(y),
                       uint16_t UNUSED(width), uint16_t UNUSED(height), uint32_t UNUSED(color)) {
  /* To be completed */
  //printf("%s(0x%03X, %u, %u, %u, %u, 0x%08x): under construction\n",
      //   __func__, mode, x, y, width, height, color);

  return 1;
}

int (video_test_pattern)(uint16_t UNUSED(mode), uint8_t UNUSED(no_rectangles), uint32_t UNUSED(first), uint8_t UNUSED(step)) {
  /* To be completed */
  //printf("%s(0x%03x, %u, 0x%08x): under construction\n", __func__, mode, no_rectangles, first);

  return 1;
}
