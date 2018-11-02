#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"

static int hook_id = 2;

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
		sys_inb_cnt(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if( stat & OBF ) 
		{
			sys_inb_cnt(OUT_BUF, &status); /* assuming it returns OK */

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
		}

		i++;
	}

	error = true;
	return;
}