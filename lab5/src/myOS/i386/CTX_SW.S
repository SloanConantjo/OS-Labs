.text
.code32 

#上下文切换
#上下文示意图：
#(高地址)CS:IP
#(低地址)
  .global CTX_SW
CTX_SW:
	pushf # 旧进程的标志寄存器入栈
	pusha # 旧进程的通用寄存器入栈，此条指令和上一条指令一并，起到了保护现场的作用

	movl prevTSK_StackPtr, %eax # prevTSK_StackPtrAddr是指针的指针，此行指将其存入 eax 寄存器
	movl %esp, (%eax) # () 是访存的标志，该语句的目的是存储任务的栈空间
	movl nextTSK_StackPtr, %esp # 该语句的目的是通过改变esp来切换栈
	
	popa # 新进程的通用寄存器出栈
	popf # 新进程的标志寄存器出栈
	
	ret  # 返回指令，从栈中取出返回地址，存入 eip 寄存器
