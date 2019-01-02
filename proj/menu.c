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
#include "score.h"
#include "PS2mouse.h"
#include "rtc.h"


//VARIABLE INITIALIZATION
static state_t state = START;
bool exit_game = false;
bool do_not_change = false;


//FUNCTIONS
//////////////////////////////////////////////////////////////////

int menu()
{
  /* images are loaded at the begining */
  loadImages();

  /* asks for a name */
  drawBitmap(images.name_menu, 0, 0, ALIGN_LEFT);
  double_buffer_to_video_mem();

  load_score_from_file();

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
  bool wait = false, ini = false;
  message msg;
  char name[25];

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

            kbc_asm_ih();

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

            if (status == ENTER_KEY_BK && !ini)
            {
              if (strlen(name) == 0)
              {
                print_sentence("empty name!", 360, 600);
                double_buffer_to_video_mem();
                drawBitmap(images.name_menu, 0, 0, ALIGN_LEFT);
                continue;
              }

              ini = true;
              set_current_player_name(name);
              default_menu();
              continue;
            }

            if (size == 1 && !ini)
            {
              if (strlen(name) == 20 && status != BACKSPACE_KEY)
                continue;

              convert_key(status, name);
              print_sentence(name, 180, 500);
              double_buffer_to_video_mem();
            }

            if (!ini)
              continue;

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

  save_score_to_file();

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
      {
        game(bit_no_timer, bit_no_kb, bit_no_mouse);
      }
      else if (state == INSTRUCTIONS)
      {
        drawBitmap(images.instructions, 0, 0, ALIGN_LEFT);
        double_buffer_to_video_mem();
        do_not_change = true; //so that it doesn't print the menu over the instructions
      }
      else if (state == HIGHSCORES)
      {
        drawBitmap(images.highscores, 0, 0, ALIGN_LEFT);
        print_high_scores();
        double_buffer_to_video_mem();
        do_not_change = true; //so that it doesn't print the menu over the panel
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

//////////////////////////////////////////////////////////////////

void append(char* s, char c)
{
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}

//////////////////////////////////////////////////////////////////

void convert_key(uint32_t status, char name[25])
{
  switch (status)
  {
    case A_KEY:
    {
      append(name, 'a');
      break;
    }
    case B_KEY:
    {
      append(name, 'b');
      break;
    }
    case C_KEY:
    {
      append(name, 'c');
      break;
    }
    case D_KEY:
    {
      append(name, 'd');
      break;
    }
    case E_KEY:
    {
      append(name, 'e');
      break;
    }
    case F_KEY:
    {
      append(name, 'f');
      break;
    }
    case G_KEY:
    {
      append(name, 'g');
      break;
    }
    case H_KEY:
    {
      append(name, 'h');
      break;
    }
    case I_KEY:
    {
      append(name, 'i');
      break;
    }
    case J_KEY:
    {
      append(name, 'j');
      break;
    }
    case K_KEY:
    {
      append(name, 'k');
      break;
    }
    case L_KEY:
    {
      append(name, 'l');
      break;
    }
    case M_KEY:
    {
      append(name, 'm');
      break;
    }
    case N_KEY:
    {
      append(name, 'n');
      break;
    }
    case O_KEY:
    {
      append(name, 'o');
      break;
    }
    case P_KEY:
    {
      append(name, 'p');
      break;
    }
    case Q_KEY:
    {
      append(name, 'q');
      break;
    }
    case R_KEY:
    {
      append(name, 'r');
      break;
    }
    case S_KEY:
    {
      append(name, 's');
      break;
    }
    case T_KEY:
    {
      append(name, 't');
      break;
    }
    case U_KEY:
    {
      append(name, 'u');
      break;
    }
    case V_KEY:
    {
      append(name, 'v');
      break;
    }
    case W_KEY:
    {
      append(name, 'w');
      break;
    }
    case X_KEY:
    {
      append(name, 'x');
      break;
    }
    case Y_KEY:
    {
      append(name, 'y');
      break;
    }
    case Z_KEY:
    {
      append(name, 'z');
      break;
    }
    case BACKSPACE_KEY:
    {
      /* deletes last character of "name" */
      name[strlen(name) - 1] = '\0';
      drawBitmap(images.name_menu, 0, 0, ALIGN_LEFT);
      break;
    }
  }
}
