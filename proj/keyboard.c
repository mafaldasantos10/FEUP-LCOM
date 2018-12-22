#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "keyboard.h"

//VARIABLE INITIALIZATION
static int hook_id = 1;
uint32_t status = 0;
bool error = false;


//FUNCTIONS
//////////////////////////////////////////////////////////////////

int (kbd_subscribe_int)(uint8_t *bit_no)
{
	*bit_no = hook_id;
	//printf("dd %d\n", *bit_no );

	//checks if the sys call was valid
	if (sys_irqsetpolicy(IRQ_KEYBOARD, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

int (kbd_unsubscribe_int)() 
{
	//checks if the sys call was valid
	if (sys_irqrmpolicy(&hook_id) != OK)
	{
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////

void (kbc_ih)(void) 
{
	int i = 0;
	uint32_t stat;

	while( i < 5 ) 
	{
		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if( stat & OBF ) 
		{
			sys_inb(OUT_BUF, &status); /* assuming it returns OK */

			if ( (stat & (PAR_ERR | TO_ERR)) == 0 )
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
