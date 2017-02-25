#include<reg52.h>
#define	uint	unsigned	int
#define	uchar	unsigned	char

sbit	beep=P1^7;
sbit	select=P1^0;
sbit	jia=P1^1;
sbit	jian=P1^2;
sbit	xuanshi=P1^3;

bit		flag=1,leap=0;
char	sec_l=0,sec_r=0,min_l=0,min_r=0,hour_l=0,hour_r=0;
char	year_l=0,year_r=0,month_l=0,month_r=0,day_l=0,day_r=0; 
uint	num=0,sec=0,min=0,hour=0;
uint	day=6,month=12,year=15;
char	count=0;
uchar	code	table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};


void	display();
void	delayms(uchar	x)
{
	uchar	i;
	while(x--)
	{
		for(i=0;i<113;i++);
	}
}


void	timer_0()	interrupt	1
{
	TH0=0x4c; 
	TL0=0x00;
	num++;

	if((year%4==0&&year%100!=0)||(year%400==0))
     	leap=1;		
	  else
	   	leap=0;		 
	if(num==20)			
		{
		 	num=0;
			sec++;	
		}
	if(sec==60)		 
		{
			sec=0;
			min++;
		}
	if(min==60)
		{
			min=0;
			hour++;
		}
	if(hour==24)       
		{
			hour=0;
			min=0;
			sec=0;
			day++;
		}
	if((leap==1&&month==2&&day==30)||(leap==0&&month==2&&day==29))
		{
			day=1;
			month++;
		}
	if(month==1||month==3||month==5||month==7||month==8||month==10||month==12)
		{
			if(day==32)
			{
				day=1;
				month++;
			}
		}
	if(month==4||month==6||month==9||month==11)
		{
			if(day==31)
			{
				day=1;
				month++;
			}
		}
	if(month==13)
		{
			year++;
			month=1;
		}
	sec_r=sec%10;
	sec_l=sec/10;
	min_r=min%10;
	min_l=min/10;
	hour_r=hour%10;
	hour_l=hour/10;

	day_r=day%10;
	day_l=day/10;
	month_r=month%10;
	month_l=month/10;
	year_r=year%10;
	year_l=year/10;
}


void	key_flag()	interrupt	0
{
	flag=!flag;
}
void	control(void)
{
	if(select==0)
	{
			EA=0;
			sec=0;
			while(1)
			{
				display();
				if(jia==0)
				{
					delayms(10);
					EA=1;
					if(jia==0)
					{
						sec=0;
						min++;
						display();
						while(!jia)
						{
							if(min==60)
								min=0;
						}
					}
				}
				if(jian==0)
				{
					delayms(10);
					EA=1;
					if(jian==0)
					{
						sec=0;
						min--;
						display();
						while(!jian)
						{
							if(min==-1)
								min=59;
						}
					}
				} 
			} 
		} 
			if(xuanshi==0)
			{
				EA=0;
				sec=0;
				while(1)
				{
					display();
					if(jia==0)
					{
						delayms(10);
						EA=1;
						if(jia==0)
						{
							sec=0;
							hour++;
							display();
							while(!jia)
							{
								if(hour==24)
									hour=0;
							}
						}
			    	}
					if(jian==0)
					{
						delayms(10);
						EA=1;
						if(jian==0)
						{
							sec=0;
							hour--;
							while(!jian)
							{
								if(hour==-1)
									hour=23;
							}
							display();
						}
		
					}
				} 

			  }	
 }
void	baoshi() 
{
	if(min==0&&sec<10)
	{
		beep=0;
		delayms(10);
		beep=1;
	}
	else
		beep=1;
}

void	naozhong()
{
	if(hour==0&&min==2&&sec<15)	
	{
		beep=0;
		delayms(10);
		beep=1;
	}
	else
		beep=1;
}


void display()
{
	if(flag==1) 
	{
		P2=0x80;
		P0=table[sec_r];
		delayms(5);
		P2=0;
	
	    P2=0x40;
		P0=table[sec_l];
		delayms(5);
		P2=0;
	
	    P2=0x20;
		P0=0xbf;
		delayms(5);
		P2=0;
	
	    P2=0x10;
		P0=table[min_r];
		delayms(5);
		P2=0;
	
	    P2=0x08;
		P0=table[min_l];
		delayms(5);
		P2=0;
	
	    P2=0x04;
		P0=0xbf;
		delayms(5);
		P2=0;
	
		P2=0x02;
		P0=table[hour_r];
		delayms(5);
		P2=0;
	
	    P2=0x01;
		P0=table[hour_l];
		delayms(5);
		P2=0;
	}
	else
		{   
		P2=0x80;
		P0=table[day_r];
		delayms(5);
		P2=0;
	
	    P2=0x40;
		P0=table[day_l];
		delayms(5);
		P2=0;
	
	    P2=0x20;
		P0=0xbf;
		delayms(5);
		P2=0;
	
	    P2=0x10;
		P0=table[month_r];
		delayms(5);
		P2=0;
	
	    P2=0x08;
		P0=table[month_l];
		delayms(5);
		P2=0;
	
	    P2=0x04;
		P0=0xbf;
		delayms(5);
		P2=0;
	
		P2=0x02;
		P0=table[year_r];
		delayms(5);
		P2=0;
	
	    P2=0x01;
		P0=table[year_l];
		delayms(5);
		P2=0;	
		}	
}

void timer0(void)
{
    TMOD = 0x01;
    TH0 = 0x4C;
    TL0 = 0x00;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
	PT0=0;
}


void Int0()
{	
    EA=1;
	EX0=1;
	IT0=1;
	PX0=0;
}
void	main()
{
	timer0();
	Int0();
	while(1)
	{
		display();
		control();
		baoshi();
		naozhong();
	}
}
