#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "string.h"
#include "ov7670.h"
#include "timer.h"
#include "exti.h"

const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};							//5种光照模式	    
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
extern u8 ov_sta;	//在exit.c里 面定义
extern u8 ov_frame;	//在timer.c里面定义		 

//更新LCD显示
void camera_refresh(void)
{
	u32 j;
	u16 color;
	if(ov_sta)
	{
		LCD_Scan_Dir(U2D_L2R);           //设置扫描方式 从上到下 从左到右
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//将显示区域设置到屏幕中央
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();                 //开始写入GRAM
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)                              //320*240
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			LCD->LCD_RAM=color;    
		}   							  
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}	   
		
int main()
{
	u8 key;
	u8 lightmode=0,saturation=2,brightness=2,contrast=2;   //设置默认值
	u8 i=0;
	u8 msgbuf[15];                                             //消息缓存区 第二行的信息
	u8 tm=0;
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);
	LCD_Init();
	KEY_Init();
	LED_Init();
	POINT_COLOR=RED;			//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(30,70,200,16,16,"KEY1:Saturation");
	LCD_ShowString(30,90,200,16,16,"KEY2:Brightness");
	LCD_ShowString(30,110,200,16,16,"KEY_UP:Contrast");
	LCD_ShowString(30,130,200,16,16,"OV7670 Init...");
	while(OV7670_Init())
	{
		LCD_ShowString(30,150,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	   LCD_Fill(30,150,200+30,150+16,WHITE);
		delay_ms(200);
	}
	LCD_ShowString(30,150,200,16,16,"OV7670 Init OK");
	delay_ms(1500);	 	   
	OV7670_Light_Mode(lightmode);
	OV7670_Color_Saturation(saturation);
	OV7670_Brightness(brightness);
	OV7670_Contrast(contrast);
	TIM2_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
	EXTI8_Init();						//使能外部中断8
	OV7670_Window_Set(12,176,240,320);	//设置窗口	  
  OV7670_CS=0;					
	LCD_Clear(BLACK);                   //清屏
	while(1)
	{
		key=KEY_Scan(0);
		if(key)
		{
			tm=20;
			switch (key)
			{
				case KEY0_PRES:	//灯光模式Light Mode
					lightmode++;
					if(lightmode>4) lightmode=0;
					OV7670_Light_Mode(lightmode);
					sprintf((char*)msgbuf,"%s",LMODE_TBL[lightmode]);
					break;
				case KEY1_PRES:	//饱和度Saturation
					saturation++;
					if(saturation>4)saturation=0;
					OV7670_Color_Saturation(saturation);
					sprintf((char*)msgbuf,"Saturation:%d",(signed char)saturation-2);
					break;
				case KEY2_PRES:	//亮度Brightness				 
					brightness++;
					if(brightness>4)brightness=0;
					OV7670_Brightness(brightness);
					sprintf((char*)msgbuf,"Brightness:%d",(signed char)brightness-2);
					break;
				case WKUP_PRES:	//对比度Contrast			    
					contrast++;
					if(contrast>4)contrast=0;
					OV7670_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-2);
					break;
			}
		}
		camera_refresh();//更新显示
 		if(tm)
		{
			LCD_ShowString((lcddev.width-240)/2+30,(lcddev.height-320)/2+60,200,16,16,msgbuf);
			tm--;
		}
		i++;
		if(i==15)//DS0闪烁.
		{
			i=0;
			LED0=!LED0;
 		}
	}
}


