#include <stdlib.h>
#include "bc28_thread.h"
#include "iot_thread.h"
#include "rtthread.h"
#include  "string.h"
#include "com.h"
//#include "user_main.h"


#define MAX_AUTO_REGISTRATION_WAIT_TIME   40000 //40s 自动注册最大等待时间




module_info _bc28_module_info;
module_stats _bc28_module_stats;


rt_sem_t recv_urc_data_sem= RT_NULL;     //收到URC消息 信号量

//thread_info_typedef iot_thread_table={
//{IOT_RECV_DATA_DEAL_THREAD_NAME,iot_recv_data_deal_entry,}
//}; 
//AT+NPSMR=1

extern struct rt_ringbuffer uart_bc28_rxcb;  //模组ringbuff

extern uint8_t  at_response_buff[512];
extern uint8_t  urc_response_buff[512];



void bc28_init_thread_entry(void *para);

static rt_uint8_t check_bc28_auto_registration(rt_uint32_t max_wait_time);



uint8_t bc28_get_module_info(void)  //获取模块信息
{
	
	uint8_t  recvbuff[50]={0},size;
	char *index=0;
	

	rt_thread_mdelay(500); 
	//IMSI
	if(AT_RESPONSE_OK!=at_send_cmd("AT+CIMI",recvbuff,sizeof(recvbuff),300))
	{
	  return RT_FALSE;
	}
	index=strstr((char *)recvbuff,"\r\n");
	
	index+=2;
	
	memcpy(_bc28_module_info.imsi,index,15);
	
	
	//rt_thread_mdelay(500); 
	//IMEI
	if(AT_RESPONSE_OK!=at_send_cmd("AT+CGSN=1",recvbuff,sizeof(recvbuff),300))
	{
	  return RT_FALSE;
	}
	index=strstr((char *)recvbuff,"GSN:");
	
	index+=4;
	
	memcpy(_bc28_module_info.imei,index,15);
	
//	rt_thread_mdelay(500); 
	
	//CCID
	if(AT_RESPONSE_OK!=at_send_cmd("AT+NCCID",recvbuff,sizeof(recvbuff),300))
	{
	  return RT_FALSE;
	}
	index=strstr((char *)recvbuff,"CID:");
	
	index+=4;
	
	memcpy(_bc28_module_info.ccid,index,20);
	
//	rt_thread_mdelay(500); 
	//IP
	if(AT_RESPONSE_OK!=at_send_cmd("AT+CGPADDR",recvbuff,sizeof(recvbuff),300))
	{
	  return RT_FALSE;
	}
	
	index=strstr((char *)recvbuff,"DDR:");
	
	
	index+=4;

	size=strstr(index,"\r\n")-index;
	
	
	
	memcpy(_bc28_module_info.local_ip,index,size);
	
	
//	rt_kprintf("imsi:%s\r\n",_bc28_module_info.imsi);
//	rt_kprintf("ccid:%s\r\n",_bc28_module_info.ccid);
  //	rt_kprintf("imei:%s\r\n",_bc28_module_info.imei);
//	rt_kprintf("ip:%s\r\n",_bc28_module_info.local_ip);
	
	return  RT_TRUE;
}

 
uint8_t bc28_get_module_stats(void)   //获取模块状态
{
	
	uint8_t recvbuff[200]={0};
	
	uint8_t databuff[10]={0};
	
	char * index=0;
	
  if(AT_RESPONSE_OK!=at_send_cmd("AT+NUESTATS",recvbuff,sizeof(recvbuff),300))
	{
	  return RT_FALSE;
	}
	//////////////////////////////////
  index=strstr((char *)recvbuff,"Cell ID:");
	
	index+=8;
	
	memcpy(databuff,index,9);
	
	_bc28_module_stats.cell_id=atoi((char *)databuff);
	
	memset(databuff,0,sizeof(databuff));
	
	//////////////////////////////////
	
	index=strstr((char *)recvbuff,"ECL:");
	
	index+=4;
	
	memcpy(databuff,index,1);
	
	_bc28_module_stats.ecl=atoi((char *)databuff);
	
	memset(databuff,0,sizeof(databuff));
	
	/////////////////////////////
	index=strstr((char *)recvbuff,"SNR:");
	
	index+=4;
	
	memcpy(databuff,index,3);
	
	_bc28_module_stats.snr=atoi((char *)databuff);
	
	memset(databuff,0,sizeof(databuff));
	
	////////////////////////////////////
	index=strstr((char *)recvbuff,"PCI:");
	
	index+=4;
	
	memcpy(databuff,index,3);
	
	_bc28_module_stats.pci=atoi((char *)databuff);
	
	memset(databuff,0,sizeof(databuff));
	
	/////////////////////////////////////////
	
	index=strstr((char *)recvbuff,"RSRQ:");
	
	index+=5;
	
	memcpy(databuff,index,4);
	
	_bc28_module_stats.prsq=atoi((char *)databuff);
	
	memset(databuff,0,sizeof(databuff));
	
	
	return RT_TRUE;
}




