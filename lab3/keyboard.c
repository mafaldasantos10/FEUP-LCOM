  #include <lcom/lcf.h>
  #include <lcom/lab3.h>

  #include <stdint.h>

  #include "i8042.h"

int hook_id = 0;


int (kbd_subscribe_int)(uint8_t *bit_no)
{
	//checks if the sys call was valid
	if (sys_irqsetpolicy(IRQKEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK)
	{
    return 1;
	}

  	*bit_no = BIT(hook_id);

	return 0;
 }

int (kbd_unsubscribe_int)() 
{
  	//checks if the sys call was valid
 	if (sys_irqrmpolicy(&hook_id) != OK)
  	{
    return 1;
  	}

  	return 0;
}

int (kbd_int_handler)() 
{
	int sys_inb(OUT_BUF, &status); 

	status = (uint8_t) status;

	return status;
}


bool (kbd_int_size)() 
{
	uint32_t status;
	int sys_inb(OUT_BUF, &status); 
	status = (uint16_t) status;

	return status;
}