#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_



#define IRQKEYBOARD	        1    /**< @brief kbd IRQ line */

#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60


#define RD_CMD_B 0x20
#define WRT_CMD_B 0x60
/* I/O port addresses */

#define ESC_BK 0x81
#define BIT0 BIT(0)

#define TIMER_0			0x40 /**< @brief Timer 0 count register */
#define TIMER_1			0x41 /**< @brief Timer 1 count register */
#define TIMER_2			0x42 /**< @brief Timer 2 count register */

#define TIMER_SQR_WAVE		(BIT(2)|BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN		BIT(2)          /**< @brief Mode 2: rate generator */

/**@}*/

#endif /* _LCOM_I8042_H */