#include"LPC2103.h"
#include<string.h>
#include<stdio.h>
char TX_Buf[60];
char RX_Buf[45];
char control_data[10];
char Out[15];
char data_len              = 0;
int TX_Len​= 0;
char RX_Len​= 0;
int TX_Max_Len​= 0;
char carr ​= 0;
char k ​= 0;
char  TX_Enable;​
char RX_Enable;​
char Ok;
char Send_Enable;
char check;
char count;
char carr2;
char data_send;
char data_send1;
char message;
char web_error;
char GPRS_Enable;
char Zigbee_RX_Flag;
char data_mode;
char data_mode1=0;
char Tempeature[5];
char Humidity[5];
char ECG_Buf[1000];
char ECG_Buf1[135];
unsigned int count1;
unsigned int timer1_count;
unsigned char start;
unsigned char stop;
unsigned char medicine=0;
/*************** gps***********************/
 char RX_Buf_Zigbee[20];
 char LAMP_CNTL_FLAG,LAMP_CNTL_DATA,LAMP_CNTL_FLAG_RDY;
char Temp_GPS[20];
char Buf[4];
unsigned char RX_Len_GPS​= 0;
// unsigned char RMC_Enable=0;
volatile static  unsigned char Zigbee_Data;
unsigned char RX_Enable_GPS;
unsigned char Msg_End;
unsigned char GGA_Enable;
unsigned char Close;
unsigned char shutdown;
static unsigned char v=0;
unsigned char Serial_Moniter(void);
void server_config(void);
void Modem_Config(void);
void ppp_config(void);
void connect_mode(void);
void TCP_IP(void);
void send_mess(unsigned char );
void send_cmd(void);
void GPRS_Shutdown(void);
void Lcd_Cmd(unsigned char Cmd);
void Lcd_Delay(unsigned int del);
void Delay(void);
void Delay1(void);
char LCD_Buf[16];
 
/*****************************************************************************************/
 void Delay1(void)
 {
 unsigned long i,j;
  for(i=0;i<=5000;i++)
  for(j=0;j<=10000;j++);
  }
/***************************************************************************************/
void delay(void)
{
​unsigned long wait;
​for(wait=0;wait<=250000;wait++);
}
void UART0_ISR_Zigbee(void)__irq
{
   char aa=0;
​aa=U0IIR;
​Zigbee_Data= U0RBR;
​if(U0LSR==0x60)
​ {
       ​        RX_Buf_Zigbee[RX_Len_GPS] = Zigbee_Data;
​Zigbee_RX_Flag=1;
   }
else if(U0LSR==0x20 )
​   {
​if(TX_Buf[TX_Len]!='\0' && TX_Enable==1)
​U0THR=TX_Buf[TX_Len++];
​else
​{
​TX_Len=0;
​TX_Enable=0;
​}
​  }​
VICVectAddr = 0x00000000;
}
/***************************************************************************************/
void UART1(void) __irq
{
​char aa=0;
​aa=​U1IIR;
​if(U1LSR==0x61)
​{
​   //RI = 0;
      Close= U1RBR;
     // if(GPRS_Enable==1)
​  // {
​if(RX_Enable==1 && data_mode==1)
​{
​  ​control_data[data_len++] = U1RBR;
​data_mode1=1;
​}
​else if(Close==0x0D)
       {
         carr++;
       }
​else if(LAMP_CNTL_FLAG_RDY)
       {
         LAMP_CNTL_DATA=Close;
​  LAMP_CNTL_FLAG=1;
​  Close=0;
  }
else if(Close != 0x0D && Close != 0x0A  &&  data_mode==0)
​{
​RX_Buf[RX_Len] = Close;
​RX_Len=RX_Len+1;
           if(message==1)
​     k++;
​
​}​  
       else if( Close==0x0D)
       {
         carr++;
       }
      if(carr==2 && count==0 && data_send==0)  
       {
         RX_Buf[RX_Len] = '\0';
         carr=0;
         RX_Len=0;
         RX_Enable=0;
         check=1;
       }
       else if(carr==4 && count==1)
       {
         RX_Buf[RX_Len] = '\0';
         carr=0;
         count=0;
         RX_Len=0;
         carr2=1;
       }
       else if(carr==3 && data_send==1)
       {
         carr=0;
 data_send=0;
         data_send1=1;
       }
}​
else if(U1LSR==0x20 )
​{
​if(TX_Buf[TX_Len]!='\0' && TX_Enable==1)
​U1THR=TX_Buf[TX_Len++];
​else {
​TX_Len=0;
TX_Enable=0;​
​}​}
VICVectAddr = 0x00000000;
}
/***************************************************************************************/
void UART0_Init(void)
{
   PINSEL0|= 5;
   APBDIV​= 2;
​U0FCR​= 0x07;
​U0LCR​= 0x83;
​U0DLL​= 0xc3;
​U0DLM​= 0x00;
​U0LCR​= 0x03;​
​U0IER​= 0x03;​   // tX intr enable
   VICIntSelect​= 0x00000000;
​VICIntEnable​= 0x00000040;
​VICVectCntl3 ​= 0x00000020|6;  
​VICVectAddr3​= (unsigned int) UART0_ISR_Zigbee;
}
/***************************************************************************************/
void UART1_Init(void)
{
​U1FCR​= 0x07;
​U1LCR​= 0x83;
​U1DLL​= 0xc3;
​U1DLM​= 0x00;
​U1LCR​= 0x03;​
​U1IER​= 0x03;​   // tX intr enable
​PINSEL0|= 0x50000;
​VICIntSelect​= 0x00000000;
​VICIntEnable​= 0x00000080;
​VICVectCntl2 ​= 0x00000020|7;  
​VICVectAddr2​= (unsigned int) UART1;
}
/***************************************************************************************/
void Lcd_Init(void)
{
IODIR=0XFFFFFFFF;​
Lcd_Cmd(0x02);
Lcd_Cmd(0x28);
Lcd_Cmd(0x06);
Lcd_Cmd(0x0C);
Lcd_Cmd(0x01);
Lcd_Cmd(0x80);
}
/***************************************************************************************/
void Lcd_Cmd(unsigned char Cmd)
{
unsigned long cvalue[1];
​cvalue[0]=Cmd>>4;
​cvalue[0]<<=4;
​cvalue[0]=cvalue[0]|0x08;
​cvalue[0]<<=14;
​IOPIN=cvalue[0];
​delay();
​IOCLR=IOPIN ^ 0xFFFDFFFF;
​cvalue[0]=Cmd & 0x0F;
   cvalue[0]<<=4;
​cvalue[0]=cvalue[0]|0x08;
​cvalue[0]<<=14;
​IOPIN=cvalue[0];
​delay();
​IOCLR=IOPIN ^ 0xFFFDFFFF;
}
/*******************************************************************************************/
 
