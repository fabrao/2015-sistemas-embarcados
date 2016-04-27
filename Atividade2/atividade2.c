#include <18F452.h>

#use delay(clock = 20000000)
#fuses HS, NOWDT, NOPROTECT, NOLVP
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#use rtos(timer=0, minor_cycle=100ms)

int8 counter;

#task (rate=4000ms,max=100ms)
void The_first_rtos_task();

#task (rate=2000ms,max=100ms)
void The_second_rtos_task();

#task (rate=1000ms,max=100ms)
void The_third_rtos_task();


void The_first_rtos_task(){
   printf("Task 1\n\r");
   
   if(counter==3){
      printf("Task 3 disable by Task 1\n\r");
      rtos_disable(The_third_rtos_task);
   }
}

void The_second_rtos_task(){
   printf("Task 2\n\r");
   
   if(++counter==10){
         counter=0;
         printf("Task 3 disable by Task 2\n\r");
      rtos_disable(The_third_rtos_task);
   }
}

void The_third_rtos_task(){
   printf("Task 3\n\r");
}

void main(){
   counter=0;
   rtos_run();
}
