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
		//if(sys_inb(STAT_REG, &stat) != OK)
		//{
		//	error = true;
		//	return;
	printf("status_pre_ih %x\n", status);

	if(sys_inb(OUT_BUF, &status) != OK)
	{
		error = true;
		return;
	}

	error = false;

	printf("status_ih %x\n", status);
	return;
}

void packet_create()
{
	pp.bytes[0] = array[0];
	//printf("array4 %x\n", array[0]);
	pp.bytes[1] = array[1];
	//printf("array5 %x\n", array[1]);
	pp.bytes[2] = array[2];
	//printf("array6 %x\n", array[2]);

	pp.lb = (array[0] & L_B);
	pp.rb = (array[0] & R_B) >> 1;
	pp.mb = (array[0] & M_B) >> 2;

	if((array[0] & BIT(4)) >> 4 == 0)
	{
		pp.delta_x = (array[1] & 0x00FF);
	}
	else
	{
		pp.delta_x = (array[1] | 0xFF00);	
    }
	
	if((array[0] & BIT(5)) >> 5 == 0)
	{
		pp.delta_y = (array[2] & 0x00FF);
	}
	else
	{
		pp.delta_y = (array[2] | 0xFF00);	
    }
	
	pp.x_ov = (array[0] & X_OVF) >> 6;
	pp.y_ov = (array[0] & Y_OVF) >> 7;
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

			if ( (statpoll & (PAR_ERR | TO_ERR | AUX) ) == 0 )
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


void int_mouse(uint8_t write)
{
	uint32_t read;
	int ok = 0;

	while(!ok)	
	{
		sys_inb(STAT_REG, &stat);

		if(!((stat & IBF) >> 1))
		{
			sys_outb(STAT_REG, KBC_CMD);

			sys_inb(OUT_BUF, &read);
			if(read == 0xFE)
			{
				continue;
			}
			if(read == 0xFC)
			{
				return;
			}
		}

		sys_inb(STAT_REG, &stat);

		if(!((stat & IBF) >> 1))
		{
			sys_outb(OUT_BUF, write);

			sys_inb(OUT_BUF, &read);
			if(read == 0xFE)
			{
				continue;
			}
			else if(read == 0xFC)
			{
				return;
			}
			else
				ok = 1;
		}
	}

	return;
}

void mouse_write_int()
{
	int_mouse(STREAM_MODE);
	int_mouse(EN_DATA);
}

void disable_int()
{
	int_mouse(DIS_DATA);
}

void enable_poll()
{
	int_mouse(DIS_DATA);
	int_mouse(REMOTE_MODE);
}

void disable_poll()
{
	int_mouse(DIS_DATA);
	int_mouse(STREAM_MODE);
}

int mouse_poll_cmd(bool finish)
{
	uint32_t ms_cmd;

	sys_outb(STAT_REG, RD_CMD_B);
	sys_inb(OUT_BUF, &ms_cmd);

	ms_cmd = ms_cmd & MOUSE_CMD;

	sys_outb(STAT_REG, WRT_CMD_B);

	if(!finish)
	{
		if(sys_outb(OUT_BUF, ms_cmd) != OK)
			return 1;
	}
	else
	{
		if(sys_outb(OUT_BUF, minix_get_dflt_kbc_cmd_byte()) != OK)
			return 1;
	}

	return 0;
}