void lcd_data( unsigned char lcddata)
{
unsigned long dvalue[1];
​dvalue[0]=lcddata>>4;
​dvalue[0]<<=4;
​dvalue[0]=dvalue[0]|0x0C;
​dvalue[0]<<=14;
​IOPIN=dvalue[0];
​delay();
​IOCLR=IOPIN ^ 0xFFFDFFFF;
​dvalue[0]=lcddata & 0x0F;
   dvalue[0]<<=4;
​dvalue[0]=dvalue[0]|0x0C;
​dvalue[0]<<=14;
​IOPIN=dvalue[0];
​delay();
​IOCLR=IOPIN ^ 0xFFFDFFFF;
}
/***************************************************************************************/
 
void LCD (unsigned char * dat)
{
 
while(*dat)
{
  lcd_data(*dat);
  dat++;
}
}
/***************************************************************************************//***************************************************************************************/
void Lcd_Delay(unsigned int del)
{
unsigned int i,j;
 for(i=0;i<del;i++)
  for(j=0;j<65535;j++);
}
/*******************************************************************************************/
void Start_TX(void)
{
​TX_Enable = 1;
​TX_Len ​  = 1;
   TX_Max_Len=strlen(TX_Buf);
​U1THR      = TX_Buf[0];
   while(TX_Enable);
}
/*******************************************************************************************/
void Start_TX_Zigbee(void)
{
​TX_Enable = 1;
​TX_Len ​  = 1;
   TX_Max_Len=strlen(TX_Buf);
​U0THR      = TX_Buf[0];
   while(TX_Enable);
}
void SMSC_Number(void)​//To display which network used for connection  
{
​unsigned char i,j=2;
   Out[0]=0x20;
   Out[1]=0x20;
​for(i=0;i<13;i++)
​{
​Out[j++] =  RX_Buf[12+i];
​
​}
​Out[j] = 0;
   Lcd_Cmd(0x01);
   Lcd_Cmd(0x80);
​LCD("    Network     ");
​Lcd_Cmd(0xc0);
   LCD(Out);
}
/*******************************************************************************************/
unsigned char  Serial_Moniter(void)
{
RX_Len=0;
   RX_Enable=1;
   
​while(1)
​{
​   if(check==1)
      {    
​   ​      /*Lcd_Cmd(0x01);
​          Lcd_Cmd(0xc0);
             strcpy(&Temp_GPS[0],&RX_Buf[0]);
​          LCD(&Temp_GPS[0]); */
​          RX_Len=0;
             check=0;
                       
          if(RX_Buf[0]=='E' && RX_Buf[1]=='R')
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
             LCD("TRY 2 CONNECT");
​          Delay();
             return 0;
          }
          else if(RX_Buf[0]=='N' && RX_Buf[1]=='O')
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("NO CARRIER");
             Delay();
             return 0;
          }
          else if(RX_Buf[0]=='+' && RX_Buf[5]=='E')
          {
             RX_Len=0;
             
             Delay();​    
             return 0;
          }
           else if(RX_Buf[0]=='O' && RX_Buf[1]=='K'&& v==0)
          {
             RX_Len=0;
 
          ​  Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("COMMAND ACCEPT");
             Delay();
             return 1;
          }
          else if(RX_Buf[0]=='C' && RX_Buf[1]=='L')
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("   PAGE CLOSED  ");
             Delay();
             return 1;
          }
          else if(RX_Buf[0]=='S' && RX_Buf[1]=='H')  
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("    SHUTDOWN    ");
             Delay();
             return 1;
          }
      }​
