#include<P18F4550.h> 
#include <delays.h>
#include <i2c.h>
#include <adc.h>
#include "xlcd_mod.c"
#include "RTC.h"
#include<usart.h>


// Configurações 
#pragma config   PLLDIV = 5           		// PLL para 20MHz  
#pragma config  CPUDIV = OSC1_PLL2   		// PLL desligado 
#pragma config  FOSC = HS               	// Fosc = 20MHz     Tcy = 200ns 
#pragma config  WDT = OFF               	// Watchdog desativado 
#pragma config  PBADEN = OFF           		// Pinos do PORTB começam como digitais 
#pragma config  LVP = OFF               	// Desabilita gravação em baixa tensão 
#pragma config  DEBUG = ON              	// habilita debug 
  
// Definições
#define PIO		PORTBbits.RB0	// Pino IO do RTC
#define PSCLK	PORTBbits.RB1	// Pino SCLK do RTC
#define PREST	PORTAbits.RA4	// Pino REST do RTC
#define SW10	PORTCbits.RC0	// Push-buttom SW10 (TMR1)
#define SW11	PORTCbits.RC1	// Push-buttom SW11 (CH0)


// *** Protótipos
void LowPriorityISR(void);
void HighPriorityISR(void);
unsigned char USART_LOG=0;


//CABEÇALHO DA FUNÇÃO P/ SALVAR OS DADOS NA EEPROM
void salvarDados (unsigned char temp1, unsigned char temp2,  unsigned char year, unsigned char month, 
unsigned char day, unsigned char hour, unsigned char minute, unsigned char second);

//CABEÇALHO DA FUNÇÃO P/ LER OS DADOS DA EEPROM
void leituraDados();
void dadosToSerial();
void PrintUSARTBCD (char data);


// *** Configuracao das RTI

// Interrupção de baixa prioridade
#pragma code low_vector=0x18
void interrupt_at_low_vector(void){
	_asm GOTO LowPriorityISR _endasm
}
#pragma code 

// Interrupção de baixa prioridade
#pragma code high_vector=0x08
void interrupt_at_high_vector(void){
	_asm GOTO HighPriorityISR _endasm
}
#pragma code 

// Defina no parâmetro SAVE os registros a serem salvos antes da ISR
#pragma interruptlow LowPriorityISR 
#pragma interrupt HighPriorityISR 

// ISR de baixa prioridade
void LowPriorityISR (void){

}

// ISR de alta prioridade OU se não houver prioridade. 
void HighPriorityISR(void){
  	if(PIR1bits.RCIF){
    	PIR1bits.RCIF=0;
    	if(getcUSART()=='L'){ USART_LOG=1; dadosToSerial();}
		if(getcUSART()=='l'){ USART_LOG=1; dadosToSerial();}
	}
}


//Variáveis Globais
unsigned int addressPointer = 0x00;
unsigned long short  Temperatura;
char Temp[] = {"000,0"};
char Tempx[] = {"000,0"};
int result=0;
unsigned char TTemp[] = {"00"};
unsigned char dados[] = {0,0,0,0,0,0,0,0};
unsigned char tempt1, tempt2;
int contador=0;

