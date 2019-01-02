#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "keyboard.h"

//VARIABLE INITIALIZATION
static int hook_id = 1;
uint32_t status = 0;
bool error_kbd = false;


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
