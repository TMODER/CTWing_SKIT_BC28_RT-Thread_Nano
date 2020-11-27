#include "user_main.h"
#include "com.h"
#include "string.h"
#define BC28_INIT_THREAD_NAME "bc28_init"
#define BC28_INIT_THREAD_STACK_SIZE 512
#define BC28_INIT_THREAD_PRIORITY  6


#define LED_THREAD_NAME "led_task"
#define LED_THREAD_STACK_SIZE 128
#define LED_THREAD_PRIORITY  7


#define READ_DATA_THREAD_NAME "iot_read_data_thread"
#define READ_DATA_THREAD_STACK_SIZE 1024
#define READ_DATA_THREAD_PRIORITY  5


__packed typedef struct at_urc
{
	char* respos;
	void (* func);
	//void (* func)(rt_uint8_t *data);
	
}AT_URC;

extern  int  uart1_recv_cnt;


void creat_user_task(void);

void led_task_entry(void *para);

static void read_data_from_module_entry(void *para);


extern NET_STATUS net_status;

extern rt_sem_t recv_urc_data_sem;

extern rt_sem_t uart1_read_data_sem;


thread_info_typedef base_thread_table[] =
{
	 {READ_DATA_THREAD_NAME,read_data_from_module_entry,RT_NULL,READ_DATA_THREAD_STACK_SIZE,READ_DATA_THREAD_PRIORITY,10},
	 {BC28_INIT_THREAD_NAME, bc28_init_thread_entry, RT_NULL, BC28_INIT_THREAD_STACK_SIZE, BC28_INIT_THREAD_PRIORITY, 10},
	 {LED_THREAD_NAME, led_task_entry, RT_NULL, LED_THREAD_STACK_SIZE, LED_THREAD_PRIORITY, 10},
};

#define thread_table_size   sizeof(base_thread_table)/sizeof(thread_info_typedef)

extern struct rt_ringbuffer uart_bc28_rxcb;

uint8_t  at_response_buff[512]={0};   //AT缓存
uint8_t  urc_response_buff[512]={0};  //URC 缓存


AT_URC AT_URC_Table[]=
{
	{"+NNMI:",0}, //收到消息
	{"+QLWEVTIND:2:",0},//注册状态更新
	{"+NPSMR:0",0},//模组唤醒
	{"+NPSMR:1",0}, //进入休眠
	//{"+QLWEVTIND:2:",0}, //
};

#define AT_URC_Table_Size  sizeof(AT_URC_Table)/sizeof(AT_URC)


void creat_user_task(void)
{
    rt_uint8_t result = 0;
    rt_uint8_t i = 0;
    rt_thread_t tid = 0;
    for (i = 0; i < thread_table_size; i++)
    {

        tid = rt_thread_create(base_thread_table[i].thread_name, base_thread_table[i].entry, base_thread_table[i].parameter,
                               base_thread_table[i].stack_size, base_thread_table[i].priority, base_thread_table[i].tick);
        if (tid == RT_NULL)
        {
            return;
        }

        result = rt_thread_startup(tid);

        if (result == RT_EOK)
        {
            rt_kprintf("%s begain.....\r\n", base_thread_table[i].thread_name);

        }
        else
        {
            return;
        }

    }

}


//利用回显 分离 AT响应消息及URC消息   串口收到空闲中断 就读取数据拷贝到对应缓冲区

static void read_data_from_module_entry(void *para)
{
  uint8_t recv_buff[512]={0};
 // uint8_t result=0;
	while(1)
	{
		
			  rt_sem_take(uart1_read_data_sem, RT_WAITING_FOREVER);

      	rt_ringbuffer_get(&uart_bc28_rxcb,recv_buff,512);
				
				
				if(strstr((char *)recv_buff,"AT"))
				{				
					memcpy(at_response_buff,recv_buff,uart1_recv_cnt);
				}
				else
				{
					memcpy(urc_response_buff,recv_buff,uart1_recv_cnt);	
					//urc_recv_data_deal(recv_buff);
         rt_sem_release(recv_urc_data_sem);
				}			
				memset(recv_buff,0,sizeof(recv_buff));
       uart1_recv_cnt=0;						
	}		
}




void led_task_entry(void *para)
{
    while (1)
    {
        if (net_status == NET_DOWN)
        {
            HAL_GPIO_TogglePin(RGB2_G_GPIO_Port, RGB2_G_Pin);
            rt_thread_mdelay(100);

        }
        if (net_status == NET_UP)
        {
            HAL_GPIO_TogglePin(RGB2_G_GPIO_Port, RGB2_G_Pin);
            rt_thread_mdelay(500);
        }

    }

}















