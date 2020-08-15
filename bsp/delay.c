
#include <stdbool.h>
#include "delay.h"
#include "stm32f1xx_hal.h"


//void delay_us(unsigned int n)
//{
//	u8 j;
//	while(n--)
//	for(j=0;j<10;j++);
//}
//void delay_ms(unsigned int n)
//{
//	while(n--)
//	delay_us(1000);
//}

#if false
void delay_us(unsigned int n)
{
	SysTick->CTRL|= DISABLE;         
	SysTick->LOAD= n*9;       //ʱ�����  
	SysTick->VAL= 0;        //��ռ�����      
 	SysTick->CTRL|= ENABLE;          
 	while(SysTick->VAL||!(SysTick->CTRL&(1<<16)));//�ȴ�ʱ�䵽�� 
 	SysTick->CTRL=0X00000000;       //�رռ�����
 	SysTick->VAL= 0;        //��ռ�����  
}
void delay_ms(unsigned int n)
{
	while(n--)
	{
		delay_us(1000);		
	}
}
#else

__ASM void __INLINE delay_us(uint32_t volatile number_of_us)
{
loop
        SUBS    R0, R0, #1 //564ns
        NOP                //686ns
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
				
				NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
				
				NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP    //9.80us
				
				NOP  //9.93us
				NOP  //10.04us

        BNE    loop
        BX     LR
}

#if false
void delay_ms(uint32_t n)
{
	while(n--)
	{
		delay_us(1000);
	}
}
#else
void delay_ms(uint32_t n)
{
	uint32_t tick;
	
	tick = TIME2TICK(n);
	HAL_Delay(tick);
}
#endif



#endif


