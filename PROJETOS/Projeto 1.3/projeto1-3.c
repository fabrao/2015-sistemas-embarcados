#include <18F4550.h>
#device adc=10

#fuses NOWDT, NOPROTECT, DEBUG, NOLVP, HS, PLL5, CPUDIV1, USBDIV, NOPBADEN
#use delay(clock = 20000000)
#use rs232(baud=2400, xmit=PIN_C6, rcv=PIN_C7)
#use rtos(timer=0, minor_cycle=100ms)

#define LED_ON   PIN_D7

int8 sem;
unsigned int16 valor_ad;
unsigned int32 Temp;

#task (rate=200ms, max=10ms)
void The_first_rtos_task();
#task (rate=2000ms,max=40ms)
void The_second_rtos_task();
#task (rate=2000ms,max=40ms)
void The_third_rtos_task();

void The_first_rtos_task(){
   output_toggle(LED_ON);
}
void The_second_rtos_task(){
   rtos_wait(sem);
   valor_ad = read_adc();
   Temp = (unsigned int32)valor_ad*500;
   Temp /= 1023;
   rtos_signal(sem);
}
void The_third_rtos_task(){
   rtos_wait(sem);
   printf("Temperatura medida = %2.2lu Celsius \n\r", Temp);
   rtos_signal(sem);
}

void main(){
   set_tris_d(0x00); //Define portas D como saídas
   set_tris_a(0xFF); //Define portas A como entrada
   setup_adc_ports(ALL_ANALOG);  //Define todas as portas A como analógicas
   setup_adc(ADC_CLOCK_DIV_32);  //TAD= Fosc/32 -> Fosc=20mhz
   set_adc_channel(0);  //Seleciona canal AN0
   
   sem=1;
   rtos_run();
}


