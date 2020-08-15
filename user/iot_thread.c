#include "iot_thread.h"
#include "rtthread.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "AepServiceCodes.h"
#include "user_main.h"
#include "com.h"
#include "usart.h"


#define IOT_RECV_DATA_DEAL_THREAD_NAME "iot_recv_deal"
#define IOT_RECV_DATA_DEAL_THREAD_STACK_SIZE 512+128 
#define IOT_RECV_DATA_DEAL_THREAD_PRIORITY  4



#define IOT_CHECK_NET_STATUS_THREAD_NAME "iot_check_net_status"
#define IOT_CHECK_NET_STATUS_THREAD_STACK_SIZE 384 
#define IOT_CHECK_NET_STATUS_THREAD_PRIORITY  5



NET_STATUS net_status=NET_DOWN;

uint8_t recv_data_buff[256]={0};


thread_info_typedef iot_thread_table[]={
	{IOT_RECV_DATA_DEAL_THREAD_NAME,iot_recv_data_deal_entry,RT_NULL,IOT_RECV_DATA_DEAL_THREAD_STACK_SIZE,IOT_RECV_DATA_DEAL_THREAD_PRIORITY,10},
	{IOT_CHECK_NET_STATUS_THREAD_NAME,iot_check_net_status_entry,RT_NULL,IOT_CHECK_NET_STATUS_THREAD_STACK_SIZE,IOT_CHECK_NET_STATUS_THREAD_PRIORITY,10}
};

#define iot_thread_table_size (sizeof(iot_thread_table)/sizeof(thread_info_typedef))


#define NET_STATUS_CHECK_TIME 3000


extern struct rt_ringbuffer uart_bc28_rxcb;

extern rt_sem_t at_rx_sem ; 


//extern rt_uint8_t uart_bc28_rx_buff[UART_BC28_RX_BUF_LEN];

uint8_t  at_sending_flag=0;

void iot_recv_data_deal_entry(void * para);

void recv_urc_data_deal(rt_uint8_t *data);

void iot_check_net_status_entry(void *para);

void iot_creat_user_task(void);


void iot_creat_user_task(void)
{
	  rt_thread_t tid=0;
	  uint8_t result=0;
	
		for(uint8_t i=0;i<iot_thread_table_size;i++)
			{
				
					tid=rt_thread_create(iot_thread_table[i].thread_name,iot_thread_table[i].entry,iot_thread_table[i].parameter,iot_thread_table[i].stack_size,iot_thread_table[i].priority,iot_thread_table[i].tick);
					if(tid==RT_NULL)
					{
						return;
					}
					
					result=rt_thread_startup(tid);	
					
					if(result==RT_EOK)
					{
						rt_kprintf("%s begain.....\r\n",iot_thread_table[i].thread_name);
						
					}
					else					
					return ;
					
			}
	
}

void iot_check_net_status_entry(void *para)
{
	
    uint8_t recvbuff[50]={0};
		
			
					
		while(1)
		{
			
			rt_thread_mdelay(NET_STATUS_CHECK_TIME);
			
					
//			if(AT_RESPONSE_OK==at_send_cmd("AT+CEREG?",recvbuff,sizeof(recvbuff),20))
//			{
//			   
//				if(!strstr((char *)recvbuff,"+CEREG:0,1"))
//				{
//					
//					rt_kprintf("net registration failed...\r\n");
//				}
//			}
			
			rt_thread_mdelay(200);

			if(AT_RESPONSE_OK==at_send_cmd("AT+CGATT?",recvbuff,sizeof(recvbuff),100))
			{
			   
				if(!strstr((char *)recvbuff,"+CGATT:1"))
				{
					net_status=NET_DOWN;
					rt_kprintf("net activation failed..\r\n");
				}
				else
				{
					net_status=NET_UP;
					
				}
			}
				else
					{
						net_status=NET_DOWN;
					}			
		}	
}


void iot_recv_data_deal_entry(void* para)	
{
	
//	rt_uint8_t recvbuff[200]={0};
	
	rt_uint16_t readsize=0;
		
	while(1)
	{
		

		
		 if(at_sending_flag)
		 {
			 rt_thread_mdelay(10);
			 continue;
		 }
	 		  
		  if(rt_ringbuffer_data_len(&uart_bc28_rxcb)<10)
			{
				rt_thread_mdelay(100);
				continue;
			}
		  
		  readsize=rt_ringbuffer_get(&uart_bc28_rxcb,recv_data_buff,sizeof(recv_data_buff));
									
			
		  if(readsize)
			{
				//rt_kprintf("recv len %d,recv data:%s",readsize,recvbuff);
				recv_urc_data_deal(recv_data_buff);
				
				memset(recv_data_buff,0,sizeof(recv_data_buff));
			}

	}
	
	
}




void recv_urc_data_deal(rt_uint8_t *data)
{
		char *index=0;	
	
    AepCmdData _aep_cmd_data={0};
		
		
		if(strstr((char *)data,"+NNMI:"))
			{
				
		    index=strstr((char *)data,",");
				index+=1;
				//AepCmdData 
				_aep_cmd_data=decodeCmdDownFromStr(index);				
				rt_kprintf("code %d,taskID %d,serviceIdentifier %s\r\n",_aep_cmd_data.code,_aep_cmd_data.taskId,_aep_cmd_data.serviceIdentifier);
						
			}
		if(strstr((char *)data,"+QLWEVTIND:2"))
		{//+QLWEVTIND:2  注册状态更新
			rt_kprintf("module registration status update...\r\n");
		}
		//+NPSMR:
			
		if(strstr((char *)data,"+NPSMR:0"))
		{//+QLWEVTIND:2  //模组唤醒
			rt_kprintf("module wakeup...\r\n");
		}	
		
		if(strstr((char *)data,"+NPSMR:1"))
		{//+QLWEVTIND:2  //模组进入休眠模式
			rt_kprintf("module enter sleep...\r\n");
		}	
	
}








