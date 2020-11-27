#ifndef __IOT_THREAD_H_
#define __IOT_THREAD_H_

#include "rtthread.h"




void iot_creat_user_task(void);

void recv_data_deal_entry(void * para);

unsigned char iot_send_data(void * data);

//void recv_urc_data_deal(rt_uint8_t *data);

//void iot_check_net_status_entry(void *para);




#endif