​   else if(carr2==1)
      {
           carr2=0;
          if(RX_Buf[2]=='C' && RX_Buf[4]=='N')
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("WEB PAGE CONNECT");
             Delay();
             return 1;
          }
          else if(RX_Buf[2]=='S' && RX_Buf[4]=='A')    // STATE TCP CONNECTING
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("WEB DISCONNECT");
             Lcd_Cmd(0xC0);
​          LCD("  TCP CONNECT  ");
             Delay();
             return 0;
          }
          else if(RX_Buf[0]=='+' && RX_Buf[9]=='0')
          {
             RX_Len=0;
             SMSC_Number();
             Delay();
             return 1;
          }  
          else if(RX_Buf[0]=='+' && RX_Buf[9]=='K'&&v==1)
          {
             RX_Len=0;
             Lcd_Cmd(0x01);
​          Lcd_Cmd(0x80);
​          LCD("NO NETWORK");
             Lcd_Cmd(0xc0);
​          LCD("Searching...");
             Delay();
             return 0;
          }  
      }
      else if(RX_Buf[0]=='>')
      {
           if(k>=2)  
           {
             RX_Len=0;
             RX_Buf[1]='\0';
​  k=0;
             message=0;
             Lcd_Cmd(0x01);
             Lcd_Cmd(0x80);
             LCD(RX_Buf);
​          Lcd_Cmd(0xc0);
​          LCD("TYPE UR DATA");
             Delay();
             return 1;
           }
​}  
     
      else if(strcmp(RX_Buf,"ERROR")==0)
      {
         RX_Len=0;
 RX_Enable=0;
         Lcd_Cmd(0x01);
         Lcd_Cmd(0x80);
         LCD(RX_Buf);
         Delay();
         web_error=1;
         return 1;
      }
​      else if(data_send1==1)
     {
​      RX_Buf[RX_Len]='\0';  
​      RX_Len=0;    
​  RX_Enable=0;
         data_send1=0;        
         Lcd_Cmd(0x01);
         Lcd_Cmd(0x80);
         LCD(RX_Buf);
         Lcd_Cmd(0xc0);
         LCD("DATA SENT");
         Delay();
         return 1;
     }
 }  ​
}
void Delay(void)
{
 unsigned int count=65535;
 while(count--);
 count=65535;
 while(count--);
 count=65535;
 while(count--);
 count=65535;
 while(count--);​}
