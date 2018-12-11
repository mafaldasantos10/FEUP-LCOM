#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"
#include "interface.h"
#include "menu.h"

state_t state = START;

uint8_t bit_no_kb, bit_no_timer;

    if (kbd_subscribe_int(&bit_no_kb) != OK)
    {
      return 1;
    }

    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_timer = BIT(bit_no_timer);

    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1;
    bool wait = false;
    message msg;

    while(esc)
    {  
      /* Get a request message. */ 
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
      {
        printf("driver_receive failed with: %d", r); 
        continue;
      }
      if (is_ipc_notify(ipc_status))
      { /* received notification */ 
        switch (_ENDPOINT_P(msg.m_source))
        {
          case HARDWARE: /* hardware interrupt notification */ 
            if (msg.m_notify.interrupts & irq_set_keyboard) 
            { /* subscribed interrupt */ 

              kbc_ih();
            
              if (status == MSB)
              {
                wait = true;
                continue;
              }

              if (wait == true)
              {
                wait = false;
                size = 2;
              }

              if (size == 1)
              {
                byte1[0] = status;
              } 

              if (size == 2)
              {
                byte2[0] = MSB;
                byte2[1] = status;
              }
            }
            break;
          default:
            break; /* no other notifications expected: do nothing */ 
          } 
        } 
        else
        {  /* received a standard message, not a notification */ 
          /* no standard messages expected: do nothing */ 
        }

      size = 1;
    } 

    if (kbd_unsubscribe_int() != OK)
    {  
        return 1;
    }
