#include <lcom/lcf.h>
#include <stdint.h>

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"
#include "interface.h"
#include "menu.h"
#include "score.h"
#include "PS2mouse.h"
#include "serialPort.h"

static int hook_id_uart = 3;
uint32_t char_containerx = 2;
uint32_t char_containery = 2;

//FUNCTIONS
//////////////////////////////////////////////////////////////////

uint32_t getContainerx()
{
  return char_containerx;
}

uint32_t getContainery()
{
  return char_containery;
}

int serialPort_subscribe_int(uint8_t *bit_no_uart)
{
	*bit_no_uart = hook_id_uart;

	//checks if the sys call was valid
	if (sys_irqsetpolicy(COM1_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id_uart) != OK)
	{
		return 1;
	}

  if (sys_outb(IER, RD_ENABLE | RLS_ENABLE) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

int serialPort_unsubscribe_int() 
{
	//checks if the sys call was valid
	if (sys_irqrmpolicy(&hook_id_uart) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

int get_RBR(uint32_t *byte)
{
  int i = 0;
  uint32_t lsrByte;

  while (i < 5)
  {
    if (sys_inb(LSR, &lsrByte) == OK)
    {
      if (lsrByte & RECEIVER_DATA)
      {
        sys_inb(RBR, byte);
				printf("Received char: %c \n", (char) *byte); //for debuging
        tickdelay(micros_to_ticks(SP_DELAY));
        return 0;
      }
    }

    i++;
  }
  return 1;
}

//////////////////////////////////////////////////////////////////

int write_to_THR(uint32_t byte)
{
  int i = 0;
  uint32_t lsrByte;

  while (i < 5)
  {
    if (sys_inb(LSR, &lsrByte) == OK)
    {
      if (lsrByte & THR_EMPTY)
      {
        sys_outb(THR, byte);
				printf("TRANSMITED char: %c \n", (char) byte); //for debuging
        return 0;
      }
    }

    i++;
  }
  return 1;
}

//////////////////////////////////////////////////////////////////

int clear_RBR()
{
  uint32_t clear, iirByte;

  if (sys_inb(IIR, &iirByte) != OK)
  {
    return 1;
  }

  if (iirByte & RD)
  {
    if (get_RBR(&clear) != 0)
    {
      return 1;
    }
  }

  return 0;
}

//////////////////////////////////////////////////////////////////

int serialPort_handler(uint32_t *byte)
{
  uint32_t iirByte, lsrByte;

  if (sys_inb(IIR, &iirByte) != OK)
  {
    return 1;
  }

  if (iirByte & RD)
  {
    if (get_RBR(byte) != 0)
    {
      return 1;
    }
  }
  else if ((iirByte & RLS) == RLS)
  {
    if (sys_inb(LSR, &lsrByte) != OK)
    {
      return 1;
    }
  }

  return 0;
}

//////////////////////////////////////////////////////////////////

int playerY_sync(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart)
{
  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_keyboard = BIT(bit_no_kb);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);
  uint32_t irq_set_serialPort = BIT(bit_no_uart);
  
  drawBitmap(images.waiting, 0, 0, ALIGN_LEFT);
  double_buffer_to_video_mem();

  int ipc_status, r;
  bool esc = false;
  message msg;

  clear_RBR();

  while (!esc && char_containerx != 'x')
  {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
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
            write_to_THR('y');
          }

          if (msg.m_notify.interrupts & irq_set_keyboard)
          { /* subscribed interrupt */
          
            kbc_asm_ih();
            if (status == ESC_BK)
            {
              esc = true;
            }
          }

          if (msg.m_notify.interrupts & irq_set_mouse)
          {/* subscribed interrupt */
            
            mouse_ih();
            /*	uint32_t stat;
	            sys_inb(STAT_REG, &stat); 
	            if( stat & OBF ) 
	            {
	            	sys_inb(OUT_BUF, &status);
              }*/
          }

          if (msg.m_notify.interrupts & irq_set_serialPort)
          { /* subscribed interrupt */
            serialPort_handler(&char_containerx);
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
  }
  /* to reset global variables for a new game */
  if (esc)
  {
    return 1;
  }
  
  return 0;
}

//////////////////////////////////////////////////////////////////

int playerX_sync(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart)
{
  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_keyboard = BIT(bit_no_kb);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);
  uint32_t irq_set_serialPort = BIT(bit_no_uart);
  
  drawBitmap(images.waiting, 0, 0, ALIGN_LEFT);
  double_buffer_to_video_mem();

  int ipc_status, r;
  bool esc = false;
  message msg;

  clear_RBR();

  while (!esc && char_containery != 'y')
  {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
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
            write_to_THR('x');
          }

          if (msg.m_notify.interrupts & irq_set_keyboard)
          { /* subscribed interrupt */
          
            kbc_asm_ih();
            if (status == ESC_BK)
            {
              esc = true;
            }
          }

          if (msg.m_notify.interrupts & irq_set_mouse)
          { /* subscribed interrupt */
            mouse_ih();
          /*  uint32_t stat;
	          sys_inb(STAT_REG, &stat); 
          
	          if( stat & OBF ) 
	          {
	          	sys_inb(OUT_BUF, &status);
            }*/
          }

          if (msg.m_notify.interrupts & irq_set_serialPort)
          { /* subscribed interrupt */
            serialPort_handler(&char_containery);
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
  }

  /* to reset global variables for a new game */
  if (esc)
  {
    return 1;
  }
  
  return 0;
}

////////////////////////////// MULTI ///////////////////////////////////////

int gameMultiX(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart)
{
    init_arrows();
    reset_score_counter();
    drawBitmap(images.background, 0, 0, ALIGN_LEFT);

    drawBitmap(images.cromossomaX_idle, 650, 300, ALIGN_LEFT);
    
    drawBitmap(images.pad, 462, 450, ALIGN_LEFT);
    double_buffer_to_video_mem();

    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_mouse = BIT(bit_no_mouse);
    uint32_t irq_set_serialPort = BIT(bit_no_uart);
    
    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1, s = 1;
    bool esc = false, p_key = false, wait = false;
    message msg;

    while (!esc)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
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
                        if (!p_key)
                        {
                            timer_int_handler();
                            arrowProcessing(1);
                        }
                        if((timer_counter % 60) == 0)
                        {
                            printf("SCORE %d", getScore());
                            write_to_THR((uint32_t)getScore());
                            
                        }
                    }

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

                        if (status == ESC_BK)
                        {
                            esc = true;
                        }

                        if (status == P_KEY_BK)
                        {
                            if (!p_key)
                            {
                                p_key = true;
                                drawBitmap(images.pause, 180, 116, ALIGN_LEFT);
                                double_buffer_to_video_mem();
                            }
                            else
                            {
                                p_key = false;
                            }
                        }

                        if (status == W_KEY || status == A_KEY || status == S_KEY ||status  == D_KEY)
                        {
                            keyboardArrows();
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
                            currentMousePosition();
                            get_powerup();
                            s = 1;
                        }
                    }

                    if (msg.m_notify.interrupts & irq_set_serialPort)
                    { /* subscribed interrupt */
                      serialPort_handler(&char_containery);
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        }
        else
        {   /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

        /* game ends after 45 seconds */
        if (timer_counter > 2700)
        {   
            end_game();
            return 0;
        }

        size = 1;
    }

    /* to reset global variables for a new game */
    if (esc)
    {
        end_game();
    }
    
    return 0;
}

