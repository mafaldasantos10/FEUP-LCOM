// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

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

              			kbc_ih();
            
            			//printf("status: %x", status);

            			if(error == true)
              				continue;

            			if(size == 1)
            			{
              				size++;
              				continue;
            			} 

            			if(size == 2)
            			{
              				size++;
              				continue;
            			}
            		//falta guardar numa packet[]
            		//falta a funcao para a imprimir, o stor n deu ainda
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

	if (kbd_unsubscribe_int() != OK)
  	{
    	return 1;
  	}

  	return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)() {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}
