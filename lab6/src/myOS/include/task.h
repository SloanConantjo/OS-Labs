#ifndef __TASK_H__
#define __TASK_H__

#ifndef USER_TASK_NUM
#include "../../userApp/userApp.h"
#endif

#define TASK_NUM (2 + USER_TASK_NUM)   // at least: 0-idle, 1-init

#define initTskBody myMain         // connect initTask with myMain

#define STACK_SIZE 512            // definition of STACK_SIZE

void initTskBody(void);

void CTX_SW(void*prev_stkTop, void*next_stkTop);

//为 myTCB 增补合适的字段
typedef struct myTCB {
     unsigned long *stkTop;        /* 栈顶指针 */
     unsigned long stack[STACK_SIZE];      /* 开辟了一个大小为STACK_SIZE的栈空间 */  
     unsigned long TSK_State;   /* 进程状态 */
     unsigned long TSK_ID;      /* 进程ID */ 
     void (*task_entrance)(void);  /*进程的入口地址*/
     struct myTCB* nextTCB;           /*下一个TCB*/
     int prority;   /*各异的优先级，值越小优先级越高*/
} myTCB;

myTCB tcbPool[TASK_NUM];//进程池的大小设置

int arrangeModel;    
myTCB* idleTsk;                /* idle 任务 */
myTCB* currentTsk;             /* 当前任务 */
myTCB* firstFreeTsk;           /* 下一个空闲的 TCB */


typedef struct scheduler {
     /*调度模式：
          0-FCFS，1-非抢占Priority，2-(FCFS式的)RR
     */
     int arrangeModel;
     myTCB* (*nextTsk_func)(void);
     int params[2];//RR模式下分别为时间片长度和当前时间片已经过的长度
     // void (*enqueueTsk_func)(myTCB* tsk);
     // void (*dequeueTsk_func)(myTCB* tsk);
     
     // void (*schedulerInit_func)(myTCB* idleTsk);
     // int (*createTsk_hook)(void (*tskBody)(void), tskPara para);
     void (*tick_hook)(int); //if set, tick_hook will be called every tick
}scheduler;

scheduler currSch;//当前调度器

void TaskManagerInit(int model);

#endif
