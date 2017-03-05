#include <REG51.H>	
unsigned char code LEDDATA[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,
0x82,0xf8,0x80,0x90,0xff,0x8e,0x86};
//数码管显示的代码表，后三个为灭灯、"F"、"E"
unsigned char code LEDBITDATA[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f,}; 
//数码管扫描代码表
unsigned char LEDBuffer[6];			//定义显示缓冲区数组
unsigned char Hour;					//时单元
unsigned char Minute; 				//分单元
unsigned char Second;				//秒单元
unsigned char Hourrom; 				//定时时单元
unsigned char Minuterom;	 		//定时分单元
unsigned char Minutesw;        		//秒表分单元
unsigned char Secondsw;        		//秒表秒单元
unsigned char Centsw;         		//秒表百分之一秒单元
unsigned char Beepflag;	 			//定时响铃标志
unsigned char SETFlag=0;			//模式标志
unsigned char second_tick;	 		//闪动标志
unsigned char Time;					//超时计数
unsigned char ALMFlag=0;			//定时开启标志

sbit ALM_KEY=P3^2;					//显示定时时间按键
sbit SET_KEY=P3^3;			 	 	//闹钟模式键
sbit UP_KEY=P3^4;					//加计数键
sbit DOWN_KEY=P3^5;					//减计数键
sbit Beep=P1^7;						//蜂鸣器接口引脚

void init()
{ 	TMOD=0x01;						//T0初始化方式1,定时
	TH0=(65536-2000)/256;			//TH0,TL0装入定时2mS的初值
	TL0=(65536-2000)%256;
	TR0=1;							//启动T0工作
	ET0=1;							//允许T0溢出中断
	
	TH1=(65536-50000)/256;        
	TL1=(65536-50000)%256; 
	TR1=0;
	ET1=1;
	EA=1;							//CPU开中断
}

void Delay(unsigned int t)			//延时子程序
{ while(t)t--;
}
void key()							//键盘操作子程序
{	unsigned char i;				//缓冲数组位数标志
	char Num;						//临时数字，存储数组合并值
	if(SET_KEY==0)					//判断模式键是否按下
	{	Delay(5000);					//去按键抖动
		if(SET_KEY==0)				//再判断是否真得按下了
		{ 	SETFlag++;				//状态改变
			if(SETFlag==8) SETFlag=0;	//返回正常模式
			if(SETFlag==1) i=4;			//调节读取显示数组的位数
			if(SETFlag==2) i=2;
			if(SETFlag==3) i=0;
			if(SETFlag==4) i=4;
			if(SETFlag==5) i=2;
			if(SETFlag==6) i=0;
			if(SETFlag==7) ;
		}
	while(SET_KEY==0);					//等按键释放
	}

	if((UP_KEY==0)&&(SETFlag!=0))		//判断加计数键是否按下
	{
		Delay(5000);					//去按键抖动
		if(UP_KEY==0)					//再判断是否真得按下了
		{	Num=(LEDBuffer[i+1]+LEDBuffer[i]*10);
			Num++;						//时单元的数值加1
			if((Num==24)&&((SETFlag==3)||(SETFlag==5))) Num=0;	//加到24归0
			if((Num==60)&&((SETFlag==1)||(SETFlag==2)||(SETFlag==4))) Num=0;			//加到60归0
			switch(SETFlag)				//把修改值写回
			{	case 0: ;break;
				case 1: Second=Num;break;
				case 2: Minute=Num;break;
				case 3: Hour=Num;break;
				case 4: Minuterom=Num;break;			
				case 5: Hourrom=Num;break;
				case 6: ALMFlag=!ALMFlag;break;
				case 7: TR1=!TR1;break;						//秒表开始与暂停
			}
		}
	 }

	if((DOWN_KEY==0)&&(SETFlag!=0))	//判断减计数键是否按下
	{
		Delay(5000);				//去按键抖动
		if(DOWN_KEY==0)				//再判断是否真得按下了
		{	Num=(LEDBuffer[i+1]+LEDBuffer[i]*10);
			Num--;					//时单元的数值减1
			if((Num<0)&&((SETFlag==3)||(SETFlag==5))) Num=23;	//到24归0
			if((Num<0)&&((SETFlag==1)||(SETFlag==2)||(SETFlag==4))) Num=59;			//到60归0
			switch(SETFlag)				//把修改值写回
			{	case 0: ;break;
				case 1: Second=Num;break;
				case 2: Minute=Num;break;
				case 3: Hour=Num;break;	
				case 4: Minuterom=Num;break;
				case 5: Hourrom=Num;break;				
				case 6: ALMFlag=!ALMFlag;break;
				case 7: TR1=0;Minutesw=0;Secondsw=0;Centsw=0;break;		//秒表reset			
			}
		}
	}
	if(ALM_KEY==0)
	{	SETFlag=0;						//状态返回
		if(ALMFlag==0)LEDBuffer[0]=11;	//根据闹铃状态显示F或者E
		else LEDBuffer[0]=12;			//将时,分,秒单元内容送入暂存区
		LEDBuffer[1]=10;				//关闭该数码管显示
		LEDBuffer[2]=Hourrom/10;
		LEDBuffer[3]=Hourrom%10;
		LEDBuffer[4]=Minuterom/10;
		LEDBuffer[5]=Minuterom%10;
		
		if(ALMFlag==1)
		{	Beep=1;
			Beepflag=0;
		}
	}
}			 


