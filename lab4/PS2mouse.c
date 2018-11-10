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
	if(sys_inb(OUT_BUF, &status) != OK)
	{
		error = true;
		return;
	}

	error = false;


	//printf("status_ih %x\n", status);
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

int set_mouse(uint8_t write)
{
	uint32_t read = 0;

	while(read != ACK)	
	{
		sys_outb(STAT_REG, KBC_CMD);
		sys_inb(STAT_REG, &stat);

		if(!((stat & IBF) >> 1))
		{
			sys_outb(OUT_BUF, write);

			sys_inb(OUT_BUF, &read);

			if(read == NACK)
			{
				continue;
			}
			if(read == ERROR)
			{
				return 1;
			}
		}
	}

	return;
}

int mouse_write_int()
{
	if(set_mouse(STREAM_MODE) != OK)
	{
		return 1;
	}

	if(set_mouse(EN_DATA) != OK)
	{
		return 1;
	}
}

int disable_int()
{
	if(set_mouse(DIS_DATA) != OK)
	{
		return 1;
	}
}


int disable_poll()
{
	if(set_mouse(STREAM_MODE) != OK)
	{
		return 1;
	}

	if(set_mouse(DIS_DATA) != OK)
	{
		return 1;
	}

}

int mouse_poll_cmd(bool finish)
{
	uint32_t ms_cmd;

	sys_outb(STAT_REG, RD_CMD_B);
	sys_inb(OUT_BUF, &ms_cmd);

	if(ms_cmd == ERROR)
	{
		return 1;
	}

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

typedef enum {INIT, DRAWL, DRAWR, HOLD, FINAL} state_t;
typedef enum {LB_PRESSED, LB_RELEASE, RB_PRESSED, RB_RELEASED, BUTTON_EV, MOUSE_MOV} mouse_ev_t;

struct mouse_ev {
	enum mouse_ev_t type;
    int16_t delta_x, delta_y;
};

void check_v_line(enum mouse_ev_t evt) 
{
	static state_t st = INIT; // initial state; keep state

	switch (st) 
	{
		case INIT:
			if( evt->type == LB_PRESSED )
			{
				state = DRAWL;
				break;
			}
		case DRAWL:
			if( evt->type == LB_RELEASE ) 
			{
				state = HOLD;
			} 
			break;
		case HOLD:
			if( evt->type == RB_PRESSED ) 
			{
				state = DRAWR;
				break;
			}
			else if( evt->type == LB_PRESSED ) 
				state = DRAWL;
			else
				state = INIT;
		case DRAWR:
			if( evt->type == RB_RELEASE ) 
				state = FINAL;
			else if( evt->type == LB_PRESSED )
				state = DRAWL;
			else
				state = INIT;
			break;
		default:
			break;
	}
}

bool validMoveL(uint8_t tolerance)
{
	if( (pp.delta_x + pp.delta_y + tolerance) < 0)
		return false;

	return true;
}

bool validMoveR(uint8_t tolerance)
{
	if( (pp.delta_x + pp.delta_y - tolerance) < 0)
		return false;

	return true;
}

// bool validMoveC(uint8_t tolerance)
// {
// 	if( abs(pp.delta_x + pp.delta_y) < tolerance)
// 		return false;

// 	return true;
// }
