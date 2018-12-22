#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define HOOK_ID_TMP 2
#define IRQ_KEYBOARD 1    /**< @brief kbd IRQ line */
#define IRQMOUSE 12

#define STAT_REG 0x64
#define KBC_CMD 0xD4
#define OUT_BUF 0x60
#define KBC_EN BIT(0)

#define RD_CMD_B 0x20
#define WRT_CMD_B 0x60
/* I/O port addresses */

#define ESC_BK 0x81
#define MSB 0xE0
#define BIT(n) (0x01<<(n))
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TO_ERR BIT(6)
#define PAR_ERR BIT(7)

#define DELAY_US 20000 //wait ms

/**@}*/

//MOUSE
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC
#define EN_DATA 0xF4
#define DIS_DATA 0xF5
#define READ_DATA 0xEB

#define STREAM_MODE 0xEA
#define REMOTE_MODE 0xF0

#define DISABLE_MOUSE 0xA7
#define MOUSE_CMD 0x1D

#define RD_CMD_B 0x20
#define WRT_CMD_B 0x60

//PACKETS
#define L_B BIT(0)
#define R_B BIT(1)
#define M_B BIT(2)
#define X_OVF BIT(6)
#define Y_OVF BIT(7)

//BUTTONS
#define W_KEY 0x11
#define A_KEY 0x1E
#define S_KEY 0x1F
#define D_KEY 0x20
#define ENTER_KEY 0x1C

#endif /* _LCOM_I8042_H */