int main(void)
{
 char LCD_Buf[20];
 char i=0,j=0;
 char RX_Buf[30];
 char Smoke[4];
 unsigned int k;
 char k1[2];
 k1[0]=0x1A;
 k1[1]='\0';
 LAMP_CNTL_FLAG_RDY=0;
 UART0_Init();
 UART1_Init();
 Lcd_Init();
 Lcd_Cmd(0x01);
 Lcd_Cmd(0x80);
 LCD("  Street Light  ");
 Lcd_Cmd(0xC0);
 LCD(" GateWay Device ");
 // U0IER=0X02;
 Modem_Config();
 //Lcd_Cmd(0x01);
 LAMP_CNTL_FLAG_RDY=1;
 //U0IER​= 0x03;
 while(1)
​{
​  if( LAMP_CNTL_FLAG)
​  {
​    LAMP_CNTL_FLAG=0;
​LAMP_CNTL_FLAG_RDY=0;
​if( LAMP_CNTL_DATA==0X4E)
​{
​  LAMP_CNTL_DATA =0;
​  Lcd_Cmd(0x01);
​  Lcd_Cmd(0x80);
​  strcpy(LCD_Buf,"Lamp Node Going");
         LCD(LCD_Buf);
​  Lcd_Cmd(0xC0);
​  strcpy(LCD_Buf,"To Switched ON");
         LCD(LCD_Buf);
​      strcpy(TX_Buf,"N");
         Start_TX_Zigbee();
​}
​else if( LAMP_CNTL_DATA==0X4D)​  //M
​{
​ LAMP_CNTL_DATA=0;
​ Lcd_Cmd(0x01);
​ Lcd_Cmd(0x80);
​ strcpy(LCD_Buf,"Lamp Node Going");
        LCD(LCD_Buf);
​ Lcd_Cmd(0xC0);
​ strcpy(LCD_Buf,"To Switched OFF");
        LCD(LCD_Buf);
​     strcpy(TX_Buf,"M");
        Start_TX_Zigbee();
​}
​LAMP_CNTL_FLAG_RDY=1;
​   }​
​ if(Zigbee_RX_Flag)
​  {
​    //strcpy(RX_Buf,RX_Buf_Zigbee);
​    Zigbee_RX_Flag=0;
​   i=j=0;
​    LAMP_CNTL_FLAG_RDY=0;​
​
​if(RX_Buf_Zigbee[0]=='F')
​{
​     Lcd_Cmd(0x01);
​Lcd_Cmd(0x80);
​strcpy(LCD_Buf,"Value received");
​LCD(LCD_Buf);
​Lcd_Cmd(0xC0);
​strcpy(LCD_Buf,"From Node");
​LCD(LCD_Buf);
​RX_Buf_Zigbee[0]=0;
​Delay();
​Delay();
​Delay();
​Lcd_Cmd(0x01);
​Lcd_Cmd(0x80);
​strcpy(LCD_Buf,"Lamp failure in");
​LCD(LCD_Buf);
​Lcd_Cmd(0xC0);
​strcpy(LCD_Buf,"Lamp Node");
​LCD(LCD_Buf);
​    send_cmd();
​    Delay();
​strcpy(TX_Buf,"F");
​ strcat(TX_Buf,k1);
​Start_TX();
   data_send=1;
       Ok=Serial_Moniter();
​    Ok=0;
​Delay();
​data_mode=0;
​LAMP_CNTL_FLAG_RDY=1;
​Delay();
​Delay();
​Delay();
​Delay();
}
    }
       if(shutdown==1)
​ {
​shutdown=0;
​GPRS_Shutdown();
​while(1);
 ​    }
​Delay();
​Delay();
​}
 }
