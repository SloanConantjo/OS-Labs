#include "../myOS/userInterface.h"   //interface from kernel
#include "shell.h"
#include "memTestCase.h"

void FCFSTsk0(void);
void FCFSTsk1(void);
void FCFSTsk2(void);
void FCFSTsk3(void);

void NPPrioTsk0(void);
void NPPrioTsk1(void);
void NPPrioTsk2(void);
void NPPrioTsk3(void);

void RRTsk0(void);
void RRTsk1(void);
void RRTsk2(void);
void RRTsk3(void);

void startShell(void);
void initShell(void);
void memTestCaseInit(void);
int createTsk(void (*tskBody)(void), int prority);
void schedule(int msg);

void wallClock_hook_main(void){
	int _h, _m, _s;
	char hhmmss[]="hh:mm:ss\0\0\0\0";

	getWallClock(&_h,&_m,&_s);
	sprintf(hhmmss,"%02d:%02d:%02d",_h,_m,_s);
	put_chars(hhmmss,0x7E,24,72);
}

void doSomeTestBefore(void){		
	setWallClock(18,59,59);		//set time 18:59:59
    	setWallClockHook(&wallClock_hook_main);
}

// init task 入口
void myMain(void) {
     clear_screen();

     doSomeTestBefore();

     if (currSch.arrangeModel == 0) {//测试FCFS
          myPrintf(0x07, "********************************\n");
          myPrintf(0x07, "*         INIT   INIT !        *\n");
          myPrintf(0x07, "*I create 4 processes:         *\n");
          myPrintf(0x07, "*          Tsk 0,1,2,3 in order*\n");
          myPrintf(0x07, "********************************\n");

          //创建后续进程
          createTsk(FCFSTsk0, 40);
          createTsk(FCFSTsk1, 20);
          createTsk(FCFSTsk2, 10);
          createTsk(FCFSTsk3, 30);

          initShell();
          memTestCaseInit();
          createTsk(startShell, 100);

          //0:进程终止时调度
          schedule(0);
     }
     else if (currSch.arrangeModel == 1) {
          myPrintf(0x07, "********************************\n");
          myPrintf(0x07, "*      INIT : Prority=0        *\n");
          myPrintf(0x07, "*I create 4 processes:         *\n");
          myPrintf(0x07, "*          Tsk 0,1,2,3 in order*\n");
          myPrintf(0x07, "********************************\n");

          //创建后续进程
          createTsk(NPPrioTsk0, 40);
          createTsk(NPPrioTsk1, 20);
          createTsk(NPPrioTsk2, 10);
          createTsk(NPPrioTsk3, 30);

          initShell();
          memTestCaseInit();
          createTsk(startShell, 100);
          
          //0:进程终止时调度
          schedule(0);
     }
     else if (currSch.arrangeModel == 2) {
          //创建后续进程
          createTsk(RRTsk0, 40);
          createTsk(RRTsk1, 20);
          createTsk(RRTsk2, 10);
          createTsk(RRTsk3, 30);
          
          //0:进程终止时调度
          schedule(0);
     }
}

void user_schedule_hook() {
     if (currentTsk->prority == 40) myPrintf(0x2, "Tast0 get time slice.\n");
     else if (currentTsk->prority == 20) myPrintf(0x2, "Tast1 get time slice.\n");
     else if (currentTsk->prority == 10) myPrintf(0x2, "Tast2 get time slice.\n");
     else if (currentTsk->prority == 30) myPrintf(0x2, "Tast3 get time slice.\n");
}
