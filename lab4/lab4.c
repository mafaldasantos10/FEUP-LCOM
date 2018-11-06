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
	int ipc_status, r, size = 1;
	message msg;
	
	enable_int();

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

              			mouse_ih();

            
            			//printf("status: %x", status);

            			if(error == true)
              				continue;

            			if(size == 1)
            			{
            				uint8_t temp = (uint8_t)status;
            				if((temp<<4)>>7)
            				{
            				array[0] = status;
            				size++;
            			    }
             				
              				continue;
            			} 

            			if(size == 2)
            			{
            				array[1] = status;
              				size++;
              				continue;
            			}

            			array[2] = status;

            			cnt--;
            		packet_create();
            		mouse_print_packet(&pp);

          			}

          			tickdelay(micros_to_ticks(DELAY_US));


          			size = 1;
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

  	disable_int();

	if (mouse_unsubscribe_int() != OK)
  	{
    	return 1;
  	}

  	return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {

//uint8_t irq_set;
int size = 1;
uint32_t array[3];


    while(cnt > 0) 
	{   
		enable_poll();

		if(mouse_poll() == -1)
		{
			continue;
		}

		if(size == 1)
		{
			uint8_t temp = (uint8_t)status;
			if((temp<<4)>>7)
			{
				array[0] = status;
				size++;
			}

			continue;
		} 

		if(size == 2)
		{
			array[1] = status;
			size++;
			continue;
		}

		array[2] = status;

		cnt--;

		packet_create();
		mouse_print_packet(&pp);
		tickdelay(micros_to_ticks(period));


		size = 1;

	}

	disable_poll();
	minix_get_dflt_kbc_cmd_byte();

	return 0;
}

int (mouse_test_async)(uint8_t idle_time) {

  uint8_t irq_set_mouse, irq_set_timer;
  int ipc_status, r, size = 1;
  message msg;

  enable_int();

  if(idle_time<0)
  {
    return -1;
  }

  if (mouse_subscribe_int(&irq_set_mouse) != OK)
  {
    return 1;
  }
  // printf("%d", irq_set);
  if (timer_subscribe_int(&irq_set_timer) != OK)
  {
    return 1;
  }
 
  while(timer_counter < (idle_time * (uint8_t) sys_hz()) )
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
       		 	continue;

       		 if(size == 1)
       		 {
       		 	uint8_t temp = (uint8_t)status;
       		 	if((temp<<4)>>7)
       		 	{
       		 		array[0] = status;
       		 		size++;
       		 	}

       		 	continue;
       		 } 

       		 if(size == 2)
       		 {
       		 	array[1] = status;
       		 	size++;
       		 	continue;
       		 }

       		 array[2] = status;

       		 packet_create();
       		 mouse_print_packet(&pp);

          }

          tickdelay(micros_to_ticks(DELAY_US));
          size = 1;

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

  disable_int();
  if (mouse_unsubscribe_int() != OK)
  {  
    return 1;
  }

  if (timer_unsubscribe_int() != OK)
  {
    return 1;
  }


  return 0;
}


int (mouse_test_gesture)(uint8_t UNUSED(x_len),uint8_t UNUSED(tolerance)) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}