/*******************************************************************************************/
void Modem_Config(void)
{
   //Delay();
​  Delay1();
​ k=0;
   strcpy(TX_Buf,"AT\r");
    while(TX_Buf[k]!='\0')
    {
       U1THR=TX_Buf[k++];
       while((U1LSR&0X40)!=0X40);
    }
    Delay();
    k=0;
    strcpy(TX_Buf,"ATE0\r");
    while(TX_Buf[k]!='\0')
    {
       U1THR=TX_Buf[k++];
       while((U1LSR&0X40)!=0X40);
    }
​ Delay();
​ k=0;
    carr=check=carr2=0;
    sprintf(TX_Buf,"%s","AT\r");
    Start_TX();
   Ok=Serial_Moniter();
    Ok=0;
    Delay();
aa:
    count=1;
    sprintf(TX_Buf,"%s","AT+COPS?\r");
    Start_TX();
    Ok=Serial_Moniter();
    if(Ok==0) {Delay();goto aa;}
    Ok=0;
bb:
    sprintf(TX_Buf,"%s","AT+CGACT=0,1\r");
    Start_TX();
    Ok=Serial_Moniter();
    Ok=0;
    server_config();
    sprintf(TX_Buf,"%s","AT+CGATT=1\r");
    Start_TX();
    Ok=Serial_Moniter();
    Ok=0;
    sprintf(TX_Buf,"%s","AT+CGACT=1,1\r");
    Start_TX();
    Ok=Serial_Moniter();
    count=0;
    ppp_config();
    connect_mode();
     Lcd_Cmd(0x01);
     Lcd_Cmd(0x80);
     LCD("TCP_IP");
    TCP_IP();
    Ok=Serial_Moniter();
    if(Ok==0)
     {​
​    sprintf(TX_Buf,"%s","AT+CIPCLOSE\r");
       Start_TX();
       Ok=Serial_Moniter();
       Ok=0;
​    Delay();
       sprintf(TX_Buf,"%s","AT+CIPSHUT\r");
       Start_TX();
       Ok=Serial_Moniter();
       Ok=0;
​Delay();
/*******************************************************************************************/
void server_config(void)
{
    TX_Buf[0]='A';
    TX_Buf[1]='T';
    TX_Buf[2]='+';
    TX_Buf[3]='C';
    TX_Buf[4]='G';
    TX_Buf[5]='D';
    TX_Buf[6]='C';
    TX_Buf[7]='O';
    TX_Buf[8]='N';
    TX_Buf[9]='T';
    TX_Buf[10]='=';
    TX_Buf[11]='1';
    TX_Buf[12]=',';
    TX_Buf[13]='"';
    TX_Buf[14]='I';
    TX_Buf[15]='P';
    TX_Buf[16]='"';
    TX_Buf[17]=',';
    TX_Buf[18]='"';
    TX_Buf[19]='w';
    TX_Buf[20]='w';
    TX_Buf[21]='w';
    TX_Buf[22]='"';
    TX_Buf[23]='\r';
    Start_TX();
    Ok=Serial_Moniter();
    Ok=0;
}
/*******************************************************************************************/
void ppp_config(void)
{
    sprintf(TX_Buf,"%s","                                    ");
    sprintf(RX_Buf,"%s","                                    ");
    TX_Buf[0]='A';
    TX_Buf[1]='T';
    TX_Buf[2]='+';
    TX_Buf[3]='C';
    TX_Buf[4]='G';
    TX_Buf[5]='D';
    TX_Buf[6]='A';
    TX_Buf[7]='T';
    TX_Buf[8]='A';
    TX_Buf[9]='=';
    TX_Buf[10]='"';
    TX_Buf[11]='P';
    TX_Buf[12]='P';
    TX_Buf[13]='P';
    TX_Buf[14]='"';
    TX_Buf[15]=',';
    TX_Buf[16]='1';
    TX_Buf[17]='\r';
    Start_TX();
    Ok=Serial_Moniter();
    Ok=0;
}
void connect_mode(void)
{
    TX_Buf[0]='A';
    TX_Buf[1]='T';
    TX_Buf[2]='+';
    TX_Buf[3]='C';
    TX_Buf[4]='I';
    TX_Buf[5]='P';
    TX_Buf[6]='C';
    TX_Buf[7]='S';
    TX_Buf[8]='G';
    TX_Buf[9]='P';
    TX_Buf[10]='=';
    TX_Buf[11]='1';
    TX_Buf[12]=',';
    TX_Buf[13]='"';
    TX_Buf[14]='w';
    TX_Buf[15]='w';
    TX_Buf[16]='w';
    TX_Buf[17]='"';
    TX_Buf[18]='\r';
    Start_TX();
    Ok=Serial_Moniter();
    Ok=0;
}
/*****************************************************************************/
void TCP_IP(void)
{
    sprintf(TX_Buf,"%s","                                          ");
    sprintf(RX_Buf,"%s","                                          ");
    count=1;
    TX_Buf[0]='A';
    TX_Buf[1]='T';
    TX_Buf[2]='+';
    TX_Buf[3]='C';
    TX_Buf[4]='I';
    TX_Buf[5]='P';
    TX_Buf[6]='S';
    TX_Buf[7]='T';
    TX_Buf[8]='A';
    TX_Buf[9]='R';
    TX_Buf[10]='T';
    TX_Buf[11]='=';
    TX_Buf[12]='"';
    TX_Buf[13]='T';
    TX_Buf[14]='C';
    TX_Buf[15]='P';
    TX_Buf[16]='"';
    TX_Buf[17]=',';
    TX_Buf[18]='"';
    TX_Buf[19]='1';
    TX_Buf[20]='2';
    TX_Buf[21]='2';
    TX_Buf[22]='.';
    TX_Buf[23]='1';
    TX_Buf[24]='6';
    TX_Buf[25]='5';
    TX_Buf[26]='.';
    TX_Buf[27]='0';
    TX_Buf[28]='2';
    TX_Buf[29]='0';
    TX_Buf[30]='.';
    TX_Buf[31]='0';
    TX_Buf[32]='5';
    TX_Buf[33]='5';
    TX_Buf[34]='"';​                   TX_Buf[35]=',';
    TX_Buf[36]='3';
    TX_Buf[37]='5';
    TX_Buf[38]='5';
    TX_Buf[39]='7';
    TX_Buf[40]='\r';
    Start_TX();
}
