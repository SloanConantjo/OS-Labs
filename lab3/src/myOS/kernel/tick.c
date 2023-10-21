/*i8253时钟中断信号的处理程序会调用tick()--->tick()会定时地输出时钟*/
#include "wallClock.h"

int system_ticks;	//记录 tick 的调用次数(初始为0)
int HH,MM,SS;		//分布代表当前时间的"时：分：秒"

//进行时钟的输出
void tick(void) {
	if (system_ticks == 0) {	//初始化00:00:00(第一次调用)
		HH = MM = SS = 0;
		setWallClock(HH, MM, SS);
	}
	else if (system_ticks % 20 == 0) {	//20个周期时时间增加1s
		SS++;
		if (SS == 60) MM++, SS = 0;
		if (MM == 60) HH++, MM = 0;
		setWallClock(HH, MM, SS);
	}
	system_ticks++;
	return;
}
