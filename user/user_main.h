#ifndef  __USER_MAIN_H__
#define  __USER_MAIN_H__


#include "rtthread.h"
#include "bc28_thread.h"

typedef struct thread_info
{
	 const char * thread_name;
	 void (*entry)(void *parameter);
	 void       *parameter;
   rt_uint32_t stack_size;
   rt_uint8_t  priority;
   rt_uint32_t tick;	
}thread_info_typedef;



void creat_user_task(void);
void led_task_entry(void * para);

#endif