static uint8_t bc28_set_para(void)
{
  uint8_t recvbuff[50]={0};
	
	rt_thread_mdelay(500);
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"ATE1",recvbuff,sizeof(recvbuff),100))  //开启回显
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+NPSMR=1",recvbuff,sizeof(recvbuff),100))  //设置省电模式 变化URC通知
	{
		 return RT_FALSE;
	} 
	/*
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+NCDP=221.229.214.202,5683",at_response_buff,sizeof(at_response_buff),100))  //注册地址
	{
	 return RT_FALSE;
	} 
  rt_thread_mdelay(500);
	
	
		if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CFUN?",at_response_buff,sizeof(at_response_buff),100))  //射频功能
	{
	 return RT_FALSE;
	} 
  rt_thread_mdelay(500);
	
		if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CFUN=1",at_response_buff,sizeof(at_response_buff),100))  //打开射频
	{
	 return RT_FALSE;
	} 
  rt_thread_mdelay(500);
	
		if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CGATT?",at_response_buff,sizeof(at_response_buff),100))  //注册网络查询
	{
	 return RT_FALSE;
	} 
	
  rt_thread_mdelay(500);
	
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CGATT=1",at_response_buff,sizeof(at_response_buff),100))  //激活网络
	{
	 return RT_FALSE;
	} 
  rt_thread_mdelay(500);
	
	
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+QREGSWT=1",at_response_buff,sizeof(at_response_buff),100))  //自动注册
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+QREGSWT?",at_response_buff,sizeof(at_response_buff),100))  //查询是否自动注册
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	
		if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CIMI",at_response_buff,sizeof(at_response_buff),100))  //查询卡号
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	
	if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CGSN",at_response_buff,sizeof(at_response_buff),100))  //查询SN
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	
				if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CGSN=1",at_response_buff,sizeof(at_response_buff),100))  //查询IMEI
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	
 if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CSQ",at_response_buff,sizeof(at_response_buff),100))  //
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	
	 if(AT_RESPONSE_OK!=at_send_cmd((unsigned char *)"AT+CGPADDR",at_response_buff,sizeof(at_response_buff),100))  //
	{
	  return RT_FALSE;
	} 
	rt_thread_mdelay(500);
	*/
	
	
	return RT_TRUE;
	
}

extern NET_STATUS net_status;

void bc28_init_thread_entry(void *para)
{

	
	bc28_set_para();
	
	
	
	//检查自动自动注册
	

	if(check_bc28_auto_registration(MAX_AUTO_REGISTRATION_WAIT_TIME))
	{//自动注册成功
	 rt_memset(urc_response_buff,0,sizeof(urc_response_buff));
	 net_status=NET_UP;

		
				
	 calibration_rtc_from_iot_module();	
		
	 recv_urc_data_sem = rt_sem_create("urc_sem", 0, RT_IPC_FLAG_FIFO);
   if (recv_urc_data_sem == RT_NULL)
    {
        rt_kprintf("create dynamic semaphore failed.\n");
      //  return -1;
    }
		
	 iot_creat_user_task();
	
   return;
	}

	else
	{//自动注册失败
		
			
	  rt_kprintf("auto registration failed...\r\n");
	
		return;

}
}

static rt_uint8_t check_bc28_auto_registration(rt_uint32_t max_wait_time)
{
	//等待模块启动后   自动注册的UCR 报文
	do 
	{
	
			//rt_kprintf("%s\r\n",readbuff);
			if(strstr((char *)urc_response_buff,"+QLWEVTIND:0"))
			{
				rt_kprintf("registration success\r\n");
			}
			if(strstr((char *)urc_response_buff,"+QLWEVTIND:3"))
			{
				rt_kprintf("observed object 19 ok\r\n");
				return RT_TRUE;
			}	
		rt_memset(urc_response_buff,0,sizeof(urc_response_buff));
		rt_thread_mdelay(10);	
		max_wait_time--;
	}
	while(max_wait_time);

	
	return RT_FALSE;
}






