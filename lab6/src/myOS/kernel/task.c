#include "../include/task.h"
#include "../include/myPrintk.h"

void schedule(int msg);
void destroyTsk(int takIndex);
extern void user_schedule_hook();

extern void (*tick_hook)(int msg);

#define TSK_RDY 0        //表示当前进程已经进入就绪队列中
#define TSK_WAIT -1      //表示当前进程还未进入就绪队列中
#define TSK_RUNING 1     //表示当前进程正在运行
#define TSK_NONE 2       //表示进程池中的TCB为空未进行分配

//tskIdleBdy进程
void tskIdleBdy(void) {
     while(1){
          schedule(0);
     }
}

//tskEmpty进程
void tskEmpty(void){
}

//就绪队列的结构体
typedef struct rdyQueueFCFS{
     myTCB * head;
     myTCB * tail;
} rdyQueueFCFS;

rdyQueueFCFS rqFCFS;

//初始化就绪队列rqFCFS   (需要填写)
void rqFCFSInit(void) {
     rqFCFS.head = rqFCFS.tail = (myTCB*)0;
}

//如果就绪队列为空，返回1
int rqFCFSIsEmpty(void) {//当head和tail均为NULL时，rqFCFS为空
     if (!rqFCFS.head && !rqFCFS.tail) return 1;
     else return 0;
}

//获取就绪队列的头结点信息，并返回
myTCB * nextFCFSTsk(void) {//获取下一个Tsk
     return rqFCFS.head;
}

//获取就绪队列中优先级最高的节点
myTCB* nextProrityTsk(void) {
     //就绪队列为空：return NULL
     if (rqFCFS.head == (void*)0) return (void*)0;
     //就绪队列不空
     int proMax = 10000; //最高优先级-最小prority值
     myTCB* tcbMax = (void*)0;
     myTCB* tcb_ptr = rqFCFS.head;
     while (tcb_ptr) {
          if (proMax > tcb_ptr->prority) {
               proMax = tcb_ptr->prority;
               tcbMax = tcb_ptr;
          }
          tcb_ptr = tcb_ptr->nextTCB;
     }
     return tcbMax;
}

//将进程TCB加入到就绪队列末尾
void tskEnqueueFCFS(myTCB* tsk) {//将tsk入队rqFCFS
     //就绪队列rqFCFS为空
     if (rqFCFSIsEmpty())
          rqFCFS.head = rqFCFS.tail = tsk;
     //非空
     else {
          rqFCFS.tail->nextTCB = tsk;
          rqFCFS.tail = tsk;
     }
     tsk->nextTCB = (void*)0;
}

//将就绪队列中的TCB移除
void tskDequeue(myTCB* tsk) {//rqFCFS出队
     myTCB* tcb_ptr = rqFCFS.head;
     if (tcb_ptr == tsk) {
          rqFCFS.head = tsk->nextTCB;
          if (rqFCFS.tail == tsk) rqFCFS.tail = tsk->nextTCB;//就绪队列中只有一个TCB
          return;
     }
     while (tcb_ptr->nextTCB != tsk) tcb_ptr = tcb_ptr->nextTCB;
     tcb_ptr->nextTCB = tsk->nextTCB;
     if (rqFCFS.tail == tsk) rqFCFS.tail = tcb_ptr;
}

//初始化栈空间
void stack_init(unsigned long **stk, void (*task)(void)){
     *(*stk)-- = (unsigned long) 0x08;       //高地址
     *(*stk)-- = (unsigned long) task;       //EIP
     *(*stk)-- = (unsigned long) 0x0202;     //FLAG寄存器

     *(*stk)-- = (unsigned long) 0xAAAAAAAA; //EAX
     *(*stk)-- = (unsigned long) 0xCCCCCCCC; //ECX
     *(*stk)-- = (unsigned long) 0xDDDDDDDD; //EDX
     *(*stk)-- = (unsigned long) 0xBBBBBBBB; //EBX

     *(*stk)-- = (unsigned long) 0x44444444; //ESP
     *(*stk)-- = (unsigned long) 0x55555555; //EBP
     *(*stk)-- = (unsigned long) 0x66666666; //ESI
     *(*stk)   = (unsigned long) 0x77777777; //EDI
}


//以tskBody为参数在进程池中创建一个进程，将其加入就绪队列rqFCFS中并修改状态
int createTsk(void (*tskBody)(void), int prority) {
     if (!firstFreeTsk) {
          myPrintk(0x04, "Too many tasks!");
          return -1;
     }
     //取出空闲TCB,维护空闲TCB链表
     myTCB* newTsk = firstFreeTsk;
     firstFreeTsk = newTsk->nextTCB;
     //根据函数tskBody初始化TCB
     stack_init(&(newTsk->stkTop), tskBody);
     newTsk->TSK_State = TSK_WAIT;
     newTsk->task_entrance = tskBody;
     newTsk->nextTCB = (myTCB*)0;
     newTsk->prority = prority;
     //将新进程TCB加到就绪队列末尾
     tskEnqueueFCFS(newTsk);
     //设置新进程状态
     newTsk->TSK_State = TSK_RDY;

     return 0;
}

//以takIndex为关键字,在进程池中寻找并销毁takIndex对应的进程
//加入空闲TCB链表
void destroyTsk(int takIndex) {
     myTCB* delTsk = &tcbPool[takIndex];
     //销毁进程
     delTsk->stkTop = delTsk->stack + STACK_SIZE - 1;
     delTsk->TSK_State = TSK_NONE;
     delTsk->task_entrance = tskEmpty;
     //加入空闲TCB链表中
     delTsk->nextTCB = firstFreeTsk;
     firstFreeTsk = delTsk;
}

