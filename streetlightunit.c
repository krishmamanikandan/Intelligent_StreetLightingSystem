#include<16F877A.H>
#device adc=10;                             //Set ADC to 10 bit
#fuses HS,NOWDT,NOPROTECT,NOBROWNOUT,NOPUT  //Configuration Fuses
#use delay(clock=20000000)                   //20Mhz Clock for wait functions
#use rs232(baud=9600,xmit=PIN_c6,rcv=PIN_C7,PARITY=N,BITS=8)//set up RS-232
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#byte trisc=0x87
#byte ccpr1l=0x15
#byte ccp1con=0x17
#byte t2con=0x12
#byte tmr2=0x11
#byte pr2=0x92
#byte portc=0x07
unsigned char RX_RDY;
unsigned char  LAMP_OFF;
unsigned char  LAMP_ON;
unsigned char   First_attemp;
//unsigned char  Temp_Flag;
unsigned char Second_attemp;
unsigned char RX_Data;
void LCD_Cmd(unsigned char command);
void dim();
int f=0;
  double Vp,Ip,Pp;
  //long g;
  //unsigned int Unit=0;
  long V,I;
  double MeanX;
  float VV,II,Pv;
     unsigned char lcd_str[16];
#bit RCIF=0X0C.5
#int_RDA
void RDA_isr()
{
if(RCIF==1)
{
 RX_Data=getc();
 RX_RDY=1;
 RCIF=0;
/* if(RX_Data=='M')
else if(RX_Data=='N')
 output_low(PIN_D5);output_low(PIN_D6);output_low(PIN_D7);*/
}
RCIF=0;
}
/****************************************************************/
void lcd_init(void)
{
//delay_ms(1000);
LCD_Cmd(0x02);
//delay_ms(3);
LCD_Cmd(0x28);
//delay_ms(3);
LCD_Cmd(0x06);
//delay_ms(3);
LCD_Cmd(0x0c);
//delay_ms(3);
LCD_Cmd(0x01);
}
/***************************************************************/
void LCD_Cmd(unsigned char command)
{
  unsigned char cvalue;
  cvalue=command>>4;
  output_b(cvalue);  //PORTB=cvalue;
  output_low(PIN_B4); //RS=0;
  output_high(PIN_B5);//EN=1;
  delay_ms(30);
  output_low(PIN_B5);//EN=0;
  cvalue=command & 0x0F;
  output_b(cvalue);  //PORTB=cvalue;
  output_low(PIN_B4);//RS=0;
  output_high(PIN_B5);//EN=1;
  delay_ms(30);
  output_low(PIN_B5);//EN=0;  
}
/****************************************************************/
void lcd_data(unsigned char data)
{
  unsigned char dvalue;
  dvalue=data>>4;
  output_b(dvalue);  //PORTB=dvalue;
  output_high(PIN_B4);//RS=1;
  output_high(PIN_B5);//EN=1;
  delay_ms(30);
  output_low(PIN_B5);//EN=0;
  dvalue=data & 0x0F;
  output_b(dvalue);   //PORTB=dvalue;
  output_high(PIN_B4);//RS=1;
  output_high(PIN_B5);//EN=1;
  delay_ms(30);//Delay();
  output_low(PIN_B5);//EN=0
  output_low(PIN_B4);//RS=0;
}
/****************************************************************/
void LCD(unsigned char *dat,unsigned char loc)
{
LCD_Cmd(loc) ;
while(*dat)
{   lcd_data(*dat);
        dat++; }
}
void Power()
{
    setup_adc_ports( ALL_ANALOG );
    setup_adc( ADC_CLOCK_DIV_32 );
    set_adc_channel(0);                        //set ADC channel to port 0 (pin 2, AN0).
    delay_ms(50);
    MeanX=0;f=0,I=0;
    LCD_Cmd(0x01);
    for(f=0;f<25;f++)
       I += read_ADC();
    MeanX=I/25;
    Ip=I*0.0048875855327468230694037145650049;
    Ip=Ip/2;
    II=Ip;
    //delay_ms(200);
    sprintf(lcd_str,"I%1.3f",II);
   // printf(lcd_str,"I%1.3f",II);
     LCD(lcd_str,0x88);
    set_adc_channel(1);                           //set ADC channel to port 0 (pin 2, AN0).
    delay_ms(10);
    V = read_ADC();
    Vp=V*0.22482893450635386119257086999022;
    VV=Vp;
    sprintf(lcd_str,"V%3.2f",VV);
    //printf(lcd_str,"V%3.2f",VV);
    ///delay_ms(200);
    LCD(lcd_str,0x80);
    Pp=Vp*Ip;
    Pv=Pp;
     sprintf(lcd_str,"P%3.2f",Pp);
    //printf(lcd_str,"P%3.2f",Pp);
    LCD(lcd_str,0xC0);    }
 
