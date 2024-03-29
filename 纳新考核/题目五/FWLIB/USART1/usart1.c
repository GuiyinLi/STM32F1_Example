#include "usart1.h"
#include "stm32f10x_usart.h"
#include "led.h"
u8 USART1_RX_BUF[USART1_REC_LEN]; //数组用来存储接收到的数据，而USART3_REC_LEN为最多能接收的字节限度
u16 USART1_RX_STA=0;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位
void My_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE);   //串口和GPIO口使能
	
	//GPIO端口模式设置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;    //PA9 TXD 推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;    //PA10 RXD 浮空输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//串口初始化
	USART_InitStructure.USART_BaudRate=115200;               //设置波特率
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;    //不适用硬件流
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;                      //使能发送和接受模式
	USART_InitStructure.USART_Parity=USART_Parity_No;                                //不使用奇偶校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;                             //设置停止位 1
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;                        //设置字长 8位
	USART_Init(USART1,&USART_InitStructure);
	
	USART_Cmd(USART1,ENABLE);                                                        //使能串口1
	
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);               //使能接受中� 用DMA中断接收数据需要把此中断关闭
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); //使能串口1的DMA接收  
	
	//开启中断分组初始化
	NVIC_InitInitStructure.NVIC_IRQChannel=USART1_IRQn;                              //串口1通道
	NVIC_InitInitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitInitStructure.NVIC_IRQChannelPreemptionPriority=1;                      //抢占优先级为1
	NVIC_InitInitStructure.NVIC_IRQChannelSubPriority=1;                             //响应优先级为1
	NVIC_Init(&NVIC_InitInitStructure);
}
//编写中断处理函数
/*void USART1_IRQHandler(void)
{
	u8 res;//暂时缓存接收的数据
   if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//判断是否为串口1接收中断
	 {
		   
	     res=USART_ReceiveData(USART1);//接收到数据放进res
		   USART1_RX_BUF[USART1_RX_STA&0x7FFF]=res;//数据放进数组中，则可以用到main函数中了
		   USART1_RX_STA++;                         //字节长度++
		 if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	+; 
		  USART1_RX_STA|=0x8000;//串口3接收完成
		 USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除接收中断标志
		   
	  }
}*/


