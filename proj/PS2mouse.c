#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h>

#include "i8042.h"
#include "PS2mouse.h"

//VARIABLE INITIALIZATION
static int hook_id_mouse = 2;
uint32_t status_mouse = 0;
bool error_mouse = false;
struct packet pp;
int mouseX = 512;
int mouseY = 384;


//FUNCTIONS
//////////////////////////////////////////////////////////////////

int (mouse_subscribe_int)(uint8_t *bit_no)
{
	*bit_no = hook_id_mouse;

	//checks if the sys call was valid
	if (sys_irqsetpolicy(IRQMOUSE, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id_mouse) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

int (mouse_unsubscribe_int)() 
{
	//checks if the sys call was valid
	if (sys_irqrmpolicy(&hook_id_mouse) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

void (mouse_ih)(void) 
{
	if (sys_inb(OUT_BUF, &status_mouse) != OK)
	{
		error_mouse = true;
		return;
	}

	error_mouse = false;

	return;
}

//////////////////////////////////////////////////////////////////

void packet_create()
{
	pp.lb = (pp.bytes[0] & L_B);
	pp.rb = (pp.bytes[0] & R_B) >> 1;
	pp.mb = (pp.bytes[0] & M_B) >> 2;

	if ((pp.bytes[0] & BIT(4)) >> 4 == 0)
	{
		pp.delta_x = (pp.bytes[1] & 0x00FF);
	}
	else
	{
		pp.delta_x = (pp.bytes[1] | 0xFF00);	
  }
	
	if ((pp.bytes[0] & BIT(5)) >> 5 == 0)
	{
		pp.delta_y = (pp.bytes[2] & 0x00FF);
	}
	else
	{
		pp.delta_y = (pp.bytes[2] | 0xFF00);
  }
	
	pp.x_ov = (pp.bytes[0] & X_OVF) >> 6;
	pp.y_ov = (pp.bytes[0] & Y_OVF) >> 7;
}

//////////////////////////////////////////////////////////////////

int set_mouse(uint8_t write)
{
	uint32_t read = 0, stat;

	while(read != ACK)	
	{
		if (sys_outb(STAT_REG, KBC_CMD) != OK)
		{
			return 1;
		}
		if (sys_inb(STAT_REG, &stat) != OK)
		{
			return 1;
		}

		if(!((stat & IBF) >> 1))
		{
			if (sys_outb(OUT_BUF, write) != OK)
			{
				return 1;
			}

			if (sys_inb(OUT_BUF, &read) != OK)
			{
				return 1;
			}

			if (read == NACK)
			{
				continue;
			}
			if (read == ERROR)
			{
				return 1;
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

int mouse_write_int()
{
	if (set_mouse(STREAM_MODE) != OK)
	{
		return 1;
	}

	if (set_mouse(EN_DATA) != OK)
	{
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////

int disable_int()
{
	if (set_mouse(DIS_DATA) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

void currentMousePosition()
{
	mouseX += pp.delta_x;
  mouseY -= pp.delta_y;

  if (mouseX < 0) 
	{
		mouseX = 0;
	}

  if (mouseY < 0) 
	{
		mouseY = 0;
	}

	if (mouseX > 1024) 
	{
		mouseX = 1024;
	}

  if (mouseY > 768)
	{
		mouseY = 768;
	}
}
