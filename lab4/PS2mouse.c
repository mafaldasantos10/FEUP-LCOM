#include <lcom/lcf.h>
#include "PS2mouse.h"

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"

static int hook_id = 2;
uint32_t status, stat; 
bool error = false;
struct packet pp;

int (mouse_subscribe_int)(uint8_t *bit_no)
{
	*bit_no = hook_id;

	//checks if the sys call was valid
	if (sys_irqsetpolicy(IRQMOUSE, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id) != OK)
	{
		return 1;
	}

	return 0;
}

int (mouse_unsubscribe_int)() 
{
	//checks if the sys call was valid
	if (sys_irqrmpolicy(&hook_id) != OK)
	{
		return 1;
	}

	return 0;
}

void (mouse_ih)(void) 
{
	int i = 0;
	
	while( i < 5 ) 
	{
		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		
			sys_inb(OUT_BUF, &status); /* assuming it returns OK */ //validation missing

			if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
			{
				error = false;
				return;
			}
			else
			{
				error = true;
				return;
			}
		

		i++;
	}

	error = true;
	return;
}

void packet_create()
{
	
	pp.bytes[0] = array[0];
	pp.bytes[1] = array[1];
	pp.bytes[2] = array[2];
	pp.rb = (array[0]<<6>>7);
	pp.mb = (array[0]<<5>>7);
	pp.lb = (array[0]<<7>>7);
	if((array[0] & BIT(4))>>4 ==0)
	{
		pp.delta_x = (array[1] & 0x00FF);
	}
	else
	{
		pp.delta_x = (array[1] | 0xFF00);	
    }
	
	if((array[0] & BIT(5))>>5 ==0)
	{
		pp.delta_y = (array[2] & 0x00FF);
	}
	else
	{
		pp.delta_y = (array[2] | 0xFF00);	
    }
	
	pp.x_ov = (array[0]<<1>>7);
	pp.y_ov = (array[0]>>7);

}

int mouse_poll()
{
	int i = 0;
	uint32_t statpoll = 0;

	while( i < 5 )
	{
		sys_inb(STAT_REG, &statpoll); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if( statpoll & OBF ) 
		{
			sys_inb(OUT_BUF, &status); /* assuming it returns OK */

			if ( (stat & (PAR_ERR | TO_ERR | AUX) ) == 0 )
				return (uint8_t) status;
			else
			{
				return -1;
			}
		}

		i++;
	}

	return -1;
}


void enable_int()
{
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, STREAM_MODE);
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, KBC_EN);
}

void disable_int()
{
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, DISABLE);
}

void enable_poll()
{
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, DISABLE);
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, REMOTE_MODE);
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, KBC_EN);

}


void disable_poll()
{
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, DISABLE);
	sys_outb(STAT_REG, KBC_CMD);
	sys_outb(STAT_REG, STREAM_MODE);
}
