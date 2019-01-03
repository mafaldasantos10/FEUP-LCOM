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
#include "rtc.h"
#include "menu.h"
#include "serialPort.h"

static int hook_id_uart = 3;

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

int serialPort_unsubscribe_int() 
{
	//checks if the sys call was valid
	if (sys_irqrmpolicy(&hook_id_uart) != OK)
	{
		return 1;
	}

	return 0;
}

int get_RBR(uint32_t *byte)
{
  int i = 0;
  uint32_t lsrByte;

  while(i<5)
  {
    if(sys_inb(LSR,&lsrByte) == OK)
    {
      if(lsrByte & RECEIVER_DATA)
      {
        sys_inb(RBR, byte);
				printf("Received char: %c \n",(char) *byte); //for debuging
        tickdelay(micros_to_ticks(SP_DELAY));
        return 0;
      }
    }
    i++;
  }
  return 1;
}

int write_to_THR(uint32_t byte)
{
  int i = 0;
  uint32_t lsrByte;

  while(i<5)
  {
    if(sys_inb(LSR,&lsrByte) == OK)
    {
      if(lsrByte & THR_EMPTY)
      {
        sys_outb(THR, byte);
				printf("TRANSMITED char: %c \n",(char) byte); //for debuging
        return 0;
      }
    }
    i++;
  }
  return 1;
}

int clear_RBR()
{
  uint32_t clear, iirByte;

  if(sys_inb(IIR,&iirByte) != OK)
  {
    return 1;
  }

  if(iirByte & RD)
  {
    if(get_RBR(&clear) != 0)
    {
      return 1;
    }
  }

  return 0;
}


uint32_t serialPort_handler()
{
  uint32_t byte = 0, iirByte, lsrByte;

  if(sys_inb(IIR,&iirByte) != OK)
  {
    return 1;
  }

  if(iirByte & RD)
  {
    if(get_RBR(&byte) != 0)
    {
      return 1;
    }
  }
  else if ((iirByte & RLS) == RLS)
  {
    if(sys_inb(LSR,&lsrByte) != OK)
    {
      return 1;
    }

  } 
    return byte;
}

int playery_sync(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart)
{
    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_mouse = BIT(bit_no_mouse);
    uint32_t irq_set_serialPort = BIT(bit_no_uart);
    
    drawBitmap(images.waiting, 0, 0, ALIGN_LEFT);
    double_buffer_to_video_mem();
    uint32_t char_container = 2;
    int ipc_status, r;
    bool esc = false;
    message msg;
    clear_RBR();
    while (!esc && char_container != 'x')
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

                      	uint32_t stat;
	                      sys_inb(STAT_REG, &stat); 

	                      if( stat & OBF ) 
	                      {
	                      	sys_inb(OUT_BUF, &status);
                        }


                    }
                     if (msg.m_notify.interrupts & irq_set_serialPort)
                    { /* subscribed interrupt */
                      char_container = serialPort_handler();
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
    }

    /* to reset global variables for a new game */
    if (esc)
    {
        end_game();
    }
    
    return 0;
}


int playerx_sync(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart)
{
    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_mouse = BIT(bit_no_mouse);
    uint32_t irq_set_serialPort = BIT(bit_no_uart);
    
    drawBitmap(images.waiting, 0, 0, ALIGN_LEFT);
    double_buffer_to_video_mem();

    uint32_t char_container = 2;
    int ipc_status, r;
    bool esc = false;
    message msg;
    clear_RBR();
    while (!esc && char_container != 'y')
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

                      uint32_t stat;
	                    sys_inb(STAT_REG, &stat); 
                    
	                    if( stat & OBF ) 
	                    {
	                    	sys_inb(OUT_BUF, &status);
                      }


                    }
                     if (msg.m_notify.interrupts & irq_set_serialPort)
                    { /* subscribed interrupt */
                      char_container = serialPort_handler();
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
    }

    /* to reset global variables for a new game */
    if (esc)
    {
        end_game();
    }
    
    return 0;
}