unsigned long **prevTSK_StackPtr;
unsigned long *nextTSK_StackPtr;

//切换上下文
void context_switch(myTCB *prevTsk) {
     prevTSK_StackPtr = &(prevTsk->stkTop);
     nextTSK_StackPtr = currentTsk->stkTop;
     currSch.params[1] = 0;//RR:slice count置为0
     CTX_SW(prevTSK_StackPtr, nextTSK_StackPtr);
}

//FCFS调度算法
void scheduleFCFS(void) {
     myTCB* prevTsk = currentTsk;
     destroyTsk(prevTsk->TSK_ID);//old进程TCB加入空闲TCB链表
     
     currentTsk = nextFCFSTsk();
     tskDequeue(currentTsk);//当前进程离开就绪队列，准备进入CPU
     
     context_switch(prevTsk);//上下文切换
}

//非抢占式优先级调度算法
void schedulePrority(void) {
     myTCB* prevTsk = currentTsk;
     destroyTsk(prevTsk->TSK_ID);//old进程TCB加入空闲TCB链表

     currentTsk = nextProrityTsk();
     tskDequeue(currentTsk);//当前进程离开就绪队列，准备进入CPU
     
     context_switch(prevTsk);//上下文切换
}

//使用FCFS的RR调度算法
void scheduleRR(int msg) {
     if (rqFCFSIsEmpty() == 1) {//就绪队列为空
          if (msg == 1) return;
          else {
               while (1);
          }
     }

     myTCB* prevTsk = currentTsk;
     currentTsk = nextFCFSTsk();
     
     tskDequeue(currentTsk);//当前进程离开就绪队列，准备进入CPU

     if (msg == 1) tskEnqueueFCFS(prevTsk);
     else destroyTsk(prevTsk->TSK_ID);
     //Just For Test
     user_schedule_hook();
     
     context_switch(prevTsk);//上下文切换
}

//调度算法
void schedule(int msg) {
     if (currSch.arrangeModel == 0)//FCFS:一定是进程终止时调度
          scheduleFCFS();
     else if (currSch.arrangeModel == 1)//非抢占式优先级:一定是进程终止时调度
          schedulePrority();
     else if (currSch.arrangeModel == 2) {
          if (currSch.params[1] < 0) return;
          if (msg == 1) {//tick触发
               currSch.params[1]++;
               // myPrintk(0x2, "++%d\n", currSch.params[1]);
               if (currSch.params[1] > currSch.params[0]) scheduleRR(1);
          }
          else {//进程终止触发
               currSch.params[1] = -1;//tick触发schedule时不进行调度
               scheduleRR(0);
          }
     }
}

//设置调度器
void setScheduler(int model) {
     if (model >= 0 && model <= 3)
          currSch.arrangeModel = model;
     if (model == 0) {//FCFS
          currSch.nextTsk_func = nextFCFSTsk;
          currSch.tick_hook = (void*)0;
     }
     else if (model == 1) {//Non-Preem Priority
          currSch.nextTsk_func = nextProrityTsk;
          currSch.tick_hook = (void*)0;
     }
     else if (model == 2) {//RR with FCFS
          currSch.nextTsk_func = nextFCFSTsk;
          currSch.tick_hook = schedule;
          currSch.params[0] = 10;//10个tick周期=100ms
          currSch.params[1] = -1;//负数表示没有进程正在CPU上运行
     }
     //设置tick_hook
     tick_hook = currSch.tick_hook;
}


unsigned long BspContextBase[STACK_SIZE];//一个全局栈
unsigned long* BspContext;//全局栈栈底指针
//进入多任务调度模式
void startMultitask(void) {
     BspContext = BspContextBase + STACK_SIZE -1;
     prevTSK_StackPtr = &BspContext;
     
     currentTsk = currSch.nextTsk_func();
     tskDequeue(currentTsk);

     nextTSK_StackPtr = currentTsk->stkTop;

     currSch.params[1] = 0;//slice count归0
     
     CTX_SW(prevTSK_StackPtr, nextTSK_StackPtr);
}

//准备进入多任务调度模式
void TaskManagerInit(int model) {
     // 初始化进程池(所有的进程状态都是TSK_NONE)
     int i;
     myTCB * thisTCB;
     for(i=0;i<TASK_NUM;i++){//对进程池tcbPool中的进程进行初始化处理
          thisTCB = &tcbPool[i];
          thisTCB->TSK_ID = i;
          thisTCB->stkTop = thisTCB->stack+STACK_SIZE-1;//将栈顶指针复位
          thisTCB->TSK_State = TSK_NONE;//表示该进程池未分配，可用
          thisTCB->task_entrance = tskEmpty;
          if(i==TASK_NUM-1){
               thisTCB->nextTCB = (void *)0;
          }
          else{
               thisTCB->nextTCB = &tcbPool[i+1];
          }
     }
     //创建idle任务
     idleTsk = &tcbPool[0];
     stack_init(&(idleTsk->stkTop),tskIdleBdy);
     idleTsk->task_entrance = tskIdleBdy;
     idleTsk->nextTCB = (void *)0;
     idleTsk->TSK_State = TSK_RDY;
     idleTsk->prority = -1;

     rqFCFSInit();//初始化就绪队列

     firstFreeTsk = &tcbPool[1];//更新空闲链表

     //创建init任务
     createTsk(initTskBody, 0);//init进程具有最高优先级

     //设置调度模式
     setScheduler(model);

     //进入多任务状态
     myPrintk(0x2, "START MULTITASKING......\n");
     startMultitask();
     myPrintk(0x2,"STOP MULTITASKING......SHUT DOWN\n");
}