#include "../../include/myPrintk.h"


//dPartition是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp){
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}


// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	union {
		unsigned long nextStart;    // if free: pointer to next block
        unsigned long userData;		// if allocated, belongs to user
	};	                           
} EMB;	//共占8个字节

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB * emb){
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}


void dPartitionWalkByAddr(unsigned long dp){
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按空闲链表的顺序遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息
	*/
	dPartition* dpPtr = (dPartition*)dp;
	showdPartition(dpPtr);
	EMB* embPtr = (EMB*)dpPtr->firstFreeStart;
	while ((unsigned long)embPtr) {
		showEMB(embPtr);
		embPtr = (EMB*)embPtr->nextStart;
	}
}

//动态分区内存的初始化
unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	*/
	if (totalSize <= dPartition_size + EMB_size) {
		myPrintk(0x5, "Total size is too small.");
		return 0;
	}
	//初始化dPartitionInit结构体
	dPartition* dp = (dPartition*)start;
	dp->size = totalSize;
	dp->firstFreeStart = start + dPartition_size;
	//构造第一个空闲分区
	EMB* firstEmb = (EMB*)dp->firstFreeStart;
	firstEmb->nextStart = 0;
	firstEmb->size = totalSize - dPartition_size - EMB_size;
	
	return start;
}


//分配一个分区
unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size) {
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。
	*/
	dPartition* dpPtr = (dPartition*)dp;
	unsigned long* preNext = &(dpPtr->firstFreeStart);
	EMB* emb = (EMB*)dpPtr->firstFreeStart;
	while ((unsigned long)emb) {
		if (emb->size == size) {
			*preNext = emb->nextStart;
			return (unsigned long)emb + EMB_size;
		}
		else if (emb->size < size) {
			preNext = &(emb->nextStart);
			emb = (EMB*)emb->nextStart;
		}
		else {
			EMB* newEmb = (EMB*)((unsigned long)emb + size + EMB_size);
			newEmb->size = emb->size - size - EMB_size;
			newEmb->nextStart = emb->nextStart;
			*preNext = (unsigned long)newEmb;
			emb->size = size;
			return (unsigned long)emb + EMB_size;
		}
	}
	return 0;
}

//释放一个分区
unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并
	*/
	dPartition* dpPtr = (dPartition*)dp;
	EMB* freeEmb = (EMB*)(start - EMB_size);//要释放的分区
	if (dp + dPartition_size + EMB_size > start || start + freeEmb->size > dp + dpPtr->size) {
		myPrintk(0x5, "The block is out of the space.");
		return 0;
	}
	//查看相邻分区是否为空闲分区
	EMB* adjEmb[2] = { (EMB*)0,(EMB*)0 };//前、后相邻分区:不为空时表示free
	//后相邻分区free时,索引后相邻分区在free分区链中前驱的nextstart字段
	unsigned long* prePostNext = &dpPtr->firstFreeStart;//索引free分区链中emb分区前一分区的nextstart字段
	EMB* emb = (EMB*)dpPtr->firstFreeStart;
	while ((unsigned long)emb) {
		//检查前相邻的分区
		if ((unsigned long)emb + EMB_size + emb->size == start - EMB_size) {
			adjEmb[0] = emb;//前相邻分区
			if (adjEmb[1]) break;
		}
		//检查后相邻的分区
		if ((unsigned long)emb == start + freeEmb->size) {
			adjEmb[1] = emb;//后相邻分区
			if (adjEmb[0]) break;
		}
		if (adjEmb[1] == (EMB*)0) prePostNext = &emb->nextStart;
		emb = (EMB*)emb->nextStart;
		//找到free的后相邻分区后,prePostNext索引后相邻分区在free分区链中前驱的nextstart字段,从而保持不变
	}
	if (adjEmb[0] && adjEmb[1]) {	//前、后相邻都是空闲分区
		adjEmb[0]->size += 2 * EMB_size + freeEmb->size + adjEmb[1]->size;
		*prePostNext = adjEmb[1]->nextStart;
	}
	else if (adjEmb[0] && !adjEmb[1]) //前相邻分区free,后相邻分区not free
		adjEmb[0]->size += EMB_size + freeEmb->size;
	else if (!adjEmb[0] && adjEmb[1]) { //前相邻分区not free,后相邻分区free
		*prePostNext = start - EMB_size;
		freeEmb->size += EMB_size + adjEmb[1]->size;
		freeEmb->nextStart = adjEmb[1]->nextStart;
	}
	else {	//前、后相邻都不是空闲分区
		freeEmb->nextStart = dpPtr->firstFreeStart;
		dpPtr->firstFreeStart = start - EMB_size;
	}
	return 1;
}


// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}
