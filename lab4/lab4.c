#include <lcom/lcf.h>
#include "PS2mouse.h"
#include "i8042.h"

#include <lcom/timer.h>
#include <stdint.h>
#include <stdio.h>

//VARIABLES
uint32_t array[3];

// Any header files included below this line should have been created by you
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) 
{
  //VARIABLES
	uint8_t irq_set;
	int ipc_status, r, s = 1;
	message msg;

  if(cnt < 1)
  {
    return 1;
  }
	
	if(mouse_write_int() != OK)
  {
    return 1;
  }
  
 	if (mouse_subscribe_int(&irq_set) != OK)
  {
  	return 1;
  }

  while(cnt > 0 ) 
  { /* You may want to use a different condition */ 
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
          if (msg.m_notify.interrupts & BIT(irq_set)) 
          { /* subscribed interrupt */

            mouse_ih();

            if(error == true)
            {
              continue;
            }

            if(s == 1)
            {
            	if(status & BIT(3))
            	{
            		array[0] = status;
            		s++;
            	}
              continue;
            } 

            if(s == 2)
            {
            	array[1] = status;
              s++;
              continue;
            }

            if(s == 3)
            {
              array[2] = status;
            	packet_create();
            	mouse_print_packet(&pp);

              cnt--;
              s = 1;
            }
          }
          break;
        default: 
          break; /* no other notifications expected: do nothing */ 
      } 
    } 
    else 
    { 	/* received a standard message, not a notification */ 
      	/* no standard messages expected: do nothing */ 
    }
  }

	if (mouse_unsubscribe_int() != OK)
  {
    return 1;
  }

  if(disable_int() != OK)
  {
    return 1;
  }

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) 
{
  //VARIABLES
  int s = 1;

  if(period < 0 || cnt < 1)
  {
    return 1;
  }

  while(cnt > 0)
	{    
    if(set_mouse(READ_DATA) != OK)
    {
      return 1;
    }

    for(unsigned int i = 0; i < 3;)
    {
      mouse_ih();

      if(error == true)
      {
        continue;
      }

      if(s == 1)
      {
        uint8_t temp = (uint8_t)status;

        if(temp & BIT(3))
        {
          array[0] = status;
          s++;
          i++;
        }
        continue;
      } 

      if(s == 2)
      {
        array[1] = status;
        s++;
        i++;
        continue;
      }

      array[2] = status;
      i++;
      s = 1;
    }

    packet_create();
    mouse_print_packet(&pp);
    tickdelay(micros_to_ticks(period * 1000));

    cnt--;
  }

  if(disable_poll() != OK)
  {
    return 1;
  }

  if(mouse_poll_cmd() != OK)
  {
    return 1;
  }

	return 0;
}


int (mouse_test_async)(uint8_t idle_time) 
{
  //VARIABLES
  uint8_t irq_set_mouse, irq_set_timer;
  int ipc_status, r, s = 1;
  message msg;

  if(idle_time < 0)
  {
    return 1;
  }

  if(mouse_write_int() != OK)
  {
    return 1;
  }

  if (mouse_subscribe_int(&irq_set_mouse) != OK)
  {
    return 1;
  }

  if (timer_subscribe_int(&irq_set_timer) != OK)
  {
    return 1;
  }
 
  while(timer_counter < (idle_time * (uint8_t) sys_hz()))
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
          if (msg.m_notify.interrupts & BIT(irq_set_timer)) 
          {
              timer_int_handler();
              
          }
          if (msg.m_notify.interrupts & BIT(irq_set_mouse)) 
          { /* subscribed interrupt */ 

           timer_counter = 0; //resets the timer if another interrupt is needed
           
       	  	mouse_ih();

            if(error == true)
            {
              continue;
            }

            if(s == 1)
            {
              if(status & BIT(3))
              {
                array[0] = status;
                s++;
              }
              continue;
            } 

            if(s == 2)
            {
              array[1] = status;
              s++;
              continue;
            }

            if(s == 3)
            {
              array[2] = status;

              packet_create();
              mouse_print_packet(&pp);

              s = 1;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */ 
      } 
    } 
    else 
    { /* received a standard message, not a notification */ 
      /* no standard messages expected: do nothing */ 
    }
  }

  if (mouse_unsubscribe_int() != OK)
  {  
    return 1;
  }

  if (timer_unsubscribe_int() != OK)
  {
    return 1;
  }

  if(disable_int() != OK)
  {
    return 1;
  }

  return 0;
}


int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance)
{
  //VARIABLES
  uint8_t irq_set;
  int ipc_status, r, s = 1;
  message msg;

  if(x_len < 0 || tolerance < 0)
  {
    return 1;
  }
  
  if(mouse_write_int() != OK)
  {
    return 1;
  }
  
  if (mouse_subscribe_int(&irq_set) != OK)
  {
    return 1;
  }
 
  while(state != FINAL) 
  { /* You may want to use a different condition */ 
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
          if (msg.m_notify.interrupts & BIT(irq_set)) 
          { /* subscribed interrupt */

            mouse_ih();

            if(error == true)
            {
              continue;
            }

            if(s == 1)
            {
              if(status & BIT(3))
              {
                array[0] = status;
                s++;
              }
              continue;
            } 

            if(s == 2)
            {
              array[1] = status;
              s++;
              continue;
            }

            if(s == 3)
            {
              array[2] = status;
              packet_create();
              mouse_print_packet(&pp);
              create_enum(tolerance);
              check_v_line(x_len);

              s = 1;
            }
          }
          break;
        default: 
          break; /* no other notifications expected: do nothing */ 
      }
    }
    else 
    {   /* received a standard message, not a notification */ 
        /* no standard messages expected: do nothing */ 
    }
  }

  if (mouse_unsubscribe_int() != OK)
  {
    return 1;
  }

  if(disable_int() != OK)
  {
    return 1;
  }

  return 0;
}
