# one
#include<reg52.h>
#define uchar unsigned char
#define uint unsigned char
uint N=1000;
sbit ML1=P2^0;//电机
sbit ML2=P2^1;
sbit MR1=P2^2;
sbit MR2=P2^3;
sbit beep=P2^4;//蜂鸣器
uchar i,j;
//传感器
//指示灯接口定义
sbit led1=P0^0;
sbit led2=P0^1;
sbit led3=P0^2;
sbit led4=P0^3;
sbit led5=P0^4;
//红外地址接口定义
sbit A0=P1^2;
sbit A1=P1^3;
sbit A2=P1^4;
//红外传感器接收信号口定义（接收到的信号值为0）
sbit irR1=P2^5;
sbit irR2=P2^6;
sbit irR3=P2^7;
sbit irR4=P3^6;
sbit irR5=P3^7;
bit irL=0,irR=0,irC=0,irLU=0,irRU=0;//定义红外传感器检测状态全局位变量
uchar intCountL,intCountR,CountL,CountR;
uchar Direction=0;
uchar X=0;
uchar Y=0;//绝对方向
uint pathx=0;
uint pathy=0;
uint now=0;
uint all=0;
uchar pdata StackX[64];
uchar pdata StackY[64];//岔路信息
uchar pdata map[8][8];//挡板信息
uchar pdata best[8][8];
uchar stack=0;
uint num;
uint pathnum=0;

