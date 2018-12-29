#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @brief IRQ lines */
#define IRQ_KEYBOARD 1    
#define IRQ_MOUSE 12

/* I/O port addresses */
#define STAT_REG 0x64
#define OUT_BUF 0x60

/* KBC */
#define KBC_EN BIT(0)
#define RD_CMD_B 0x20
#define WRT_CMD_B 0x60
#define KBC_CMD 0xD4

#define MSB 0xE0
#define BIT(n) (0x01<<(n))
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TO_ERR BIT(6)
#define PAR_ERR BIT(7)
#define DELAY_US 20000

/* MOUSE */
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

/* Packets */
#define L_B BIT(0)
#define R_B BIT(1)
#define M_B BIT(2)
#define X_OVF BIT(6)
#define Y_OVF BIT(7)

/* Keyboard Keys */
#define A_KEY 0x1E
#define B_KEY 0x30
#define C_KEY 0x2E
#define D_KEY 0x20
#define E_KEY 0x12
#define F_KEY 0x21
#define G_KEY 0x22
#define H_KEY 0x23
#define I_KEY 0x17
#define J_KEY 0x24
#define K_KEY 0x25
#define L_KEY 0x26
#define M_KEY 0x32
#define N_KEY 0x31
#define O_KEY 0x18
#define P_KEY 0x19
#define Q_KEY 0x10
#define R_KEY 0x13
#define S_KEY 0x1F
#define T_KEY 0x14
#define U_KEY 0x16
#define V_KEY 0x2F
#define W_KEY 0x11
#define X_KEY 0x2D
#define Y_KEY 0x15
#define Z_KEY 0x2C

#define BACKSPACE_KEY 0x0E
#define ENTER_KEY 0x1C
#define ENTER_KEY_BK 0x9C
#define SPACE_KEY 0x39
#define SPACE_KEY_BK 0xB9
#define ESC_BK 0x81
#define P_KEY_BK 0x99

/* Interface */
#define FRAME_RATE 60

/* Score */
#define MAX_PLAYER_SLOTS 7

#endif /* _LCOM_I8042_H */
