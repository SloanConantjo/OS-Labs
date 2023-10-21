#include "../../include/myPrintk.h"


//eFPartition是表示整个(可分配)内存的数据结构,它按照按等大小固定分区策略划分分区
typedef struct eFPartition {
	unsigned long totalN;
	unsigned long perSize;  //unit: byte	
	unsigned long firstFree;//第一个空闲分区的地址
}eFPartition;	//占12个字节

#define eFPartition_size 12

void showeFPartition(struct eFPartition *efp){
	myPrintk(0x5,"eFPartition(start=0x%x, totalN=0x%x, perSize=0x%x, firstFree=0x%x)\n", efp, efp->totalN, efp->perSize, efp->firstFree);
}


//一个EEB表示一个空闲可用的Block;EEB总紧靠在对应block之前
//每个Block的起始地址认为是对应EEB的地址
typedef struct EEB {
	unsigned long next_start;//地址(而非序号)
}EEB;	// 占4个字节

#define EEB_size 4

void showEEB(struct EEB *eeb){
	myPrintk(0x7,"EEB(start=0x%x, next=0x%x)\n", eeb, eeb->next_start);
}


//输出等大小固定分区划分内存的情况
void eFPartitionWalkByAddr(unsigned long efpHandler){
	// TODO
	/*功能：本函数是为了方便查看和调试的。
	1. 打印eFPartiiton结构体的信息，可以调用上面的showeFPartition函数。
	2. 遍历每一个EEB，打印出他们的地址以及下一个EEB的地址（可以调用上面的函数showEEB）
	*/
	eFPartition* efpPtr = (eFPartition*)efpHandler;
	showeFPartition(efpPtr);
	EEB* eebPtr = (EEB*)efpPtr->firstFree;
	while ((unsigned long)eebPtr) {
		showEEB(eebPtr);
		eebPtr = (EEB*)eebPtr->next_start;
	}
}

const int align = 4;
//对齐
unsigned long efpAlign = ((eFPartition_size + align - 1) / align) * align;
unsigned long eebAlign = ((EEB_size + align - 1) / align) * align;


//计算占用空间的实际大小
//n个区块, 每个区块大小不小于perSize
unsigned long eFPartitionTotalSize(unsigned long perSize, unsigned long n) {
	// TODO
	/*功能：计算占用空间的实际大小，并将这个结果返回
	1. 根据参数persize（每个大小）和n个数计算总大小，注意persize的对齐。
		例如persize是31字节，你想8字节对齐，那么计算大小实际代入的一个块的大小就是32字节。
	2. 同时还需要注意“隔离带”EEB的存在也会占用4字节的空间。
		typedef struct EEB {
			unsigned long next_start;
		}EEB;	
	3. 最后别忘记加上eFPartition这个数据结构的大小，因为它也占一定的空间。
	*/
	perSize = ((perSize + align - 1) / align) * align;
	return efpAlign + n * (eebAlign + perSize);
}

// 按照按等大小固定分区策略初始化内存
unsigned long eFPartitionInit(unsigned long start, unsigned long perSize, unsigned long n) {
	// TODO
	/*功能：初始化内存
	1. 需要创建一个eFPartition结构体，需要注意的是结构体的perSize不是直接传入的参数perSize，需要对齐。结构体的next_start也需要考虑一下其本身的大小。
	2. 就是先把首地址start开始的一部分空间作为存储eFPartition类型的空间
	3. 然后再对除去eFPartition存储空间后的剩余空间开辟若干连续的空闲内存块，将他们连起来构成一个链。注意最后一块的EEB的nextstart应该是0
	4. 需要返回一个句柄，也即返回eFPartition *类型的数据
	注意的地方：
		1.EEB类型的数据的存在本身就占用了一定的空间。
	*/
	//初始化eFPartition结构体
	eFPartition efp;
	perSize=((perSize + align - 1) / align) * align;
	efp.totalN = n; efp.perSize = perSize;
	efp.firstFree = start + efpAlign;
	*((eFPartition*)start) = efp;
	//构造空闲内存区块
	unsigned long pstart = efp.firstFree;
	EEB eeb;
	for (int i = 0;i < n - 1;i++) {
		eeb.next_start = pstart + eebAlign + perSize;
		*((EEB*)pstart) = eeb;
		pstart = eeb.next_start;
	}
	eeb.next_start = 0;
	*((EEB*)pstart) = eeb;
	return start;
}

//分配一个分区
unsigned long eFPartitionAlloc(unsigned long EFPHandler){
	// TODO
	/*功能：分配一个空间
	1. 本函数分配一个空闲块的内存并返回相应的地址，EFPHandler表示整个内存的首地址
	2. 事实上EFPHandler就是我们的句柄，EFPHandler作为eFPartition *类型的数据，其存放了我们需要的firstFree数据信息
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EEB类型的数据的存在本身就占用了一定的空间。
	*/
	unsigned long freeEEB = ((eFPartition*)EFPHandler)->firstFree;
	if (freeEEB == 0) {//无空闲区块
		myPrintk(0x5, "ERROR:No free block.");
		return 0;
	}
	((eFPartition*)EFPHandler)->firstFree = ((EEB*)freeEEB)->next_start;//维护空闲block链表
	return freeEEB + eebAlign;
}

//释放一个分区
unsigned long eFPartitionFree(unsigned long EFPHandler,unsigned long mbStart){
	// TODO
	/*功能：释放一个空间
	1. mbstart将成为第一个空闲块，EFPHandler的firstFree属性也需要相应的更新。
	2. 同时我们也需要更新维护空闲内存块组成的链表。
	*/
	eFPartition* efpPtr = (eFPartition*)EFPHandler;
	if ((mbStart - eebAlign - EFPHandler - efpAlign) % (eebAlign + efpPtr->perSize)) {//不合适的地址
		myPrintk(0x5, "Address is not the start of a block.");
		return 0;
	}
	//维护空闲block链表
	EEB eeb = { efpPtr->firstFree };
	efpPtr->firstFree = mbStart - eebAlign;
	*((EEB*)efpPtr->firstFree) = eeb;
	return efpPtr->firstFree;
}
