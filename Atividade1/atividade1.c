#include <18F452.h>

#use delay(clock = 20000000)
#fuses HS, NOWDT, NOPROTECT, NOLVP
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#use rtos(timer=0, minor_cycle=100ms)

int8 counter;

#task (rate=4000ms, max=100ms)
void The_first_rtos_task(){
   printf("Task 1\n\r");
   
   if(++counter==5){
      rtos_terminate();
   }
}


#task (rate=2000ms, max=100ms)
void The_second_rtos_task(){
   printf("Task 2\n\r");
}


#task (rate=1000ms, max=100ms)
void The_third_rtos_task(){
   printf("Task 3\n\r");
}


void main(){
   counter=0;
   rtos_run();
   printf("RTOS has been terminated\n\r");
}


