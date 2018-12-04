#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"

uint16_t y = 768;
uint8_t fr_rate = 30;
uint16_t speed = 5;
int arrow = 0;
bool up = false;
bool down = false;
bool left = false;
bool right = false;



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
  srand(time(NULL));
  printf("(%d, %p): under construction\n", argc, argv);

  if(vg_init(0x144) == NULL)
  {
  	return 1;
  }

	Bitmap * background = loadBitmap("/home/lcom/labs/proj/bitmap/discof.bmp"); 
  drawBitmap(background, 0, 0, ALIGN_LEFT);
  Bitmap * pad = loadBitmap("/home/lcom/labs/proj/bitmap/pad.bmp"); 
  drawBitmap(pad, 430, 358, ALIGN_LEFT);
  Bitmap * arrowup = loadBitmap("/home/lcom/labs/proj/bitmap/arrowup.bmp"); 
  Bitmap * arrowright = loadBitmap("/home/lcom/labs/proj/bitmap/arrowright.bmp"); 
  Bitmap * arrowdown = loadBitmap("/home/lcom/labs/proj/bitmap/arrowdown.bmp"); 
  Bitmap * arrowleft = loadBitmap("/home/lcom/labs/proj/bitmap/arrowleft.bmp"); 
  Bitmap * cromossoma1 = loadBitmap("/home/lcom/labs/proj/bitmap/cromossoma1.bmp"); 
  Bitmap * cromossomaup = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaup.bmp"); 
  drawBitmap(cromossoma1, 412, 50, ALIGN_LEFT);
  Bitmap * perfect = loadBitmap("/home/lcom/labs/proj/bitmap/perfect.bmp"); 
  Bitmap * great = loadBitmap("/home/lcom/labs/proj/bitmap/great.bmp"); 
  Bitmap * okay = loadBitmap("/home/lcom/labs/proj/bitmap/okay.bmp"); 
  Bitmap * miss = loadBitmap("/home/lcom/labs/proj/bitmap/miss.bmp"); 

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

 // drawBitmap(pad, 430, 358, ALIGN_LEFT);
 // drawBitmap(arrow1, 430, 768, ALIGN_LEFT);


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

            keyboarArrows(cromossomaup, pad, background, cromossoma1, okay, miss, perfect, great);

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
      sleep(1);
      int arrowRate();     
      y = 768;
      keep = true;
      continue;
    }
    else
    {
      switch(arrow)
      {
        case 0:
        {
          right = true;
          pix_map_move_pos(pad, background, arrowright, cromossoma1, 358, speed, fr_rate);
          break;
        }
         case 1:
        {
          up = true;
          pix_map_move_pos(pad, background, arrowup, cromossoma1, 358, speed, fr_rate);
          break;
        }
         case 2:
        {
          down = true;
          pix_map_move_pos(pad, background, arrowdown, cromossoma1, 358, speed, fr_rate);
          break;
        }
         case 3:
        {
          left = true;
          pix_map_move_pos(pad, background, arrowleft, cromossoma1, 358, speed, fr_rate);
          break;
        }
      }
      //pix_map_move_pos(pad, background, arrow, cromossoma1, 358, speed, fr_rate);
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
