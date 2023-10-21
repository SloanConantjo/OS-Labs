//内存大小检测;调用相应算法初始化内存空间

#include "../../include/myPrintk.h"
#include "../../include/mem.h"

unsigned long pMemStart;  // 可用的内存的起始地址
unsigned long pMemSize;   // 可用的大小

//检测内存大小
//设置起始地址pMemStart和大小pMemSize
void memTest(unsigned long start, unsigned long grainSize) {
	if (start < 0x100000) start = 0x100000;
	if (grainSize < 4) grainSize = 4;
	unsigned char ch[2];
	unsigned long end = start;
	while (1) {
		unsigned char* base = (unsigned char*)end;
		ch[0] = *base; ch[1] = *(base + 1);
		*base = 0x55;*(base + 1) = 0xAA;
		if (*base != 0x55 || *(base + 1) != 0xAA) break;
		*base = 0xAA;*(base + 1) = 0x55;
		if (*base != 0xAA || *(base + 1) != 0x55) break;
		*base = ch[0]; *(base + 1) = ch[1];

		base = (unsigned char*)(end + grainSize - 2);
		ch[0] = *base; ch[1] = *(base + 1);
		*base = 0x55;*(base + 1) = 0xAA;
		if (*base != 0x55 || *(base + 1) != 0xAA) break;
		*base = 0xAA;*(base + 1) = 0x55;
		if (*base != 0xAA || *(base + 1) != 0x55) break;
		*base = ch[0]; *(base + 1) = ch[1];
		
		end += grainSize;
	}
	pMemStart = start;
	pMemSize = end - start;
	myPrintk(0x7, "MemStart: %x  \n", pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n", pMemSize);
}

extern unsigned long _end;

//内存空间初始化
void pMemInit(void) {
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);	//设置内存空间起始地址,计算大小
	myPrintk(0x7,"_end:     %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {	//减去OS已经使用的部分-->得到可分配内存
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}
	
	//此处选择不同的内存管理算法来进行初始化
	//全局变量pMemHandler等于pMemStart
	pMemHandler = dPartitionInit(pMemStart, pMemSize);
}
