#include<P18F4550.h>
#include <delays.h>

// Definições
#define PIO		PORTBbits.RB0	// Pino IO do RTC
#define PSCLK	PORTBbits.RB1	// Pino SCLK do RTC
#define PREST	PORTAbits.RA4	// Pino REST do RTC
#define SW10	PORTCbits.RC0	// Push-buttom SW10 (TMR1)
#define SW11	PORTCbits.RC1	// Push-buttom SW11 (CH0)

unsigned char year,month,day,hour,minute,second;

// Protótipos de Funções
void send_char(unsigned char sentchar);
unsigned char get_char();
void atraso(unsigned char time);
void set_time();
void set_wp_off();
void set_wp_on();
void read_time();
void set_ch_on();
void PrintBCD2 (char data);




// Função envia caracter para o ht1380
void sent_char(unsigned char sentchar)
{
	unsigned char n;
	TRISBbits.TRISB0=0;
	for(n=0;n<8;n++)
	{
		PSCLK=0;
		if ((sentchar&1)!=0)
		{
			PIO=1;
		}
		else
		{
			PIO=0;
		}
		sentchar=sentchar>>1;
		atraso(2);
		PSCLK=1;
		atraso(2);
	}
}

// Função recebe caracter do ht1380
unsigned char get_char()
{
	unsigned char n;
	unsigned char getchar=0;
	unsigned char temp=1;
	TRISBbits.TRISB0=1;
	for(n=0;n<8;n++)
	{
		PSCLK=0;
		atraso(1);
		if(PORTBbits.RB0==1)
		{
			getchar=getchar|temp;
		}
		temp=temp<<1;
		atraso(2);
		PSCLK=1;
		atraso(2);
	}
	return (getchar);
}


void read_time() {
  unsigned char temp;
    PREST=1;
	PSCLK=0;
    sent_char(0xbf);      //10111111b
    second=get_char();
    minute=get_char();
    hour=  get_char();
    day=   get_char();
    month= get_char();
    temp=  get_char();   /* week day */
    year=  get_char();
    temp=  get_char();
    PSCLK=0;
    atraso(2);
    PREST=0;
    atraso(10);
 }

// Função para gerar atraso
void atraso(unsigned char time)
{
  unsigned int i=0;
  for(i=0;i<=time;i++){}
}

// Função set_time
void set_time()
 {
   set_wp_off();
   PREST=1;
   atraso(4);
   second=second & 0x7f;
   sent_char(0xbe);      //10111110b
   sent_char(second);
   sent_char(minute);
   sent_char(hour);
   sent_char(day);
   sent_char(month);
   sent_char(1);         /* week  day */
   sent_char(year);
   sent_char(0);
   PSCLK=0;
   atraso(4);
   PREST=0;
   atraso(4);
   set_wp_on();
 }

// Desabilita o write protect (WP=0)
void set_wp_off()
{
    PREST=1;
    atraso(4);
    sent_char(0x8e);     //10001110b
    sent_char(0);        //00000000b
    PSCLK=0;
    atraso(4);
    PREST=0;
    atraso(4);
}


void PrintBCD2 (char data){

	char in,valor,save;

	save = data;
	valor = data >>= 4;
	in = (valor & 0x0F);
	putcXLCD (in + 0x30);
	in = (save & 0x0F);
	putcXLCD (in + 0x30);
}

// Habilita o write protect (WP)
void set_wp_on()
{
  PREST=1;
  atraso(4);
  sent_char(0x8e);       //10001110b
  sent_char(0x80);       //10000000b
  PSCLK=0;
  atraso(4);
  PREST=0;
  atraso(4);
}

// Habilita o oscilador (CH=0)
void set_ch_on()
{
  unsigned int i;
  PREST=1;
  atraso(4);
  sent_char(0x80);       //10001110b
  sent_char(0);       	//00000000b
  PSCLK=0;
  atraso(4);
  PREST=0;
  atraso(4);
  // atraso de 3s para estabilizar o oscilador do RTC
  for(i=0;i<=5;i++)
  {
	Delay10KTCYx(250); // Atraso de 0.5s
  }
}
