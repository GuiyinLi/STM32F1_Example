#include "stm32f10x.h"
#include "sys.h"
#include "led.h"
#include "delay.h" 
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include "usart.h"
#include "string.h"
#include "sram.h"
int main()
{
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 paddr[18];			//存放P Addr:+p地址的ASCII值
	u8 sramx=0;				//默认为内部sram
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD  
  FSMC_SRAM_Init();			//初始化外部SRAM  	
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	POINT_COLOR=RED;			//设置字体为红色 
	LCD_ShowString(30,130,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,150,200,16,16,"KEY_UP:SRAMx KEY1:Write");
	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(30,170,200,16,16,"SRAMIN");
	LCD_ShowString(30,190,200,16,16,"SRAMIN USED:   %");
	LCD_ShowString(30,210,200,16,16,"SRAMEX USED:   %");
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case 0:
				break;
			case KEY0_PRES: 
				p=mymalloc(sramx,2*1024);    //申请2K字节
			  if(p!=NULL) sprintf((char*)p,"Memory Malloc Test%03d",i);//向p写入一些内容
			  break;
			case KEY1_PRES:
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//更新显示内容 	 
					LCD_ShowString(30,250,200,16,16,p);			 //显示P的内容
				}
				break;
			case KEY2_PRES:
				myfree(sramx,p);              //释放内存
				p=0;                           //指向空地址
			  break;
			case WKUP_PRES:
				sramx=!sramx;	//切换当前malloc/free操作对象
				if(sramx)LCD_ShowString(30,170,200,16,16,"SRAMEX");
				else LCD_ShowString(30,170,200,16,16,"SRAMIN");
				break;
		}
		if(tp!=p)
		{
			tp=p;
			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
			LCD_ShowString(30,230,200,16,16,paddr);	//显示p的地址
			if(p)LCD_ShowString(30,250,200,16,16,p);//显示P的内容
		  else LCD_Fill(30,250,239,266,WHITE);	//p=0,清除显示
		}
		delay_ms(10);
		i++;
		if((i%20)==0)
		{
			LCD_ShowNum(30+96,190,my_mem_perused(SRAMIN),3,16);//显示内部内存使用率
			LCD_ShowNum(30+96,210,my_mem_perused(SRAMEX),3,16);//显示外部内存使用率
			LED0=!LED0;
		}
	}
	
}


