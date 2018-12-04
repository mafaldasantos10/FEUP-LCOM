#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"

uint16_t y = 768;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) 
{
  printf("(%d, %p): under construction\n", argc, argv);

  if(vg_init(0x144) == NULL)
  {
  	return 1;
  }

	Bitmap * background = loadBitmap("/home/lcom/labs/proj/bitmap/discof.bmp"); //local onde esta
  drawBitmap(background, 0, 0, ALIGN_LEFT);
  Bitmap * pad = loadBitmap("/home/lcom/labs/proj/bitmap/pad.bmp"); //local onde esta
  drawBitmap(pad, 430, 358, ALIGN_LEFT);
  Bitmap * arrow1 = loadBitmap("/home/lcom/labs/proj/bitmap/arrow.bmp"); //local onde esta
  Bitmap * cromossoma1 = loadBitmap("/home/lcom/labs/proj/bitmap/cromossoma1.bmp"); //local onde esta
  Bitmap * cromossoma2 = loadBitmap("/home/lcom/labs/proj/bitmap/cromossoma2.bmp"); //local onde esta
  drawBitmap(cromossoma1, 412, 50, ALIGN_LEFT);

  uint8_t bit_no_kb, bit_no_timer;

  if (kbd_subscribe_int(&bit_no_kb) != OK)
  {
    return 1;
  }

  if (timer_subscribe_int(&bit_no_timer) != OK)
  {
    return 1;
  }

  uint32_t irq_set_keyboard = BIT(bit_no_kb);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  uint8_t byte1[1], byte2[2];
  int ipc_status, r, size = 1;
  bool esc = true, make = true, wait = false;
  message msg;

  drawBitmap(pad, 430, 358, ALIGN_LEFT);
  drawBitmap(arrow1, 430, 768, ALIGN_LEFT);

  while(esc)
  { 
    /* Get a request message. */ 
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
    {
      printf("driver_receive failed with: %d", r); 
      continue;
    }
    if (is_ipc_notify(ipc_status))
    { /* received notification */ 
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE: /* hardware interrupt notification */ 
          if (msg.m_notify.interrupts & irq_set_timer) 
          {
            timer_int_handler();
          }

          if (msg.m_notify.interrupts & irq_set_keyboard) 
          { /* subscribed interrupt */ 

            kbc_ih();
          
            if (status == MSB)
            {
              wait = true;
              continue;
            } 

            if (wait == true)
            {
              wait = false;
              size = 2;
            }

            if (status == ESC_BK)
            {
              esc = false;
              make = false;
            }

            if (status == 0x1c)
            {
              keep = false;
              drawBitmap(background, 0, 0, ALIGN_LEFT);
              drawBitmap(pad, 438, 358, ALIGN_LEFT);
              drawBitmap(cromossoma2, 412, 50, ALIGN_LEFT);  
              sleep(1);
              drawBitmap(cromossoma1, 412, 50, ALIGN_LEFT);
            }

            if ((status >> 7) == BIT(0))
            {
              make = false;
            }

            if (size == 1)
            {
              byte1[0] = status;
            } 

            if (size == 2)
            {
              byte2[0] = MSB;
              byte2[1] = status;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */ 
      } 
    } 
    else
    { /* received a standard message, not a notification */ 
      /* no standard messages expected: do nothing */ 
    }

    size = 1;
    make = true;

    if(!keep)
    {
      continue;
    }
    else
    {
      pix_map_move_pos(pad, background, arrow1, cromossoma1, 358, 5, 30);
    }
  }

  if (kbd_unsubscribe_int() != OK)
  {  
    return 1;
  }

  if (timer_unsubscribe_int() != OK)
  {
    return 1;
  }

  if (vg_exit())
  {
    return 1;
  }
 
  return 0;
}
