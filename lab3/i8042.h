#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

extern uint32_t status; //global variable that contains the scan of the buffer
extern uint32_t stat;
extern uint32_t counter;

#define HOOK_ID_TMP 2
#define IRQKEYBOARD 1    /**< @brief kbd IRQ line */

#ifdef LAB3
	int sys_inb_cnt(port_t port, uint32_t *byte);
	#else
	#define sys_inb_cnt(p,q) sys_inb(p,q) 
	#endif

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60

#define RD_CMD_B 0x20
#define WRT_CMD_B 0x60
/* I/O port addresses */

#define ESC_BK 0x81
#define MSB 0xE0

#define BIT(n) (0x01 << (n))
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TO_ERR BIT(6)
#define PAR_ERR BIT(7)

#define DELAY_US 20000 //wait ms

/**@}*/

#endif /* _LCOM_I8042_H */
