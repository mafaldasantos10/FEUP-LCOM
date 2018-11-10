#include <lcom/lcf.h>
#include "PS2mouse.h"

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"

//VARIABLES
static int hook_id = 2;
uint32_t status, stat;
bool error = false;
struct packet pp;
struct mouse_ev gest;
state_t state = INIT;


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

	return;
}


void packet_create()
{
	pp.bytes[0] = array[0];
	pp.bytes[1] = array[1];
	pp.bytes[2] = array[2];

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
		if(sys_outb(STAT_REG, KBC_CMD) != OK)
		{
			return 1;
		}
		if(sys_inb(STAT_REG, &stat) != OK)
		{
			return 1;
		}

		if(!((stat & IBF) >> 1))
		{
			if(sys_outb(OUT_BUF, write) != OK)
			{
				return 1;
			}

			if(sys_inb(OUT_BUF, &read) != OK)
			{
				return 1;
			}

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

	return 0;
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
	return 0;
}


int disable_int()
{
	if(set_mouse(DIS_DATA) != OK)
	{
		return 1;
	}

	return 0;
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

	return 0;
}


bool check_call()
{
	uint32_t ms_cmd;

	if(sys_inb(STAT_REG, &ms_cmd) != OK)
	{
		return 1;
	}

	if((ms_cmd & IBF) >> 1)
	{
		return 1;
	}

	return 0;
}


int mouse_poll_cmd()
{
	if(check_call())
	{
		return 1;
	}

	if(sys_outb(STAT_REG, WRT_CMD_B) != OK)
	{
		return 1;
	}

	if(check_call())
	{
		return 1;
	}

	if(sys_outb(OUT_BUF, minix_get_dflt_kbc_cmd_byte()) != OK)
	{
		return 1;
	}

	return 0;
}


void create_enum(uint8_t tolerance)
{
	if( pp.lb && !pp.rb && !pp.mb )
  	{
  	 	if(pp.delta_x != 0 && pp.delta_y != 0)
	 	{
 			if(validMoveL(tolerance))
 			{
 				gest.delta_x += pp.delta_x;
   				gest.delta_y += pp.delta_y;
 				gest.type = MOUSE_MOV;
 			}
 			else
 			{
 			 	gest.type = BUTTON_EV;
 			}

 			return;
 		}

    	gest.type = LB_PRESSED;
    	return;
 	}

 	if(!pp.lb && !pp.rb &&!pp.mb)
  	{
  		if(state == DRAWL)
  		{
    		gest.type = LB_RELEASED;
		}
		else if(state == HOLD)
		{
			if(pp.delta_x != 0 || pp.delta_y != 0)
	 		{
 				if(validMoveC(tolerance))
 				{
 			  	 	gest.type = MOUSE_MOV;
 				}
 				else
 				{
 					gest.type = BUTTON_EV;
 				}
 			}
 		}
		else if(state == DRAWR)
		{
			gest.type = RB_RELEASED;
		}
		else
		{
		 	gest.type = BUTTON_EV;
		}

    	return;
    } 

    if( !pp.lb && pp.rb && !pp.mb)
  	{
  		if(pp.delta_x != 0 && pp.delta_y != 0)
  		{
  			if(validMoveR(tolerance))
  			{
  				gest.delta_x += pp.delta_x;
  				gest.delta_y += pp.delta_y;
  				gest.type = MOUSE_MOV;
  			}
  			else
 			{
 			 	gest.type = BUTTON_EV;
 			}
 		
 			return;
 		}

    	gest.type = RB_PRESSED;

    	return;
  	}
 	
 	else
 	{
 		gest.type = BUTTON_EV;
 	}
}


void check_v_line(uint8_t x_len) 
{
	switch (state) 
	{
		case INIT:

			if(gest.type == LB_PRESSED )
			{
				gest.delta_x = 0;
 			    gest.delta_y = 0;
				state = DRAWL;	
			}
			break;

		case DRAWL:

			if(gest.type == MOUSE_MOV)
			{
				state = DRAWL;
			}
		
			else if(gest.type == LB_RELEASED && gest.delta_x >= x_len) 
			{
				state = HOLD;
				break;
			} 

			else
			{
				state = INIT;
			}
			break;

		case HOLD:

			if( gest.type == RB_PRESSED ) 
			{
				state = DRAWR;
				gest.delta_x = 0;
				gest.delta_y = 0;
				break;
			}

			else if(gest.type == MOUSE_MOV)
			{
				state = HOLD;
			}

			else if(gest.type == LB_PRESSED ) 
			{
				state = DRAWL;
				gest.delta_x = 0;
				gest.delta_y = 0;
			}

			else
			{
				state = INIT;
			}
			break;

		case DRAWR:

			if(gest.type == MOUSE_MOV)
			{
				state = DRAWR;
			}
		
			else if(gest.type == RB_RELEASED && gest.delta_x >= x_len) 
			{
				state = FINAL;
				break;
			} 

			else if( gest.type == LB_PRESSED )
			{
				state = DRAWL;
			}

			else
			{
				state = INIT;
			}
			break;

		default:
			break;
	}
}


bool validMoveL(uint8_t tolerance)
{
	if( (pp.delta_x + tolerance) < 0)
		return false;

	if( (pp.delta_y + tolerance) < 0)
		return false;

	return true;
}


bool validMoveR(uint8_t tolerance)
{
	if(pp.delta_x >= -tolerance)
	{
		if(pp.delta_y <= tolerance)
		{
		return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;	
	}
}


bool validMoveC(uint8_t tolerance)
{
 	if( abs(pp.delta_x) > tolerance)
 	{
 		return false;
 	}
 
  	if(abs(pp.delta_y) > tolerance)
  	{
 		return false;
  	}

 	return true;
}