void main() {
  unsigned int16 T;
  unsigned int16 L;
  dim();
  ccpr1l=0xff;
  setup_adc_ports( ALL_ANALOG );
  setup_adc(ADC_CLOCK_DIV_32 );
  set_tris_d(0X00);
  set_tris_b(0X00);
  output_d(0x0f);
  enable_interrupts(GLOBAL);
  enable_interrupts(int_rda);
  delay_ms(1200);
  lcd_init();
  delay_ms(100);
  LCD_Cmd(0x01);
  strcpy(lcd_str,"Street Light ");
  LCD(lcd_str,0x80);
  LCD_Cmd(0xC0);
  strcpy(lcd_str," Control");
  LCD(lcd_str,0xc0);
  RX_RDY=0;
  while (true)
   {
  if(RX_RDY)
         {
          RX_RDY=0;
          do
         {
          if( RX_Data=='N')
            {
             // LCD(lcd_str,0x01);
            LCD_Cmd(0x01);
            RX_Data=0;
            LAMP_ON=1;
            LAMP_OFF=0;
            ccpr1l=0x00;
            LCD_Cmd(0x01);
            strcpy(lcd_str,"Lamp Switched ON ");
            LCD(lcd_str,0x80);
            strcpy(lcd_str,"From Server");
            LCD(lcd_str,0xc0);
            delay_ms(200);
            LCD_Cmd(0x01);
           }
          else if(RX_Data=='M')
           {
           LCD_Cmd(0x01);
           RX_Data=0;
           ccpr1l=0xff;
           strcpy(lcd_str,"Lamp SwitchedOFF ");
           LCD(lcd_str,0x80);
           strcpy(lcd_str,"From Server ");
           LCD(lcd_str,0xc0);
           LAMP_OFF=1;
           LAMP_ON=0;
           delay_ms(200);
           LCD_Cmd(0x01);
          }
          if(LAMP_OFF)
          {
             setup_adc_ports( ALL_ANALOG );
             setup_adc( ADC_CLOCK_DIV_32 );
             delay_ms(100);
             set_adc_channel(2);                        //set ADC channel to port 0 (pin 2, AN0).
             //delay_ms(30);
             L = read_ADC();
             LCD_Cmd(0x01);
             sprintf(lcd_str,"%lu",L);
         
            LCD(lcd_str,0xc0);
            //delay_ms(50);
            //if( L>600)
             if(L>980)
               {
                delay_ms(100);
                ccpr1l=0x00;
                LCD_Cmd(0x01);
                strcpy(lcd_str,"Lamp Switched ON");
                LCD(lcd_str,0x80);
                strcpy(lcd_str,"Due to Low Light");
                LCD(lcd_str,0xc0);
                delay_ms(100);
              }
            else
               {
                 delay_ms(100);
                 ccpr1l=0xff;
                }
           }
           if(LAMP_ON)
           {  
             Power();
             setup_adc_ports( ALL_ANALOG );
             setup_adc( ADC_CLOCK_DIV_32 );
             set_adc_channel(0);                        //set ADC channel to port 0 (pin 2, AN0).
             I = read_ADC();
            if(I<5)
            {
              LCD_Cmd(0x01);
             strcpy(lcd_str,"Lamp:FAIL");
             LCD(lcd_str,0xC0);
             if(First_attemp)
             {
              First_attemp=0;
              //Send_Zigbee('F');  //LAMP FAILURE
              putc('F');
              delay_ms(10);
             }
             else
             {
              Second_attemp=Second_attemp+1;
              if(Second_attemp>=13)
              {
               Second_attemp=0;
               //Send_Zigbee('F');
               putc('F');
               delay_ms(10);
              }
             }
            }
            if(input(PIN_D0))
             {
             ccpr1l=0x00;
               //delay_ms(500);
            //output_low(pin_c2);
           //delay_ms(500);
             //D0=0;D1=0;D2=0;
             //output_low(PIN_D5);output_low(PIN_D6);output_low(PIN_D7);
             LCD_Cmd(0x01);
             strcpy(lcd_str,"Mov:YES");
             LCD(lcd_str,0xc9);
             }
            else
             {
              //D0=0;D1=1;D2=1;
               //output_low(PIN_D5);output_high(PIN_D6);output_low(PIN_D7);
               //LCD_Cmd(0x89);
               //dim();
                ccpr1l=253;
                LCD_Cmd(0x01);
               strcpy(lcd_str,"Mov:NO ");
               LCD(lcd_str,0xc9);
            }
           }
       } while(!RX_RDY);
    }
   delay_ms(100);
  }
}
void dim()
{
trisc=0x80;//o/p configuration of tris for pwm
t2con=0x06;//timer 2 control reg
ccp1con=0x5c;//pwm mode
//tmr2=0x00;//timer 2
pr2=0xff;//period reg for timer2
}
 
 
 
 
