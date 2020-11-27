#include "iot_thread.h"
#include "rtthread.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "AepServiceCodes.h"
#include "user_main.h"
#include "com.h"
#include "usart.h"
#include "HTS221.H"

#define IOT_RECV_DATA_DEAL_THREAD_NAME "iot_recv_deal"
#define IOT_RECV_DATA_DEAL_THREAD_STACK_SIZE 512
#define IOT_RECV_DATA_DEAL_THREAD_PRIORITY  4



#define IOT_CHECK_NET_STATUS_THREAD_NAME "iot_check_net_status"
#define IOT_CHECK_NET_STATUS_THREAD_STACK_SIZE 256
#define IOT_CHECK_NET_STATUS_THREAD_PRIORITY  10


//#define IOT_REPORT_DATA_THREAD_NAME  "iot_report_data"
//#define IOT_REPORT_DATA_THREAD_STACK_SIZE 512+128
//#define IOT_REPORT_DATA_THREAD_PRIORITY 4



NET_STATUS net_status = NET_DOWN;

extern rt_sem_t recv_urc_data_sem;

extern uint8_t  urc_response_buff[512];

void recv_data_deal_entry(void *para);

static void recv_urc_data_deal(rt_uint8_t *data);

void iot_creat_user_task(void);

void iot_check_net_status_entry(void * para);

uint8_t iot_send_data(void * data);



thread_info_typedef iot_thread_table[] =
{
    {IOT_RECV_DATA_DEAL_THREAD_NAME, recv_data_deal_entry, RT_NULL, IOT_RECV_DATA_DEAL_THREAD_STACK_SIZE, IOT_RECV_DATA_DEAL_THREAD_PRIORITY, 10},
		
    {IOT_CHECK_NET_STATUS_THREAD_NAME, iot_check_net_status_entry, RT_NULL, IOT_CHECK_NET_STATUS_THREAD_STACK_SIZE, IOT_CHECK_NET_STATUS_THREAD_PRIORITY, 10}
};

#define iot_thread_table_size (sizeof(iot_thread_table)/sizeof(thread_info_typedef))


#define NET_STATUS_CHECK_TIME 3000


extern struct rt_ringbuffer uart_bc28_rxcb;




void iot_creat_user_task(void)
{
    rt_thread_t tid = 0;
    uint8_t result = 0;
 
    for (uint8_t i = 0; i < iot_thread_table_size; i++)
    {

        tid = rt_thread_create(iot_thread_table[i].thread_name, iot_thread_table[i].entry, iot_thread_table[i].parameter,
                               iot_thread_table[i].stack_size, iot_thread_table[i].priority, iot_thread_table[i].tick);
        if (tid == RT_NULL)
        {
					  rt_kprintf("%s thread create failed\r\n",iot_thread_table[i].thread_name);
            return;
        }

        result = rt_thread_startup(tid);

        if (result == RT_EOK)
        {
            rt_kprintf("%s begain.....\r\n", iot_thread_table[i].thread_name);

        }
        else
            return ;

    }

}




//检测入网状态
void iot_check_net_status_entry(void *para)
{

    uint8_t recvbuff[50] = {0};

    while (1)
    {

        rt_thread_mdelay(NET_STATUS_CHECK_TIME);

        if (AT_RESPONSE_OK == at_send_cmd((uint8_t *)"AT+CGATT?", recvbuff, sizeof(recvbuff), 100))
        {

            if (!strstr((char *)recvbuff, "+CGATT:1"))
            {
                net_status = NET_DOWN;
                rt_kprintf("net activation failed..\r\n");
            }
            else
            {
                net_status = NET_UP;

            }
        }
        else
        {
            net_status = NET_DOWN;
        }
				memset(recvbuff,0,50);
				
    }
}


//收到URC 的信号量就处理 URC缓冲区

void recv_data_deal_entry(void *para)
{

//    rt_uint16_t readsize = 0;

	
    while (1)
    {
      if(rt_sem_take(recv_urc_data_sem,RT_WAITING_FOREVER)!=RT_EOK)
			{
					rt_kprintf("take urc sem failed.\r\n");
			}
			
			recv_urc_data_deal(urc_response_buff);
	   	memset(urc_response_buff,0,sizeof(urc_response_buff));
    }
}








static void recv_urc_data_deal(rt_uint8_t *data)
{
    char *index = 0;

    AepCmdData _aep_cmd_data = {0};


    if (strstr((char *)data, "+NNMI:"))
    {

        index = strstr((char *)data, ",");
        index += 1;
        //AepCmdData
        _aep_cmd_data = decodeCmdDownFromStr(index);
        rt_kprintf("code %d,taskID %d,serviceIdentifier %s\r\n", _aep_cmd_data.code, _aep_cmd_data.taskId,
                   _aep_cmd_data.serviceIdentifier);

    }
    if (strstr((char *)data, "+QLWEVTIND:2"))
    {
        //+QLWEVTIND:2  注册状态更新
        rt_kprintf("module registration status update...\r\n");
    }
    //+NPSMR:

    if (strstr((char *)data, "+NPSMR:0"))
    {
                  //模组唤醒
			  net_status = NET_UP;
        rt_kprintf("module wakeup...\r\n");
    }

    if (strstr((char *)data, "+NPSMR:1"))
    {
        //+QLWEVTIND:2  //模组进入休眠模式
			   net_status = NET_DOWN;
        rt_kprintf("module enter sleep...\r\n");
    }

}



//发送数据到 平台
uint8_t iot_send_data(void * data)
{
	    char send_buff[256]={0};
			uint8_t recvbuff[256]={0};
		
			static  	AepString Sensor_Data={0};
	    //AT+NMGS
	    HTS221_Data_TypeDef *sensor_data=(HTS221_Data_TypeDef *)data;
	
	    sensor_data_report reort_data;

	   if(net_status==NET_DOWN)
		 {
			 rt_kprintf("connect iot is not ready\r\n");
			 return 0;
		 }
		 reort_data.humility=sensor_data->Humidity;
		 reort_data.temperature=sensor_data->Temperature;
		 
		 Sensor_Data=sensor_data_report_CodeDataReport(reort_data);
		 sprintf(send_buff,"AT+NMGS=%d,%s",Sensor_Data.len/2,Sensor_Data.str);
		 
		 
		 //rt_kprintf("%s\r\n",send_buff);
		 
		  if (AT_RESPONSE_OK == at_send_cmd((uint8_t *)send_buff, recvbuff, sizeof(recvbuff), 300))
			{
				rt_kprintf("send data to iot success\r\n");
			}
	
}






