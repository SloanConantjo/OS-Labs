#include "include/i8253.h"
#include "include/i8259.h"
#include "include/irq.h"
#include "include/uart.h"
#include "include/vga.h"
#include "include/mem.h"
#include "include/task.h"
#include "include/myPrintk.h"

void osStart(void){
     //pressAnyKeyToStart(); // prepare for uart device
     init8259A();
     init8253();
     enable_interrupt();
     
     clear_screen();

     //初始化内存和任务管理

	pMemInit();  //after this, we can use kmalloc/kfree and malloc/free

	{
		unsigned long tmp = dPartitionAlloc(pMemHandler,100);
		dPartitionWalkByAddr(pMemHandler);
		dPartitionFree(pMemHandler,tmp);
		dPartitionWalkByAddr(pMemHandler);
	}

     //选择调度模式
     myPrintk(0x2, "Please select arrange model now.\n");
     char ch = uart_get_char();
     uart_put_char(ch); uart_put_char('\n');
     int model = ch - 48;
     if (model == 0)
          myPrintk(0x2, "!!You select FCFS!!\n");
     else if (model == 1)
          myPrintk(0x2, "!!You select Non-preemptive prority!!\n");
     else if (model == 2)
          myPrintk(0x2, "!!You select RR with FCFS!!\n");
     
     TaskManagerInit(model);

     while(1);
}
