#include "user_main.h"
#include "com.h"
#define BC28_INIT_THREAD_NAME "bc28_init"
#define BC28_INIT_THREAD_STACK_SIZE 512 
#define BC28_INIT_THREAD_PRIORITY  5


#define LED_THREAD_NAME "led_task"
#define LED_THREAD_STACK_SIZE 128 
#define LED_THREAD_PRIORITY  7



extern NET_STATUS net_status;

thread_info_typedef base_thread_table[]={
														{BC28_INIT_THREAD_NAME,bc28_init_thread_entry,RT_NULL,BC28_INIT_THREAD_STACK_SIZE,BC28_INIT_THREAD_PRIORITY,10}, 
                            {LED_THREAD_NAME,led_task_entry,RT_NULL,LED_THREAD_STACK_SIZE,LED_THREAD_PRIORITY,10}, 											
                            };

#define thread_table_size   sizeof(base_thread_table)/sizeof(thread_info_typedef)
																									

void creat_user_task(void);

void led_task_entry(void * para);

void creat_user_task(void)
{
	rt_uint8_t result=0;
	rt_uint8_t i=0;
	rt_thread_t tid=0;
	for(i=0;i<thread_table_size;i++)
	{
		
			tid=rt_thread_create(base_thread_table[i].thread_name,base_thread_table[i].entry,base_thread_table[i].parameter,base_thread_table[i].stack_size,base_thread_table[i].priority,base_thread_table[i].tick);
			if(tid==RT_NULL)
			{
				return;
			}
			
			result=rt_thread_startup(tid);	
			
			if(result==RT_EOK)
			{
				rt_kprintf("%s begain.....\r\n",base_thread_table[i].thread_name);
				
			}
			else
				
			return ;
			
	}
	
}


void led_task_entry(void * para)
{
	while(1)
	{
	 if(net_status==NET_DOWN)
	 {
		 HAL_GPIO_TogglePin(RGB2_G_GPIO_Port,RGB2_G_Pin);
		 rt_thread_mdelay(100);
		 
	 }
	 if(net_status==NET_UP)
	 {
		 HAL_GPIO_TogglePin(RGB2_G_GPIO_Port,RGB2_G_Pin);
		 rt_thread_mdelay(500);
	 }
	 
 }
	
}















