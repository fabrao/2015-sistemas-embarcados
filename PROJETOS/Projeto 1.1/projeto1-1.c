#include <18F4550.h>

#use delay(clock = 20000000)
#fuses HS, NOWDT, NOPROTECT, NOLVP
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#use rtos(timer=0, minor_cycle=10ms)

#define LED1 PIN_D0
#define LED2 PIN_D1
#define LED3 PIN_D2
#define LED4 PIN_D3
#define LED5 PIN_D4
#define LED6 PIN_D5
#define LED7 PIN_D6
#define LED8 PIN_D7

#task (rate=250ms,max=10ms)
void The_first_rtos_task();

#task (rate=500ms,max=10ms)
void The_second_rtos_task();

#task (rate=1000ms,max=10ms)
void The_third_rtos_task();

#task (rate=2000ms,max=10ms)
void The_fourth_rtos_task();


void The_first_rtos_task(){
   output_toggle(LED1);
}

void The_second_rtos_task(){
   output_toggle(LED2);
}

void The_third_rtos_task(){
   output_toggle(LED3);
}

void The_fourth_rtos_task(){
   output_toggle(LED4);
}

void main(){
   set_tris_d(0x00);
   output_high(LED5);
   output_high(LED6);
   output_high(LED7);
   output_high(LED8);;
   rtos_run();
}