int gameMultiY(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart)
{
    init_arrows();
    reset_score_counter();
    drawBitmap(images.background, 0, 0, ALIGN_LEFT);

    drawBitmap(images.cromossomaY_idle, 650, 300, ALIGN_LEFT);

    drawBitmap(images.pad, 462, 450, ALIGN_LEFT);
    double_buffer_to_video_mem();

    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_mouse = BIT(bit_no_mouse);
    uint32_t irq_set_serialPort = BIT(bit_no_uart);
    
    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1, s = 1;
    bool esc = false, p_key = false, wait = false;
    message msg;

    while (!esc)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
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
                        if (!p_key)
                        {
                            timer_int_handler();
                            arrowProcessing(2);
                        }
                        if((timer_counter % 60) == 0)
                        {
                            printf("SCORE %d", getScore());
                            write_to_THR((uint32_t)getScore());

                        }
                    }

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

                        if (status == ESC_BK)
                        {
                            esc = true;
                        }

                        if (status == P_KEY_BK)
                        {
                            if (!p_key)
                            {
                                p_key = true;
                                drawBitmap(images.pause, 180, 116, ALIGN_LEFT);
                                double_buffer_to_video_mem();
                            }
                            else
                            {
                                p_key = false;
                            }
                        }

                        if (status == W_KEY || status == A_KEY || status == S_KEY ||status  == D_KEY)
                        {
                            keyboardArrows();
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
                            currentMousePosition();
                            get_powerup();
                            s = 1;
                        }
                    }

                    if (msg.m_notify.interrupts & irq_set_serialPort)
                    { /* subscribed interrupt */
                      serialPort_handler(&char_containerx);
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        }
        else
        {   /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

        /* game ends after 45 seconds */
        if (timer_counter > 2700)
        {   
            end_game();
            return 0;
        }

        size = 1;
    }

    /* to reset global variables for a new game */
    if (esc)
    {
        end_game();
    }
    
    return 0;
}