// FUNÇÃO PRINCIPAL
void main(void){
	TRISCbits.TRISC0=1; // Pino SW10 entrada
	TRISCbits.TRISC1=1; // Pino SW11 entrada
	TRISBbits.TRISB1=0; // Pino SCLK (RTC) saída
	TRISAbits.TRISA4=0; // Pino PREST (RTC) saída
	TRISD=0x00;
	TRISE = 0x00;
	PREST=0;
	PSCLK=0;
	
	set_wp_off();
	set_ch_on();

	//SETA DATA NAS VARIAVEIS
	year=0x12;
	month=0x10;
	day=0x31;
	hour=0x15;
	minute=0x59;
	second=0x50; 
	set_time();  


	// Configura e inicializa o conversor A/D
	OpenADC( 	ADC_FOSC_32 		& 
				ADC_RIGHT_JUST 		& 
				ADC_20_TAD,	ADC_CH0	& 
				ADC_REF_VDD_VSS 	& 
				ADC_INT_OFF, ADC_1ANA );

	// Inicialização da porta serial
	OpenUSART(USART_TX_INT_OFF  &
	          USART_RX_INT_ON   &
	          USART_ASYNCH_MODE &
	          USART_EIGHT_BIT   &
	          USART_CONT_RX     &
	          USART_BRGH_HIGH,129 );
	
	//Configurações transmissor serial
	TRISCbits.TRISC6=1;  //Habilita o pino TX como entrada
	TRISCbits.TRISC7=1;  //Habilita o pino RX como entrada
	
	//Configurações da interrupção para o receptor serial
	PIR1bits.RCIF=0;
	IPR1bits.RCIP=1;
	//PIE1bits.RCIE=1;
	
	//Configurações interrupção geral
	RCONbits.IPEN=1;	
	INTCONbits.GIEL=1;	
	INTCONbits.GIEH=1;
		
	
	// Iniciação do LCD
	OpenXLCD(FOUR_BIT & LINES_5X7);
	WriteCmdXLCD(0x01); //limpeza do display com retorno cursor 
	WriteCmdXLCD(0x0C); // cursor ativo (sem cursor)
	Delay10KTCYx(10);
	 


	while(1){

		// Loop infinito
			TRISCbits.TRISC0=1; // Pino SW10 entrada
			TRISCbits.TRISC1=1; // Pino SW11 entrada
			TRISBbits.TRISB1=0; // Pino SCLK (RTC) saída
			TRISAbits.TRISA4=0; // Pino PREST (RTC) saída
//*************************
			TRISD=0x00;
			TRISE = 0x00;
//*************************	

		//LENDO TEMPERATURA
			Delay10TCYx(250);
			ConvertADC();
			while (BusyADC());
			result = ReadADC();

			itoa(result,TTemp);//Transforma Int em Char e separa em 2


			read_time();

		//IMPRIME NO DISPLAY - HORA
		    WriteCmdXLCD(0x80); // posição do cursor em 80
    		putrsXLCD("HORA: ");
			PrintBCD2(hour); //hora
			putcXLCD(':');
			PrintBCD2(minute); //segundos
			putcXLCD(':');
			PrintBCD2(second); //segundos		
			atraso(2);


		    WriteCmdXLCD(0xC0); // posição do cursor em C0
  		//IMPRIME NO DISPLAY - DIA/MES/ANO
			PrintBCD2(day); //dia
			putcXLCD('/');
			PrintBCD2(month); //mês
			putcXLCD('/');
			PrintBCD2(year); //ano		
			

		//CONVERTENDO TEMPERATURA PARA IMPRIMIR NO LCD 
			//Converter o inteiro (0-1023) Tensao e depois para ºC
			Temperatura = ((long int) 5000*result)/1023;
			
			//Converter para o LCD
			Temp[0] = 0x30 +(Temperatura/1000);
			Temperatura=Temperatura%1000;
			Temp[1] = 0x30 +(Temperatura/100);
			Temperatura=Temperatura%100;
			Temp[2] = 0x30 + (Temperatura/10);
			Temp[4] = 0x30 + (Temperatura%10);
			atraso(2);

		//IMPRIMINDO TEMPERATURA NO DISPLAY
			putcXLCD('T');
			putcXLCD('=');
			putsXLCD(Temp);
			putcXLCD(0b11011111);
			putcXLCD('C');

			Delay10KTCYx(250);	
			WriteCmdXLCD(0x01); // limpando o lcd e retornando o cursor
	
		
		//SE O BOTÃO RC0 FOR PRECIONADO
			if(PORTCbits.RC0==0){
				salvarDados(TTemp[1], TTemp[2], year, month, day, hour, minute, second);
				contador++;
				//leituraDados(dados);
			}

			if(PORTCbits.RC1==0){
			
				//leituraDados();
				
			}
				
	}
}


