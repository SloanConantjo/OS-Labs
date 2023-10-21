#include "myPrintk.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"

void myMain(void);
void startShell(void);

void osStart(void)
{
    init8253();     //配置i8253:时钟源
    init8259A();    //配置i8259A:中断控制器
	tick();         //初始化时间
	enable_interrupt();//打开中断
	
    clear_screen();
    myPrintk(0x2, "START RUNNING......\n");
    myMain();
    myPrintk(0x2, "STOP RUNNING......ShutDown\n");
    while (1);
}
