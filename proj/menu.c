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
#include "PS2mouse.h"


//VARIABLE INITIALIZATION
static state_t state = START;
bool exit_game = false;
bool do_not_change = false;


//FUNCTIONS
//////////////////////////////////////////////////////////////////

int menu()
{
  //images are loaded at the begining
  loadImages();

  default_menu();

  uint8_t bit_no_timer;
  uint8_t bit_no_kb;
  uint8_t bit_no_mouse;
  
  if (timer_subscribe_int(&bit_no_timer) != OK)
  {
    return 1;
  }

  if (kbd_subscribe_int(&bit_no_kb) != OK)
  {
    return 1;
  }

  if (mouse_write_int() != OK)
  {
    return 1;
  }
  if (mouse_subscribe_int(&bit_no_mouse) != OK)
  {
    return 1;
  }

  uint32_t irq_set_keyboard = BIT(bit_no_kb);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);

  uint8_t byte1[1], byte2[2];
  int ipc_status, r, size = 1, s = 1;
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

            change_state(bit_no_timer, bit_no_kb, bit_no_mouse);
            change_buttons();
          }

          if (msg.m_notify.interrupts & irq_set_mouse)
          { /* subscribed interrupt */

            mouse_ih();
            //if (error_mouse == true)
            //{
            //    continue;
            //}
            if (s == 1)
            {
              if (status_mouse & BIT(3))
              {
                pp.bytes[0] = status_mouse;
                s++;
              }
              continue;
            } 
            if (s == 2)
            {
              pp.bytes[1] = status_mouse;
              s++;
              continue;
            }
            
            if (s == 3)
            {
              pp.bytes[2] = status_mouse;
              packet_create();
              s = 1;
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

    do_not_change = false;
    size = 1;
  }

  if (kbd_unsubscribe_int() != OK)
  {
    return 1;
  }

  if (mouse_unsubscribe_int() != OK)
  {  
    return 1;
  }
  
  if (disable_int() != OK)
  {
    return 1;
  }

  if (timer_unsubscribe_int() != OK)
  {
    return 1;
  }

  /* frees memory occupied by the loaded images when exiting */
  deleteImages();

  /* to clear the buffer when exiting */
  uint32_t stat;
	sys_inb(STAT_REG, &stat); 
		
	if( stat & OBF ) 
	{
		sys_inb(OUT_BUF, &status);
  }

  return 0;
}

//////////////////////////////////////////////////////////////////

void change_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse)
{
  switch (status)
  {
    case W_KEY:
      state = (state - 1) % 4;
      break;
    case S_KEY:
      state = (state + 1) % 4;
      break;
    case ENTER_KEY_BK:
      if (state == EXIT)
        exit_game = true;
      else if (state == START)
        game(bit_no_timer, bit_no_kb, bit_no_mouse);
      else if (state == INSTRUCTIONS)
      {
        drawBitmap(images.instructions, 0, 0, ALIGN_LEFT);
        double_buffer_to_video_mem();
        do_not_change = true; //so that it doesn't print the menu over the instructions
      }
      break;
  }
}

//////////////////////////////////////////////////////////////////

void change_buttons()
{ 
  drawBitmap(images.menu, 0, 0, ALIGN_LEFT); 

  if (do_not_change)
  {
    return;
  }

  switch(state % 4)
  {
    case 0:
      drawBitmap(images.start_selected, 342, 305, ALIGN_LEFT); 
      drawBitmap(images.highscores_not_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(images.instructions_not_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(images.exit_not_selected, 342, 665, ALIGN_LEFT);
      break;
    case 1:
      drawBitmap(images.start_not_selected, 342, 305, ALIGN_LEFT);
      drawBitmap(images.highscores_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(images.instructions_not_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(images.exit_not_selected, 342, 665, ALIGN_LEFT);
      break;
    case 2:
      drawBitmap(images.start_not_selected, 342, 305, ALIGN_LEFT);
      drawBitmap(images.highscores_not_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(images.instructions_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(images.exit_not_selected, 342, 665, ALIGN_LEFT);
      break;
    case 3:
      drawBitmap(images.start_not_selected, 342, 305, ALIGN_LEFT);
      drawBitmap(images.highscores_not_selected, 342, 425, ALIGN_LEFT);
      drawBitmap(images.instructions_not_selected, 342, 545, ALIGN_LEFT);
      drawBitmap(images.exit_selected, 342, 665, ALIGN_LEFT);
      break;
  } 

  double_buffer_to_video_mem();
}

//////////////////////////////////////////////////////////////////

void default_menu()
{
  drawBitmap(images.menu, 0, 0, ALIGN_LEFT);
  drawBitmap(images.start_selected, 342, 305, ALIGN_LEFT);
  drawBitmap(images.instructions_not_selected, 342, 545, ALIGN_LEFT);
  drawBitmap(images.exit_not_selected, 342, 665, ALIGN_LEFT);
  drawBitmap(images.highscores_not_selected, 342, 425, ALIGN_LEFT);
  double_buffer_to_video_mem();
}
