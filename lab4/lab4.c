// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "PS2mouse.h"
#include "i8042.h"

#include <lcom/timer.h>
#include <stdint.h>
#include <stdio.h>

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


int (mouse_test_packet)(uint32_t cnt) {

	uint8_t irq_set;
	int ipc_status, r, s = 1;
	message msg;
	
	mouse_write_int();
  
 	if (mouse_subscribe_int(&irq_set) != OK)
  {
  	return 1;
  }
  	// printf("%d", irq_set);
 
  	while(cnt > 0 ) 
  	{   /* You may want to use a different condition */ 
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

                //printf(". ");
              	mouse_ih();
            
            		//printf("status: %x", status);

            		if(error == true)
                {
                  //printf("$");
              		continue;
                }

            		if(s == 1)
            		{
            			//uint8_t temp = (uint8_t)status;
            			if(status & BIT(3))
            			{
            				array[0] = status;
            				s++;
                    //printf(",");
            			}
              		continue;
            		} 

            		if(s == 2)
            		{
            			array[1] = status;
              		s++;
                  //printf("!");
              		continue;
            		}

                if(s == 3)
                {
                  //printf("?");
                  array[2] = status;
            		  cnt--;

            		  packet_create();
            		  mouse_print_packet(&pp);
                  s = 1;
                }
          		}
          		//tickdelay(micros_to_ticks(DELAY_US));
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

  disable_int();

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) 
{
  int size = 1;
  //mouse_poll_cmd(0);
  //enable_poll();

  while(cnt > 0)
	{    
    int_mouse(READ_DATA);
    //printf("read_data %x\n", READ_DATA);

    for(unsigned int i = 0; i < 3;)
    {
      //printf("i %x\n", i);
      //printf("size %x\n", size);
      mouse_ih();
      //printf("status_main %x\n", status);

      if(error == true)
        continue;

      if(size == 1)
      {
        uint8_t temp = (uint8_t)status;

        if(temp & BIT(3))
        {
          array[0] = status;
          //printf("status1 %x\n", status);
          size++;
          //printf("array1 %x\n", array[i]);
          i++;
        }
        continue;
      } 

      if(size == 2)
      {
        array[1] = status;
        //printf("status2 %x\n", status);
        size++;
        //printf("array2 %x\n", array[i]);
        i++;
        continue;
      }

      array[2] = status;
      //printf("status3 %x\n", status);
      //printf("array3 %x\n", array[i]);
      i++;
      size = 1;
    }

    packet_create();
    mouse_print_packet(&pp);
    tickdelay(micros_to_ticks(period*1000));

    cnt--;
  }

  disable_poll();

  if(mouse_poll_cmd(1) != OK)
    return 1;

	return 0;
}

int (mouse_test_async)(uint8_t idle_time) {

  uint8_t irq_set_mouse, irq_set_timer;
  int ipc_status, r, s = 1;
  message msg;

  if(idle_time < 0)
  {
    return -1;
  }

  mouse_write_int();

  if (mouse_subscribe_int(&irq_set_mouse) != OK)
  {
    return -1;
  }
  // printf("%d", irq_set);
  if (timer_subscribe_int(&irq_set_timer) != OK)
  {
    return -1;
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
            //printf("status: %x", status);

            if(error == true)
            {
              //printf("$");
              continue;
            }

            if(s == 1)
            {
              //uint8_t temp = (uint8_t)status;
              if(status & BIT(3))
              {
                array[0] = status;
                s++;
                //printf(",");
              }
              continue;
            } 

            if(s == 2)
            {
              array[1] = status;
              s++;
              //printf("!");
              continue;
            }

            if(s == 3)
            {
              //printf("?");
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

    //tickdelay(micros_to_ticks(DELAY_US));
  }

  if (mouse_unsubscribe_int() != OK)
  {  
    return 1;
  }

  if (timer_unsubscribe_int() != OK)
  {
    return 1;
  }

  disable_int();

  return 0;
}


int (mouse_test_gesture)(uint8_t UNUSED(x_len), uint8_t (tolerance)) 
{
  uint8_t irq_set;
  int ipc_status, r, s = 1;
  message msg;

  struct mouse_ev gest;
  gest.delta_x = &pp.bytes[1];
  gest.delta_y = &pp.bytes[2];
  
  mouse_write_int();
  
  if (mouse_subscribe_int(&irq_set) != OK)
  {
    return 1;
  }
    // printf("%d", irq_set);
 
    while(1) 
    {   /* You may want to use a different condition */ 
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

                //printf(". ");
                mouse_ih();
                //printf("status: %x", status);

                if(error == true)
                {
                  //printf("$");
                  continue;
                }

                if(s == 1)
                {
                  //uint8_t temp = (uint8_t)status;
                  if(status & BIT(3))
                  {
                    array[0] = status;
                    s++;
                    //printf(",");
                  }
                  continue;
                } 

                if(s == 2)
                {
                  array[1] = status;
                  s++;
                  //printf("!");
                  continue;
                }

                if(s == 3)
                {
                  //printf("?");
                  array[2] = status;

                  packet_create();
                  mouse_print_packet(&pp);

                  if( pp.lb )
                    gest.type = LB_PRESSED;

                  //if(validMoveL(tolerance))
                    check_v_line(&gest);

                  s = 1;
                }
              }
              //tickdelay(micros_to_ticks(DELAY_US));
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

  disable_int();

  return 0;
}
