#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_


#define IRQKEYBOARD 1    /**< @brief kbd IRQ line */

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
#define TO_ERR BIT(6)
#define PAR_ERR BIT(7)

#define DELAY_US 20000 //wait ms

/**@}*/

#endif /* _LCOM_I8042_H */
