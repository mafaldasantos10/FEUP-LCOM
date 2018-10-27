#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"

//global variables
static int hook_id = 2;
uint32_t status = 0, stat = 0, counter = 0;
bool error = false;


int (kbd_subscribe_int)(uint8_t *bit_no)
{
	*bit_no = hook_id;

	//printf("dd %d\n", *bit_no );

	//checks if the sys call was valid
	if (sys_irqsetpolicy(IRQKEYBOARD, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id) != OK)
	{
		return 1;
	}

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


void (kbc_ih)(void) 
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


int sys_inb_cnt(port_t port, uint32_t *byte)
{
	counter++;
	return sys_inb(port, byte);
}


int kbd_poll_cmd()
{
	uint32_t kbdcmd;

	sys_outb(KBC_CMD_REG, RD_CMD_B);
	sys_inb(OUT_BUF, &kbdcmd);

	kbdcmd = kbdcmd | KBC_EN;

	sys_outb(KBC_CMD_REG, WRT_CMD_B);
	sys_outb(OUT_BUF, kbdcmd);

	return 0;
}


int kbd_scan_poll()
{
	int i = 0;
	uint32_t statpoll = 0;

	while( i < 5 )
	{
		sys_inb_cnt(STAT_REG, &statpoll); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if( statpoll & OBF ) 
		{
			sys_inb_cnt(OUT_BUF, &status); /* assuming it returns OK */

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