void maze();
void delay(uint z);
void stopR();
void stopL();
void turnL();
void turnR();
void turn180();
void goStraight();
void return_Stack();
void push_Stack();
uint Judge();
void Back();
void coordinate(uchar direction);
void initStack()//初始化栈
{
	for(i=0;i<20;i++)
	{
	   StackX[i]=9;
	   StackY[i]=9;
	}
}
void initbest()// 初始化等高表
{
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			best[i][j]=100;
		}
	}
}
void path()//建立等高表
{
	initStack();
	initbest();
	best[0][0] = 1;
	StackX[0] = 0 ;
	StackY[0] = 0 ;
	all++;
	while(now != all)
	{
		pathx = StackX[now];
		pathy = StackY[now];
		if ((map[pathx][pathy]&0x04)== 0x00&&(best[pathx + 1][pathy]) == 0xff)//右
		{
			best[pathx+1][pathy] = best[pathx][pathy] + 1;
			StackX[all] = pathx+1;//存储坐标
			StackY[all] = pathy;
			all++;
		}
		if ((map[pathx][pathy] & 0x08 )== 0x00&&(best[pathx][pathy + 1] )== 0xff)//上
		{
			best[pathx][pathy+1] = best[pathx][pathy] + 1;
			StackX[all] = pathx;
			StackY[all] = pathy + 1;
			all++;	
		}
		if ((map[pathx][pathy] &0x04) == 0x00&&(best[pathx - 1][pathy] )== 0xff)//左
		{
			best[pathx - 1][pathy] = best[pathx][pathy] + 1;
			StackX[all] = pathx - 1;
			StackY[all] = pathy;
			all++;
		}
		if ((map[pathx][pathy]&0x02) == 0x00&&(best[pathx][pathy-1]) == 0xff)//下
		{
			best[pathx][pathy - 1] = best[pathx][pathy] + 1;
			StackX[all] = pathx;
			StackY[all] = pathy - 1;
			all++;
		}
		now++;
	}
}
void bestPath()//生成最短路径
{
	if(X==0&&Y==0)
	{
		turn180();
		path();//建立等高表
		initStack();
		pathx = pathy = 7;
		i = best[7][7];
		while( i >= 1)
		{
			StackX[i] = pathx;
			StackY[i] = pathy;
			i--;
			if (best[pathx][pathy - 1] == i)
			{
				pathy--;
			}
			else if (best[pathx-1][pathy ] == i)
			{
				pathx--;
			}
			else if (best[pathx+1][pathy] == i)
			{
				pathx++;
			}
			else if (best[pathx][pathy +1] == i)
			{
				pathy++;
			}
		}
		for(i=0;i<3;i++)
		{
			beep=0;
			delay(2000);
			beep=1;
			delay(2000);
		}
		//---------------------------------------
		pathx = pathy = 0;
		pathnum = 1;//步数
		Direction=0;//方向
		while (pathx != 7 || pathy != 7)
		{
			pathnum++;
			switch (Direction)
			{
				case 0: {
					if (StackX[pathnum] == pathx + 1)
					{
						pathx += 1;
						//turnR go
						turnR();
						delay(500);
						Direction=(Direction+1)%4;
						goStraight();
						delay(500);
					}
					if (StackX[pathnum] == pathx - 1)
					{
						pathx -= 1;
						//turnL go
						turnL();
						delay(500);
						Direction=(Direction+3)%4;
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy + 1)
					{
						pathy += 1;
						//gostraight
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy - 1)
					{
						pathy -= 1;
						//turnBack  go
						turn180();
						delay(500);
						Direction=(Direction+2)%4;
						goStraight();
						delay(500);
					}
				}break;
				case 1: {
					if (StackX[pathnum] == pathx + 1)
					{
						pathx += 1;
						//turn
						goStraight();
						delay(500);
					}
					if (StackX[pathnum] == pathx -1)
					{
						pathx -= 1;
						turn180();
						delay(500);
						Direction=(Direction+2)%4;
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy + 1)
					{
						pathy += 1;
						turnL();
						delay(500);
						Direction=(Direction+3)%4;
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy - 1)
					{
						pathy -= 1;
						turnR();
						delay(500);
						Direction=(Direction+1)%4;
						goStraight();
						delay(500);
					}
				}break;
				case 2: {
					if (StackX[pathnum] == pathx + 1)
					{
						pathx += 1;
						//turnr
						turnL();
						delay(500);
						Direction=(Direction+3)%4;
						goStraight();
						delay(500);
					}
					if (StackX[pathnum] == pathx - 1)
					{
						pathx -= 1;
						turnR();
						delay(500);
						Direction=(Direction+1)%4;
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy + 1)
					{
						pathy += 1;
						turn180();
						delay(500);
						Direction=(Direction+2)%4;
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy - 1)
					{
						pathy -= 1;
						goStraight();
						delay(500);
					}
				}break;
				case 3: {
					if (StackX[pathnum] == pathx + 1)
					{
						pathx += 1;
						//turn
						turn180();
						delay(500);
						Direction=(Direction+2)%4;
						goStraight();
						delay(500);
					}
					if (StackX[pathnum] == pathx - 1)
					{
						pathx -= 1;
						goStraight();
						delay(500);
		
					}
					if (StackY[pathnum] == pathy + 1)
					{
						pathy += 1;
						turnR();
						delay(500);
						Direction=(Direction+1)%4;
						goStraight();
						delay(500);
					}
					if (StackY[pathnum] == pathy - 1)
					{
						pathy -= 1;
						turnL();
						delay(500);
						Direction=(Direction+3)%4;
						goStraight();
						delay(500);
					}
				}break;
			}
		}
		beep=0;
		while(1);
	}
}
void delay(uint z)//延时函数
{
	while(--z)
	{
		i=2;
		j=239;
		do
		{
		while(--j);
		}while(--i);
	}
}
void map_init()//迷宫初始化
{
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			map[i][j]=0xff;
		}
	}
}
void number()//岔路口可走数
{
	num=0;
	switch(Direction)
	{
	   case 0:
	   {
	   	   if(irR==0&&map[X+1][Y]==0xff)
		   {
				num++;
		   }
		   if(irC==0&&map[X][Y+1]==0xff)
		   {
		   	 	num++;
		   }
		   if(irL==0&&map[X-1][Y]==0xff)
		   {
		   		num++;
		   }
	   }break;
	   case 1:
	   {
	   	   if(irR==0&&map[X][Y-1]==0xff)
		   {
		   	 	num++;
		   }
		   if(irC==0&&map[X+1][Y]==0xff)
		   {
		   	 	num++;
		   }
		   if(irL==0&&map[X][Y+1]==0xff)
		   {
		   	   	num++;
		   }
	   }break;
	   case 2:
	   {
	   		if(irR==0&&map[X-1][Y]==0xff)
		   {
		   	 	num++;
		   }
		   if(irC==0&&map[X][Y-1]==0xff)
		   {
		   	 	num++;
		   }
		   if(irL==0&&map[X+1][Y]==0xff)
		   {
		   	   	num++;
		   }
		}break;
		case 3:
		{
		   if(irR==0&&map[X][Y+1]==0xff)
		   {
		   	 	num++;
		   }
		   if(irC==0&&map[X-1][Y]==0xff)
		   {
		   	 	num++;
		   }
		   if(irL==0&&map[X][Y-1]==0xff)
		   {
		   	   	num++;
		   }
		}break;
	}
}
void Back()//回溯函数
{
	while(X!=StackX[stack]||Y!=StackY[stack])
	{
		beep=0;
		delay(500);
		beep=1;
		switch (Direction)
		{
			case 0:
			{
				if((map[X][Y]&0xf0)==0x70)//出去向上
				{
					coordinate(0);
					goStraight();
					delay(N);
				}
				else if((map[X][Y]&0xf0)==0xe0)//出去向左
				{
					turnL();
					delay(N);
					coordinate(3);
					goStraight();
					delay(N);
				}
				else if((map[X][Y]&0xf0)==0xd0)//出去向下
				{
					turn180();
					delay(N);
					coordinate(2);
					goStraight();
					delay(N);
				}
				else if((map[X][Y]&0xf0)==0xb0)//出去向右
				{
					turnR();
					delay(N);
					coordinate(1);
					goStraight();
					delay(N);
				}
			}break;
		case 1:
		{
			if((map[X][Y]&0xf0)==0x70)//出去向上
			{
				turnL();
				delay(N);
				coordinate(3);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xe0)//出去向左
			{
				turn180();
				delay(N);
				coordinate(2);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xd0)//出去向下
			{
				turnR();
				delay(N);
				coordinate(1);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xb0)//出去向右
			{
				coordinate(0);
				goStraight();
				delay(N);
			}
		}break;
		case 2:
		{
			if((map[X][Y]&0xf0)==0x70)//出去向上
			{
				turn180();
				delay(N);
				coordinate(2);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xe0)//出去向左
			{
				turnR();
				delay(N);
				coordinate(1);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xd0)//出去向下
			{
				coordinate(0);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xb0)//出去向右
			{
				turnL();
				delay(N);
				coordinate(3);
				goStraight();
				delay(N);
			}
		}break;
		case 3:
		{
			if((map[X][Y]&0xf0)==0x70)//出去向上
			{
				turnR();
				delay(N);
				coordinate(1);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xe0)//出去向左
			{
				coordinate(0);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xd0)//出去向下
			{
				turnL();
				delay(N);
				coordinate(3);
				goStraight();
				delay(N);
			}
			else if((map[X][Y]&0xf0)==0xb0)//出去向右
			{
				turn180();
				delay(N);
				coordinate(2);
				goStraight();
				delay(N);
			}
		}break;	
		}
	}
	number();
	if(num<2)
		return_Stack();	
}
void return_Stack()//退栈
{
	stack--;
}
void push_Stack()//进栈
{
	stack++;
	StackX[stack]=X;
	StackY[stack]=Y;
	beep=0;
	delay(500);
	beep=1;
	delay(500);
	beep=0;
	delay(500);
	beep=1;
}
void Go_Direction()//进入的方向
{
	if(Direction==0)
	{
		map[X][Y]&=0xdf;
	}
	if(Direction==1)
	{
		map[X][Y]&=0xef;
	}
	if(Direction==2)
	{
		map[X][Y]&=0x7f;
	}
	if(Direction==3)
	{
		map[X][Y]&=0xbf;
	}
}
void Right_Hand()//右手法则
{
	switch(Direction)
	{
		case 0:
		{
			if(irR==0&&map[X+1][Y]==0xff)
			{
					turnR();
					delay(N);
					goStraight();
					delay(N); 
					coordinate(1);
					Go_Direction();
					maze();
			}
			else if(irC==0&&map[X][Y+1]==0xff)
			{
			  	  	coordinate(0);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else if(irL==0&&map[X-1][Y]==0xff)
			{
			  	   	turnL();
					delay(N);
					coordinate(3);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else 
			{
					beep=0;
					delay(500);
					beep=1;
					delay(500);
					beep=0;
					delay(500);
					beep=1;
					Back();
			}
		}break;
		case 1:
		{
			if(irR==0&&map[X][Y-1]==0xff)
			{
					turnR();
					delay(N);
					coordinate(1);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			  
			}
			else if(irC==0&&map[X+1][Y]==0xff)
			{
			  	coordinate(0);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else if(irL==0&&map[X][Y+1]==0xff)
			{
			  	turnL();
					delay(N);
					coordinate(3);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else 
			{
					beep=0;
					delay(500);
					beep=1;
					delay(500);
					beep=0;
					delay(500);
					beep=1;
					Back();
			}
		}break;
		case 2:
		{
			if(irR==0&&map[X-1][Y]==0xff)
			{
					turnR();
					delay(N);
					coordinate(1);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else if(irC==0&&map[X][Y-1]==0xff)
			{
			  	coordinate(0);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else if(irL==0&&map[X+1][Y]==0xff)
			{
					turnL();
					delay(N);
					coordinate(3);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			}
			else 
			{
					beep=0;
					delay(500);
					beep=1;
				   delay(500);
					beep=0;
					delay(500);
					beep=1;

					Back();
			}
		}break;
		case 3:
		{
			  if(irR==0&&map[X][Y+1]==0xff)
			  {
					turnR();
					delay(N);
					coordinate(1);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			  
			  }
			  else if(irC==0&&map[X-1][Y]==0xff)
			  {
			  	coordinate(0);
					Go_Direction();
					goStraight();
					maze();
			  }
			  else if(irL==0&&map[X][Y-1]==0xff)
			  {
			  	turnL();
					delay(N);
					coordinate(3);
					Go_Direction();
					goStraight();
					delay(N); 
					maze();
			  }
			  else 
			  {
					beep=0;
					delay(500);
					beep=1;
					delay(500);
					beep=0;
					delay(500);
					beep=1;

					Back();
			  }
		}break;
	}
	beep=0;
	delay(500);
	beep=1;
}
void coordinate(uchar direction) //绝对方向  坐标更改
{
	Direction=(Direction+direction)%4;
	if(Direction==0)//上
	{
		Y++;
	}
	else if(Direction==1)//右
	{
		X++;
	}
	else if(Direction==2)//下
	{
		Y--;
	}
	else if(Direction==3)//左
	{
		X--;
	}
}
void maze()//迷宫信息
{
	if(Direction==0)
	{
		if(irC==0)
		{
			map[X][Y]&=0xf5;
		}
		if(irR==0)
		{
			map[X][Y]&=0xf9;
		}
		if(irL==0)
		{
			map[X][Y]&=0xfc;
		}
		if(irC&&irR&&irL)
		{
		   map[X][Y]&=0xfd;
		}
	}
	else if(Direction==1)
	{
		if(irC==0)
		{
			map[X][Y]&=0xfa;
		}
		if(irR==0)
		{
			map[X][Y]&=0xf9;
		}
		if(irL==0)
		{
			map[X][Y]&=0xf6;
		}
		if(irC&&irR&&irL)
		{
		   map[X][Y]&=0xfe;
		}
	}
	else if(Direction==2)
	{
		if(irC==0)
		{
			map[X][Y]&=0xf5;
		}
		if(irR==0)
		{
			map[X][Y]&=0xf6;
		}
		if(irL==0)
		{
			map[X][Y]&=0xf3;
		}
		if(irC&&irR&&irL)
		{
		   map[X][Y]&=0xf7;
		}
	}
	else if(Direction==3)
	{
		if(irC==0)
		{
			map[X][Y]&=0xfa;
		}
		if(irR==0)
		{
			map[X][Y]&=0xf3;
		}
		if(irL==0)
		{
			map[X][Y]&=0xf9;
		}
		if(irC&&irR&&irL)
		{
		   map[X][Y]&=0xfb;
		}
	}
}
void inittime(uint ML,uint MR)//
{
	TMOD=0x66;
	TH0=TL0=256-ML;
	TH1=TL1=256-MR;
	EA=1;
	ET0=1;
	ET1=1;
	TR0=1;
	TR1=1;
}
sfr T2MOD=0xc9;
void initTime2(uint us)//定时器T2初始化
{
	T2MOD=0;
	T2CON=0;
	TL2=RCAP2L=(65536-us)/256;
	TH2=RCAP2H=(65536-us)%256;
	TR2=1;
	ET2=1;
}
void runR(uchar flag)	//右电机
{
	if(flag)  //非0则正转
	{
		MR1=1;
		MR2=0;
	}
	else	  //反之逆转
	{
		MR1=0;
		MR2=1;
	}
}
void runL(uchar flag)//左电机
{
	if(flag)
	{
		ML1=0;
		ML2=1;
	}
	else
	{
		ML1=1;
		ML2=0;
	}
}
void stopL()//左电机停止
{
	ML1=1;
	ML2=1;
}
void stopR()//右电机停止
{
	MR1=1;
	MR2=1;	
}
 


void turnL()//左转函数
{
	intCountL=intCountR=0;
	while((intCountL<=98)||(intCountR<=98))//95
	{
		if(intCountL<intCountR)
		{
			stopR();
			runL(0);
		}
		else
		{
			stopL();	
			runR(1);
		}
	}
	stopL();
	stopR();
	delay(500);
}
void turnR()//右转函数
{
	intCountL=intCountR=0;
	while((intCountL<=98)||(intCountR<=98))
	{
		if(intCountL<intCountR)
		{
			stopR();
			runL(1);
		}
		else
		{
			stopL();	
			runR(0);
		}
	}
	stopL();
	stopR();
	delay(500);	
}
void turn180()	//后转函数
{
	intCountL=0;
	intCountR=0;
	while((intCountL<=200)||(intCountR<=200))
	{
		if(intCountL<intCountR)
		{
			stopR();
			runL(1);
		}
		else
		{
			stopL();
			runR(0);
		}
	}
	stopL();
	stopR();
	delay(1000);
}
void goStraight()//直走
{
	intCountL=intCountR=0;
	while(intCountL<250||intCountR<250)	 //230
	{
		if(intCountL<intCountR)
		{
			stopR();
			runL(1);
		}
		else
		{
			stopL();	
			runR(1);
		}
		if(irC==1)
		{
			break;
		}
		if(irLU&&!irC)//小车向左偏转
		{
			CountL=intCountL;
			CountR=intCountR;
			while(irLU)
			{
				if(intCountL<intCountR)
				{
					stopR();
					runL(1);
				}
			  	else
				{
					stopL();
					runR(0);
				}
				if(intCountL-CountL>=2||intCountR-CountR>=2)
				{
					break;
				}
			}
			intCountL=CountL;
			intCountR=CountR;
		}
		if(irRU&&!irC)//小车向右偏转
		{
			CountL=intCountL;
			CountR=intCountR;
			while(irRU)
			{
				if(intCountL<intCountR)
				{
					stopR();
					runL(0);
					
				}
			  	else
				{
					stopL();
					runR(1);
				}
				if(intCountL-CountL>=2||intCountR-CountR>=2)
				{
					break;
				}
			}
			intCountL=CountL;
			intCountR=CountR;
			
		}
	}	
	stopR();
	stopL();
	delay(1000);
}
void main()
{
	inittime(1,1);
	initTime2(5000);
	map_init();
	initStack();
	StackX[0]=0;
	StackY[0]=0;
	delay(1000);
	while(1)
	{
		if(X==7&&Y==7)//判断是否走到终点
		{
			beep=0;
			delay(N);
			beep=1;
			delay(N);
			beep=0;
			delay(N);
			beep=1;
			delay(N);
			beep=0;
			delay(N);
			beep=1;
		}
		number();
		if(num>=2)//判断当前路口是否入栈
		{
		   push_Stack();
		}
		Right_Hand();//右手法则
		bestPath();	//最短路径
	}
}
void tiem0() interrupt 1//左电机计数
{
	intCountL++;
}
void time1() interrupt 3//右电机计数
{
	intCountR++;
}
void time2() interrupt 5 //定时器T2中断
{
	static unsigned char irNo=1;
	TF2=0;
	switch(irNo)
	{
		case 1:A0=0;A1=0;A2=0;break;
		case 2:
		{
			if(irR1==1)//没检测到
			{
				irLU=0;//左前
				led1=1;
			}
			else
			{
				irLU=1;
				led1=0;
			}
			A0=1;A1=0;A2=0;
			break;
		}
		case 3:
		{
			if(irR2==1)
			{
				irC=0;
				led2=1;
			}
			else
			{
				irC=1;
				led2=0;
			}
			A0=0;A1=1;A2=0;
			break;
		}
		case 4:
		{
			if(irR3==1)
			{
				irRU=0;
				led3=1;
			}
			else
			{
				irRU=1;
				led3=0;
			}
			A0=1;A1=1;A2=0;
			break;
		}
		case 5:
		{
			if(irR4==1)
			{
				irL=0;
				led4=1;
			}
			else
			{
				irL=1;
				led4=0;
			}
			A0=0;A1=0;A2=1;
			break;
		}
		case 6:
		{
			if(irR5==1)
			{
				irR=0;
				led5=1;
			}
			else
			{
				irR=1;
				led5=0;
			}
			A0=1;A1=1;A2=1;//关闭
			break;
		}
	}
	if(irNo<6)
		irNo++;
	else
		irNo=1;	
}
