#pragma once

#define COM1_IRQ 4

#define RD_ENABLE BIT(0)
#define RLS_ENABLE BIT(2)
#define IER 0x3F9

#define SP_DELAY    10000 

#define RBR               0x3F8 
#define THR               0x3F8 
#define IER               0x3F9 
#define IIR               0x3FA
#define LSR               0x3FD 

#define RECEIVER_DATA    BIT(0) 
#define OVERRUN_ERROR    BIT(1)

#define PARITY_ERROR     BIT(2) 
#define FRAMING_ERROR    BIT(3) 
#define THR_EMPTY          BIT(5) 
 
#define THR_ENABLE       BIT(1)

#define RLS             (BIT(1)|BIT(2))

#define RD               BIT(2) 

#define IIR_THR          BIT(1)

int serialPort_subscribe_int(uint8_t *bit_no);
int serialPort_unsubscribe_int(); 
int get_RBR(uint32_t *byte);
int write_to_THR(uint32_t byte);
int clear_RBR();
uint32_t serialPort_handler();
