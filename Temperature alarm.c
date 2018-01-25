#include <AT89X52.h> 
#define uint unsigned int
#define uchar unsigned char   	                                       
#define SET  P3_1    			                       														
#define DEC  P3_2    										
#define ADD  P3_3    										
#define BEEP P3_6    												
#define ALAM P1_2																				
#define DQ   P3_7     																																								
bit shanshuo_st;    																				
bit beep_st;     															
sbit DIAN = P0^5;        																									
uchar x=0;      																	
signed char m;     																			
uchar n;      																
uchar set_st=0;     																						
signed char shangxian=38;  																											
signed char xiaxian=5;   																																			

uchar code  LEDData[]={0x5F,0x44,0x9D,0xD5,0xC6,0xD3,0xDB,0x47,0xDF,0xD7,0xCF,0xDA,0x9B,0xDC,0x9B,0x8B};

void Delay_DS18B20(int num)
{
  while(num--) ;
}

void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         
  Delay_DS18B20(8);    
  DQ = 0;         							
  Delay_DS18B20(80);   
  DQ = 1;         						
  Delay_DS18B20(14);
  x = DQ;           															
  Delay_DS18B20(20);
}

unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     												
    dat>>=1;
    DQ = 1;     										
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}

void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}

unsigned int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;
  float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  														
  WriteOneChar(0x44);  									
  Init_DS18B20();
  WriteOneChar(0xCC);  															
  WriteOneChar(0xBE);  											
  a=ReadOneChar();     									
  b=ReadOneChar();    												
  t=b;				   																		
  t<<=8;			   																		
  t=t|a;			   																
  tt=t*0.0625;		   				
  t= tt*10+0.5;     										
  return(t);		   								
}

void Delay(uint num)
{
 while( --num );
}

void InitTimer(void)
{
	TMOD=0x1;
	TH0=0x3c;
	TL0=0xb0;     																						
}

void check_wendu(void)
{
	uint a,b,c;
	c=ReadTemperature();  																	
	a=c/100;     																				
	b=c/10-a*10;    																		
	m=c/10;      																	
	n=c-a*100-b*10;    														
	if(m<0){m=0;n=0;}   																		
	if(m>99){m=99;n=9;}   																						 
}

void Disp_init(void)    
{
	P0 = ~0x80;      																			
	P2 = 0x7F;
	Delay(200);
	P2 = 0xDF;
	Delay(200);   
	P2 = 0xF7;
	Delay(200);
	P2 = 0xFD;
	Delay(200);
	P2 = 0xFF;         										
}

void Disp_Temperature(void)     								
{
	P0 = ~0x98;      											
	P2 = 0x7F;
	Delay(100);
	P2=0xff;
	P0=~LEDData[n];    																
	P2 = 0xDF;
	Delay(100);
	P2=0xff;
	P0 =~LEDData[m%10];    												
	DIAN = 0;         															
	P2 = 0xF7;
	Delay(100);
	P2=0xff;
	P0 =~LEDData[m/10];    												
	P2 = 0xFD;
	Delay(100);
	P2 = 0xff;         															
}

void Disp_alarm(uchar baojing)
{
	P0 =~0x98;      																			
	P2 = 0x7F;
	Delay(100);
	P2=0xff;
	P0 =~LEDData[baojing%10]; 														
	P2 = 0xDF;
	Delay(100);
	P2=0xff;
	P0 =~LEDData[baojing/10]; 																		
	P2 = 0xF7;
	Delay(100);
	P2=0xff;
	if(set_st==1)P0 =~0xCE;
	else if(set_st==2)P0 =~0x1A; 														
	P2 = 0xFD;
	Delay(100);
	P2 = 0xff;         																			
}

void Alarm()
{
	if(x>=10){beep_st=~beep_st;x=0;}
	if((m>=shangxian&&beep_st==1)||(m<xiaxian&&beep_st==1))
	{
		BEEP=0;
		ALAM=0;
	}
	else
	{
		BEEP=1;
		ALAM=1;
	}
}

void main(void)
{
    uint z;
 InitTimer();    																						
 EA=1;      																					
 TR0=1;
 ET0=1;      																				
 IT0=1;        
 IT1=1;
 check_wendu();
 check_wendu();
 for(z=0;z<300;z++)
 {
  Disp_init();        
  }
 while(1)
  {
  if(SET==0)
  {
   Delay(2000);
   do{}while(SET==0);
   set_st++;x=0;shanshuo_st=1;
   if(set_st>2)set_st=0;
  }
  if(set_st==0)
  {
   EX0=0;    																						
   EX1=0;    																						
    check_wendu();
      Disp_Temperature();
   Alarm();   																	
  }
  else if(set_st==1)
  {
   BEEP=1;    																					
   ALAM=1;
   EX0=1;    																			
   EX1=1;    																						
   if(x>=10){shanshuo_st=~shanshuo_st;x=0;}
   if(shanshuo_st) {Disp_alarm(shangxian);}
  }
  else if(set_st==2)
  {
   BEEP=1;    															
   ALAM=1;
   EX0=1;    																					
   EX1=1;    																		
   if(x>=10){shanshuo_st=~shanshuo_st;x=0;}
   if(shanshuo_st) {Disp_alarm(xiaxian);}
  }
    }
}

void timer0(void) interrupt 1
{
 TH0=0x3c;
 TL0=0xb0;
 x++;
}

void int0(void) interrupt 0
{
 
 EX0=0;      																				
 if(DEC==0&&set_st==1)
 {
  	do{
		Disp_alarm(shangxian);
	}
 	while(DEC==0);
  	shangxian--;
  	if(shangxian<xiaxian)shangxian=xiaxian;
 }
 else if(DEC==0&&set_st==2)
 {
  	do{
		Disp_alarm(xiaxian);
	}
 	while(DEC==0);
 	xiaxian--;
  	if(xiaxian<0)xiaxian=0;
 }
}

void int1(void) interrupt 2
{
 EX1=0;      																			
 if(ADD==0&&set_st==1)
 {
 	do{
		Disp_alarm(shangxian);
	}
 	while(ADD==0);
  	shangxian++;
  	if(shangxian>99)shangxian=99;
 }
 else if(ADD==0&&set_st==2)
 {
  	do{
		Disp_alarm(xiaxian);
	}
 	while(ADD==0);
  	xiaxian++;
  	if(xiaxian>shangxian)xiaxian=shangxian;
 }  
}