void display()							//显示暂存区内容对应的代码显示
{ 	unsigned char LEDScanCount,i;		//位选扫描计数器
P0=0xff;								//适应仿真需要
if((UP_KEY==0)||(DOWN_KEY==0)) i=0x00;	//加减键有操作放弃闪烁
 	else i = 0xff*second_tick;			//设定闪烁变量
P2= LEDBITDATA[LEDScanCount];			//送出位选数据
switch(SETFlag)
	{	case 0:	P0=LEDDATA[LEDBuffer[LEDScanCount]];break; 	//送出段选数据 
	 	case 1:	if(LEDScanCount>=4)							//判断出最高两位
					 P0=i|LEDDATA[LEDBuffer[LEDScanCount]];	//使小时闪烁
				else P0=LEDDATA[LEDBuffer[LEDScanCount]];break;	//低位正常显示
	 	case 2:	if((LEDScanCount==2)||(LEDScanCount==3))	 	//判断出中间位
					 P0=i|LEDDATA[LEDBuffer[LEDScanCount]];	//使分钟闪烁
				else P0=LEDDATA[LEDBuffer[LEDScanCount]];break;	//其他位正常显示
		case 3:	if(LEDScanCount<=1)
					 P0=i|LEDDATA[LEDBuffer[LEDScanCount]];
				else P0=LEDDATA[LEDBuffer[LEDScanCount]];break;
		case 4:	if(LEDScanCount>=4)
					 P0=i|LEDDATA[LEDBuffer[LEDScanCount]];
				else P0=LEDDATA[LEDBuffer[LEDScanCount]];break;
	 	case 5:	if((LEDScanCount==2)||(LEDScanCount==3))
					 P0=i|LEDDATA[LEDBuffer[LEDScanCount]];
				else P0=LEDDATA[LEDBuffer[LEDScanCount]];break;
		case 6:	if(LEDScanCount<=1)
					 P0=i|LEDDATA[LEDBuffer[LEDScanCount]];
				else P0=LEDDATA[LEDBuffer[LEDScanCount]];break;
		case 7:	P0=LEDDATA[LEDBuffer[LEDScanCount]];break; 
	}
LEDScanCount++; 						//扫描指针加计数
if(LEDScanCount==6) LEDScanCount=0 ;	//扫描完从头开始;
}


void time0() interrupt 1
{   unsigned int SecondCount;
	unsigned int timercp;				//秒计数器
	TH0=(65536-2000)/256;				//TH0,TL0装入定时2mS的初值
	TL0=(65536-2000)%256;	 
	display();							//调用显示函数
	timercp++;		
	if((SETFlag!=0)&&(SETFlag!=7)) 						//10秒不操作自动返回
		{	if((SET_KEY==0)||(UP_KEY==0)||(DOWN_KEY==0)) Time=0;	//任意键有操作放弃计时
		 	if (Time>=10){SETFlag=0;Time=0;}
		}
	if (timercp == 250)				
		{	timercp= 0; 
			second_tick = !second_tick;		//设定0.5秒闪动一次 
			SecondCount++;
			if(SecondCount==2) 				//1秒的时间
			{	SecondCount=0;				//计数器清零
				Time++;					
	 			Second++;					//时间的秒加1
				if(Second==60)
				{	Second=0;		 		//秒清零
					Minute++;		 		//分进一
					if(Minute==60)
					{	Minute=0;
						Hour++;
						if(Hour==24)Hour=0;	//小时清零
					}
				}
			}
	if(SETFlag<=3)				 			//状态模式小于3是加载时钟时间
			{ LEDBuffer[0]=Hour/10;
				LEDBuffer[1]=Hour%10;
				LEDBuffer[2]=Minute/10;
				LEDBuffer[3]=Minute%10;
				LEDBuffer[4]=Second/10;
				LEDBuffer[5]=Second%10; 
			
			}
			else							 //调整闹钟时间时加载闹钟时间
			{	if(ALMFlag==0)LEDBuffer[0]=11; //显示闹钟激活状态
		 		else LEDBuffer[0]=12;
				LEDBuffer[1]=10;			 //关闭倒数第二位
				LEDBuffer[2]=Hourrom/10;
				LEDBuffer[3]=Hourrom%10;
				LEDBuffer[4]=Minuterom/10;
				LEDBuffer[5]=Minuterom%10;		
			}
			if(SETFlag==7)
			{
				LEDBuffer[0]=Minutesw/10;
				LEDBuffer[1]=Minutesw%10;
				LEDBuffer[2]=Secondsw/10;
				LEDBuffer[3]=Secondsw%10;
				LEDBuffer[4]=Centsw/10;
				LEDBuffer[5]=Centsw%10;
			}
	}
	
}
void time1() interrupt 3      //秒表中断程序
{
	unsigned int Count;
	TH1=(65536-50000)/256;        
	TL1=(65536-50000)%256; 
	display();	
	Count++;
	if(Count==2)
	{
	Count=0;
	Centsw++;
	if(Centsw==100)
	{
		Centsw=0;
		Secondsw++;
		if(Secondsw==60)
		{
			Secondsw=0;
			Minutesw++;
			if(Minutesw==60)
			{
				Minutesw=0;
			}
		}
	}
	

	}
	LEDBuffer[0]=Minutesw/10;
	LEDBuffer[1]=Minutesw%10;
	LEDBuffer[2]=Secondsw/10;
	LEDBuffer[3]=Secondsw%10;
	LEDBuffer[4]=Centsw/10;
	LEDBuffer[5]=Centsw%10;
	
}
	



void main()
{	init();								//初始化
	while(1)
{		key();							//调用键盘
		if(ALMFlag==1)
		{	if(Minute!=Minuterom) Beepflag=1;	//定时和现在不同，关闭蜂鸣器
			if((Hour==Hourrom)&&(Minute==Minuterom)&&(Beepflag==1)) Beep=0;
					//时分相同并闹铃打开就响铃
		}
} 
}
