.text
.code32
_start:
	.globl enable_interrupt
enable_interrupt:	# 开中断
	sti
	ret
	
	.globl disable_interrupt
disable_interrupt:	# 关中断
	cli
	ret
