#include <lcom/lcf.h>
#include <stdint.h>

#include "serialPort.h"
#include "i8042.h"
#include "score.h"
#include "interface.h"

static int hook_id_uart = 2;

int serialPort_subscribe_int(uint8_t *bit_no)
{
	*bit_no = hook_id_uart;

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


