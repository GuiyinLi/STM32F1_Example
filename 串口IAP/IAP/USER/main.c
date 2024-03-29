#include "sys.h"
#include "led.h"
#include "delay.h"   
#include "usart.h"
#include "lcd.h"
#include "stmflash.h"
#include "key.h"
#include "iap.h"

int main()
{
	u8 key;
	u8 t;
	u8 clearflag=0;
	u16 applenth=0;          //接收到的app代码长度
	u16 oldcount=0;       //老的串口接收值
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	//串口初始化为115200
	delay_init();	   	 	//延时初始化 
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD  
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(30,30,200,16,16,"KEY_UP:Copy APP2FLASH");
	LCD_ShowString(30,50,200,16,16,"KEY2:Erase SRAM APP");
	LCD_ShowString(30,70,200,16,16,"KEY1:Run FLASH APP");
	LCD_ShowString(30,90,200,16,16,"KEY0:Run SRAM APP");
	POINT_COLOR=BLUE;
	//显示提示信息
	POINT_COLOR=BLUE;//设置字体为蓝色	  
	while(1)
	{
		if(USART_RX_CNT)   //正确 说明此次接收完后
		{
			if(oldcount==USART_RX_CNT)
			{
				applenth=USART_RX_CNT;   //获取信息长度
				oldcount=0;
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",applenth);
			}else oldcount=USART_RX_CNT;
		}
		t++;
		delay_ms(10);
		if(t==30)
		{
			LED0=!LED0;
			t=0;
			if(clearflag)
			{
				clearflag--;
				if(clearflag==0)LCD_Fill(30,210,240,210+16,WHITE);//清除显示
			}
		}
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			if(applenth)
			{
				printf("开始更新固件。。。\r\n");
				LCD_ShowString(30,210,200,16,16,"Copying APP2FLASH...");
				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);  //更新FLASH代码
					LCD_ShowString(30,210,200,16,16,"Copy APP Successed!!");
					printf("固件更新完成\r\n");
				}
				else
				{
					LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!  ");	   
					printf("非FLASH应用程序!\r\n");
				}
			}
			else
			{
				printf("没有可以更新的固件!\r\n");
				LCD_ShowString(30,210,200,16,16,"No APP!");
			}
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示								
		}
		if(key==KEY2_PRES)
		{
			if(applenth)
			{
				printf("固件清除完成\r\n");
				LCD_ShowString(30,210,200,16,16,"APP Erase Successed!");
				applenth=0;
			}
			else
			{
				printf("没有可以清除的固件！\r\n");
				LCD_ShowString(30,210,200,16,16,"No APP!");
			}
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示				
		}
		if(key==KEY1_PRES)
		{
			printf("开始执行FLASH代码！\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				printf("非FLASH应用程序,无法执行!\r\n");
				LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!");	   
			}									 
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示	  
		}
		if(key==KEY0_PRES)
		{
			printf("开始执行SRAM用户代码!!\r\n");
			if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//判断是否为0X20XXXXXX.
			{	 
				iap_load_app(0X20001000);//SRAM地址
			}else 
			{
				printf("非SRAM应用程序,无法执行!\r\n");
				LCD_ShowString(30,210,200,16,16,"Illegal SRAM APP!");	   
			}									 
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示	 
		}				   
	}
}