//FUNÇÃO PARA SALVAR TEMPERATURA E DATA NA EEPROM
void salvarDados (unsigned char temp1, unsigned char temp2, unsigned char year, unsigned char month, 
unsigned char day, unsigned char hour, unsigned char minute, unsigned char second){
	

	int i=0;
	unsigned char vetorDados[8];
	
	OpenI2C(MASTER, SLEW_OFF);
	SSPADD = 11;

	vetorDados[0] = temp1;
	vetorDados[1] = temp2;
	vetorDados[2] = year;
	vetorDados[3] = month; 
	vetorDados[4] = day; 
	vetorDados[5] = hour;
	vetorDados[6] = minute;
	vetorDados[7] = second;
	tempt1 = vetorDados[0];
	tempt2 = vetorDados[1];
	
	
	if(addressPointer>247){
		addressPointer = 0x00;
	}
		
	for(i =0; i<8; i++){
		EEByteWrite(0xA0, addressPointer, vetorDados[i]);
		Delay10KTCYx(10);
		addressPointer++;
	}

	CloseI2C();
	TRISBbits.TRISB1=0; // Pino SCLK (RTC) saída
	TRISAbits.TRISA4=0; // Pino PREST (RTC) saída
}


//FUNÇÃO PARA LER TEMPERATURA E DATA NA EEPROM
void leituraDados(){
	
	int i=0;

	OpenI2C(MASTER, SLEW_ON);
	SSPADD = 11;

	for(i=0; i<8; i++){		
		dados[i] = EERandomRead(0xA0, addressPointer); 
		Delay10KTCYx(10);
		addressPointer--;
	}

	CloseI2C();
	Delay10KTCYx(200); 
	TRISBbits.TRISB1=0; // Pino SCLK (RTC) saída
	TRISAbits.TRISA4=0; // Pino PREST (RTC) saída
}

void dadosToSerial(){
		int i=0;
		int j=0;

		if(USART_LOG){
      		USART_LOG=0;

			// Transmissão
			for(i=0; i<contador; i++){
				leituraDados();

	    		putrsUSART("TEMP , HH:MM:SS - DD/MM/AA\r\n");
	         	while(BusyUSART()); // aguarda o caractere ser enviado 	
				PrintUSARTBCD(dados[-1]);
				PrintUSARTBCD(dados[0]);
				putrsUSART("C "); while(BusyUSART());	
				putrsUSART(", "); while(BusyUSART());
				PrintUSARTBCD(dados[3]);
				putrsUSART(":"); while(BusyUSART());
				PrintUSARTBCD(dados[2]);
				putrsUSART(":"); while(BusyUSART());
				PrintUSARTBCD(dados[1]);
				putrsUSART(" - "); while(BusyUSART());
				PrintUSARTBCD(dados[4]);
				putrsUSART("/"); while(BusyUSART());
				PrintUSARTBCD(dados[5]);
				putrsUSART("/"); while(BusyUSART());
				PrintUSARTBCD(dados[6]);
	            putrsUSART("\n\r"); // envia pela serial	
				while(BusyUSART()); // aguarda o caractere ser enviado 	
	            putrsUSART("\n\r"); // envia pela serial
	         	while(BusyUSART()); // aguarda o caractere ser enviado 			
			}
		}
		contador=0;
		addressPointer = 0x00;
}

void PrintUSARTBCD (char data){

	char in,valor,save;

	save = data;
	valor = data >>= 4;
	in = (valor & 0x0F);
		WriteUSART(in + 0x30); while(BusyUSART()); 
	in = (save & 0x0F);
		WriteUSART(in + 0x30); while(BusyUSART()); 
}