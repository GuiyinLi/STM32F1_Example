#ifndef _can_h
#define _can_h
#include "sys.h"
#include "stm32f10x_can.h"

#define CAN_RX0_INT_ENABLE	0		//0,不使能;1,使能.								    
										 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
 
u8 Can_Send_Msg(u8* msg,u8 len);						//发送数据

u8 Can_Receive_Msg(u8 *buf);							//接收数据


#endif

