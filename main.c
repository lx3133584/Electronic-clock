#include<reg52.h>
#define	uint	unsigned	int
#define	uchar	unsigned	char

sbit	beep=P1^7;	//������λ����
sbit	xuanze=P1^0;
sbit	jia=P1^1;  //ʱ����һ��
sbit	jian=P1^2;	//ʱ����һ�� 
sbit	xuanshi=P1^3;

bit	flag=1,leap=0;	//flag�ǿ���ʱ�����������յ���ʾ��leap�ж��Ƿ�λ����
char	secshi=0,secge=0,minshi=0,minge=0,hourshi=0,hourge=0;    //����ʱ�����ĸ�λ��ʮλ
char	yearshi=0,yearge=0,monthshi=0,monthge=0,dayshi=0,dayge=0; //���������յĸ���ʮλ
uint	num=0,sec=0,min=0,hour=0;
uint	day=6,month=12,year=15;
char	count=0;	//����ѡ�����Ĵ���
uchar	code	table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
//P0���Ƕ�ѡ��P2����λѡ
//��ʱ����
void	display();
void	delayms(uchar	x)
{
	uchar	i;
	while(x--)
	{
		for(i=0;i<113;i++);
	}
}

//��ʱ���ж�0��
void	timer_0()	interrupt	1
{
	TH0=0x4c; //��װ��ֵ,һ��40ms����ֵλ28673��7001��
	TL0=0x00;
	num++;

	if((year%4==0&&year%100!=0)||(year%400==0))   //�ж� �Ƿ�Ϊ����
     	leap=1;		 //����
	  else
	   	leap=0;		 //ƽ��
	if(num==20)			//��ʱ�պ�Ϊ1s
		{
		 	num=0;
			sec++;	
		}
	if(sec==60)		  //��Ϊ60�ּ�1
		{
			sec=0;
			min++;
		}
	if(min==60)
		{
			min=0;
			hour++;
		}
	if(hour==24)           //����24Сʱ������һ
		{
			hour=0;
			min=0;
			sec=0;
			day++;
		}
	if((leap==1&&month==2&&day==30)||(leap==0&&month==2&&day==29))
		{
			day=1;
			month++;   //2��28��29ʱ�����Զ���һ
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
	secge=sec%10;
	secshi=sec/10;
	minge=min%10;
	minshi=min/10;
	hourge=hour%10;
	hourshi=hour/10;

	dayge=day%10;
	dayshi=day/10;
	monthge=month%10;
	monthshi=month/10;
	yearge=year%10;
	yearshi=year/10;
}

//�ⲿ�ж�0���ж�flag��������ʾ�����գ�δ������ʾʱ����
void	key_flag()	interrupt	0
{
	flag=!flag;
}
void	control(void)
{
	if(xuanze==0)
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
				}  //if
			} //while(1)
		} //if
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
				} //while(1)

			  }	 //if
 }
void	baoshi()             //����ʱ����ʱ10s
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
	if(hour==0&&min==2&&sec<15)	   //2����ʱ��15s
	{
		beep=0;
		delayms(10);
		beep=1;
	}
	else
		beep=1;
}

//��ʾ������
void display()
{
	if(flag==1) 			//ʱ����ʾ
	{
		P2=0x80;			   //������ʾ
		P0=table[secge];
		delayms(5);
		P2=0;
	
	    P2=0x40;
		P0=table[secshi];
		delayms(5);
		P2=0;
	
	    P2=0x20;			  //��������ʾ
		P0=0xbf;
		delayms(5);
		P2=0;
	
	    P2=0x10;			  //������ʾ
		P0=table[minge];
		delayms(5);
		P2=0;
	
	    P2=0x08;
		P0=table[minshi];
		delayms(5);
		P2=0;
	
	    P2=0x04;			 //��������ʾ
		P0=0xbf;
		delayms(5);
		P2=0;
	
		P2=0x02;			  //ʱ����ʾ
		P0=table[hourge];
		delayms(5);
		P2=0;
	
	    P2=0x01;
		P0=table[hourshi];
		delayms(5);
		P2=0;
	}
	else					 //��������ʾ
		{   
		P2=0x80;			   //����ʾ
		P0=table[dayge];
		delayms(5);
		P2=0;
	
	    P2=0x40;
		P0=table[dayshi];
		delayms(5);
		P2=0;
	
	    P2=0x20;			  //��������ʾ
		P0=0xbf;
		delayms(5);
		P2=0;
	
	    P2=0x10;			  //����ʾ
		P0=table[monthge];
		delayms(5);
		P2=0;
	
	    P2=0x08;
		P0=table[monthshi];
		delayms(5);
		P2=0;
	
	    P2=0x04;			 //��������ʾ
		P0=0xbf;
		delayms(5);
		P2=0;
	
		P2=0x02;			  //����ʾ
		P0=table[yearge];
		delayms(5);
		P2=0;
	
	    P2=0x01;
		P0=table[yearshi];
		delayms(5);
		P2=0;	
		}	
}
//��ʱ��0��ʼ��
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

//�ⲿ�ж�0��ʼ��
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
