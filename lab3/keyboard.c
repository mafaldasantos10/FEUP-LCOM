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

uint8_t (kbd_int_handler)() 
{
	uint32_t stat, status;

	while( 1 ) {

		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */

		if( stat & OBF ) 
		{
			sys_inb(OUT_BUF, &status); /* assuming it returns OK */

			if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
				return (uint8_t) status;
			else
				return -1;
		}

		delay(WAIT_KBC);
	}

	return 0;
}
