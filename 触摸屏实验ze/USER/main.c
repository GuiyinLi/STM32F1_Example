#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "display.h"

 int main(void)
 {	 		    
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	 
 	LED_Init();			     //LED端口初始化
	LCD_Init();	
	KEY_Init();	 	
 	tp_dev.init();

	if(tp_dev.touchtype&0X80)ctp_test();	//电容屏测试
	else rtp_test(); 						//电阻屏测试
}


