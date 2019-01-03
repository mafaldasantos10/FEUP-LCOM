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
#include "serialPort.h"


//VARIABLE INITIALIZATION
static st_t st = START;
static state_t state = SINGLE;
static state2_t state2 = C_Y;

bool exit_game = false;
bool do_not_change = true;
char name[25];


//FUNCTIONS
//////////////////////////////////////////////////////////////////

int menu()
{
  /* images are loaded at the begining */
  loadImages();

  /* asks for a name */
  printf("aqui!!\n");
  drawBitmap(images.name_menu, 0, 0, ALIGN_LEFT);
  double_buffer_to_video_mem();

  load_score_from_file();

  uint8_t bit_no_timer;
  uint8_t bit_no_kb;
  uint8_t bit_no_mouse;
  uint8_t bit_no_uart;
  
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

   if (serialPort_subscribe_int(&bit_no_uart) != OK)
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

            if (st == START)
            {
              if (strlen(name) == 20 && status != BACKSPACE_KEY)
                continue;

              //printf("entrei aqui:\n");
              convert_key();
              print_sentence(name, 180, 500);
              double_buffer_to_video_mem();
            }

            if (status == W_KEY || status == A_KEY || status == S_KEY || status  == D_KEY ||
                status  == ENTER_KEY_BK || status  == ESC_KEY || st == START)
            {
              change_menu_state(bit_no_timer, bit_no_kb, bit_no_mouse);
              change_buttons();
            }
          }

          if (msg.m_notify.interrupts & irq_set_mouse)
          { /* subscribed interrupt */

            mouse_ih();

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

void change_menu_state(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse)
{
  if (do_not_change && st == MENU && status != ESC_KEY)
  {
    return;
  }

  switch (status)
  {
    case ESC_KEY:
      if (st == MENU)
      {
        //printf("do: %d\n", do_not_change);
        if (!do_not_change)
        {
          st = START;
          state = SINGLE;
          state2 = C_Y;
          //printf("aqui2!!\n");
          drawBitmap(images.name_menu, 0, 0, ALIGN_LEFT);
          double_buffer_to_video_mem();
        }

        if (state == INSTRUCTIONS || state == HIGHSCORES)
          do_not_change = false;
      }

      if (st == CHARACTER)
      {
        st = MENU;
        state = SINGLE;
        state2 = C_Y;
        drawBitmap(images.menu, 0, 0, ALIGN_LEFT);
        double_buffer_to_video_mem();
      }
      break;

    case W_KEY:
      if (st == MENU)
      {
        if (state == SINGLE)
        {
          state = EXIT;
          break;
        }
        state = (state - 1) % 5;
      }
      break;

    case S_KEY:
      if (st == MENU)
      {
        state = (state + 1) % 5;
        
      }
      break;

    case A_KEY:
      if (st == CHARACTER)
      {
        //printf("tens razao!!\n");
        state2 = (state2 - 1) % 2;
        
      }
      break;

    case D_KEY:
      if (st == CHARACTER)
      {
        
        state2 = (state2 + 1) % 2; 
      }
      break;

    /* any state */
    case ENTER_KEY_BK:

      //printf("st: %d\n", st);

      if (st == START)
      {
        printf("tens razao!! %d\n", strlen(name));
        if (strlen(name) == 0)
        {
          print_sentence("empty name!", 360, 600);
          double_buffer_to_video_mem();
          drawBitmap(images.name_menu, 0, 0, ALIGN_LEFT);
          return;
        }
        
        set_current_player_name(name);
        do_not_change = false;
        st = MENU;
        default_menu();
        return;
      }

      if (st == MENU)
      {
        if (state == EXIT)
        {
          exit_game = true;
        }

        else if (state == SINGLE)
        {
          //printf("é aqui: %d\n", st);
          st = CHARACTER;
          drawBitmap(images.cromossomaY, 0, 0, ALIGN_LEFT);
          double_buffer_to_video_mem();
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
      }

      else if (st == CHARACTER)
      {
        if (state2 == C_Y)
        {
          //printf("hey\n");
          //printf("GAME? %d\n", st);
          
          set_current_player_cromossoma(0);
        } 
        else if (state2 == C_X)
        {
          set_current_player_cromossoma(1);
          // por o boneco X
        }

        st = GAME;
        game(bit_no_timer, bit_no_kb, bit_no_mouse);
      }

      break;
  }
}

//////////////////////////////////////////////////////////////////

void change_buttons()
{ 
  if (do_not_change)
  {
    return;
  }
  
  //printf("st_d: %d\n", st);

  if (st == MENU)
  {
    drawBitmap(images.menu, 0, 0, ALIGN_LEFT); 

    switch(state % 5)
    {
      case 0:
        drawBitmap(images.singleplayer_selected, 372, 305, ALIGN_LEFT);
        drawBitmap(images.multiplayer_not_selected, 372, 395, ALIGN_LEFT);
        drawBitmap(images.highscores_not_selected, 372, 485, ALIGN_LEFT);
        drawBitmap(images.instructions_not_selected, 372, 575, ALIGN_LEFT);
        drawBitmap(images.exit_not_selected, 372, 665, ALIGN_LEFT);
        break;
      case 1:
        drawBitmap(images.singleplayer_not_selected, 372, 305, ALIGN_LEFT);
        drawBitmap(images.multiplayer_selected, 372, 395, ALIGN_LEFT);
        drawBitmap(images.highscores_not_selected, 372, 485, ALIGN_LEFT);
        drawBitmap(images.instructions_not_selected, 372, 575, ALIGN_LEFT);
        drawBitmap(images.exit_not_selected, 372, 665, ALIGN_LEFT);
        break;
      case 2:
        drawBitmap(images.singleplayer_not_selected, 372, 305, ALIGN_LEFT);
        drawBitmap(images.multiplayer_not_selected, 372, 395, ALIGN_LEFT);
        drawBitmap(images.highscores_selected, 372, 485, ALIGN_LEFT);
        drawBitmap(images.instructions_not_selected, 372, 575, ALIGN_LEFT);
        drawBitmap(images.exit_not_selected, 372, 665, ALIGN_LEFT);
        break;
      case 3:
        drawBitmap(images.singleplayer_not_selected, 372, 305, ALIGN_LEFT);
        drawBitmap(images.multiplayer_not_selected, 372, 395, ALIGN_LEFT);
        drawBitmap(images.highscores_not_selected, 372, 485, ALIGN_LEFT);
        drawBitmap(images.instructions_selected, 372, 575, ALIGN_LEFT);
        drawBitmap(images.exit_not_selected, 372, 665, ALIGN_LEFT);
        break;
      case 4:
        drawBitmap(images.singleplayer_not_selected, 372, 305, ALIGN_LEFT);
        drawBitmap(images.multiplayer_not_selected, 372, 395, ALIGN_LEFT);
        drawBitmap(images.highscores_not_selected, 372, 485, ALIGN_LEFT);
        drawBitmap(images.instructions_not_selected, 372, 575, ALIGN_LEFT);
        drawBitmap(images.exit_selected, 372, 665, ALIGN_LEFT);
        break;
    }
  }

  else if (st == CHARACTER)
  {
    //printf("state fora: %d\n", state2);
    switch(state2 % 2)
    {
      case 0:
        drawBitmap(images.cromossomaY, 0, 0, ALIGN_LEFT);
        break;
      case 1:
        drawBitmap(images.cromossomaX, 0, 0, ALIGN_LEFT);
        break;
    }
  }

  if (st != START)
    double_buffer_to_video_mem();
}

//////////////////////////////////////////////////////////////////

void default_menu()
{
  drawBitmap(images.menu, 0, 0, ALIGN_LEFT);
  drawBitmap(images.singleplayer_selected, 372, 305, ALIGN_LEFT);
  drawBitmap(images.multiplayer_not_selected, 372, 395, ALIGN_LEFT);
  drawBitmap(images.highscores_not_selected, 372, 485, ALIGN_LEFT);
  drawBitmap(images.instructions_not_selected, 372, 575, ALIGN_LEFT);
  drawBitmap(images.exit_not_selected, 372, 665, ALIGN_LEFT);
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

void convert_key()
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

//////////////////////////////////////////////////////////////////

void default_state()
{
  default_menu();
  st = MENU;
  state = SINGLE;
  state2 = C_Y;
}
