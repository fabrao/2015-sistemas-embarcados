#include <18F452.h>

#use delay(clock = 20000000)
#fuses HS, NOWDT, NOPROTECT, NOLVP
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#use rtos(timer=0, minor_cycle=100ms)

#task (rate=4000ms, max=100ms, queue=2)
void The_first_rtos_task();

#task (rate=20000ms, max=100ms, queue=2)
void The_second_rtos_task();


void The_first_rtos_task(){
  int count=0;
  
  while(TRUE){
   count++;
   rtos_msg_send(The_second_rtos_task, count);
   rtos_yield();
  }
}


void The_second_rtos_task(){

   if(rtos_msg_poll()){
      printf("count is: %i\n\r", rtos_msg_read());
   }
}

void main(){
   rtos_run();
}
