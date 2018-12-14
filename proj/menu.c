#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"
#include "interface.h"
#include "menu.h"

static state_t state = START;
bool exit_game = false;

int menu()
{
  Bitmap * start_selected = loadBitmap("/home/lcom/labs/proj/bitmap/start2.bmp");
  Bitmap * instructions_selected = loadBitmap("/home/lcom/labs/proj/bitmap/instructions2.bmp"); 
  Bitmap * highscores_selected = loadBitmap("/home/lcom/labs/proj/bitmap/highscores2.bmp");
  Bitmap * exit_selected = loadBitmap("/home/lcom/labs/proj/bitmap/exit2.bmp"); 

  Bitmap * start_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/start1.bmp");
  Bitmap * instructions_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/instructions1.bmp"); 
  Bitmap * highscores_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/highscores1.bmp");  
  Bitmap * exit_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/exit1.bmp");

  Bitmap * menu = loadBitmap("/home/lcom/labs/proj/bitmap/menu.bmp");

  drawBitmap(menu, 0, 0, ALIGN_LEFT); 
  drawBitmap(start_selected, 342, 305, ALIGN_LEFT);
  drawBitmap(instructions_not_selected, 342, 545, ALIGN_LEFT);
  drawBitmap(exit_not_selected, 342, 665, ALIGN_LEFT);
  drawBitmap(highscores_not_selected, 342, 425, ALIGN_LEFT);

  double_buffer_to_video_mem();

  uint8_t bit_no_kb;

  if (kbd_subscribe_int(&bit_no_kb) != OK)
  {
    return 1;
  }

  uint32_t irq_set_keyboard = BIT(bit_no_kb);

  uint8_t byte1[1], byte2[2];
  int ipc_status, r, size = 1;
  bool wait = false;
  message msg;

  while (!exit_game) 
  {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:  /* hardware interrupt notification */
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

            if (size == 1)
            {
              byte1[0] = status;
            }

            if (size == 2)
            {
              byte2[0] = MSB;
              byte2[1] = status;
            }

            change_state(bit_no_kb);
            change_buttons(start_selected,start_not_selected,highscores_not_selected,  highscores_selected,  instructions_not_selected, instructions_selected,  exit_not_selected, exit_selected, menu);
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }

    size = 1;
  }

  if (kbd_unsubscribe_int() != OK)
  {
    return 1;
  }

  uint32_t stat;
	sys_inb(STAT_REG, &stat); 
		
		if( stat & OBF ) 
		{
			sys_inb(OUT_BUF, &status);
    }

  return 0;
}

void change_state(uint8_t bit_no_kb)
{
  switch(status)
  {
    case W_KEY:
      state = (state - 1) % 4;
      break;
    case S_KEY:
      state = (state + 1) % 4;
      break;
    case ENTER_KEY:
      if (state == EXIT)
        exit_game = true;
      if (state == START)
        game(bit_no_kb);
      break;
  }
}

void change_buttons(Bitmap * start_selected,Bitmap * start_not_selected, Bitmap * highscores_not_selected, Bitmap * highscores_selected, Bitmap * instructions_not_selected, Bitmap * instructions_selected, Bitmap * exit_not_selected, Bitmap * exit_selected, Bitmap * menu)
{
  drawBitmap(menu, 0, 0, ALIGN_LEFT); 

  switch(state % 4)
  {
    case 0:
      drawBitmap(start_selected, 342, 305, ALIGN_LEFT);   
      drawBitmap(highscores_not_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(instructions_not_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(exit_not_selected, 342, 665, ALIGN_LEFT);
      break;
    case 1:
      drawBitmap(start_not_selected, 342, 305, ALIGN_LEFT);
      drawBitmap(highscores_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(instructions_not_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(exit_not_selected, 342, 665, ALIGN_LEFT);
      break;
    case 2:
      drawBitmap(start_not_selected, 342, 305, ALIGN_LEFT);
      drawBitmap(highscores_not_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(instructions_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(exit_not_selected, 342, 665, ALIGN_LEFT);
      break;
      case 3:
      drawBitmap(start_not_selected, 342, 305, ALIGN_LEFT);
      drawBitmap(highscores_not_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(instructions_not_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(exit_selected, 342, 665, ALIGN_LEFT);
      break;
  } 

  double_buffer_to_video_mem();
}
