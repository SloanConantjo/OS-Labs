# 需要填写一处！
# 其余部分无须修改
STACK_SIZE =	0x1000	# Size of heap and stack. 4KB

/* ===== code32 =========================== */

	.globl 	_start		# GNU default entry point
	.globl 	osStart
.text
.code32
_start:
	jmp establish_stack

dead:	jmp dead			# Never here

# Set up the stack
establish_stack:
	movl	$__bss_end, %eax	# eax = end of bss/start of heap 
	addl	$STACK_SIZE, %eax	# make room for stack
	andl	$0xffffffc0, %eax	# align it on 64 byte boundary
	
	movl	%eax, %esp		# set stack pointer
	movl	%eax, %ebp		# set base pointer

# Zero out the BSS segment	(by cld-repne-stosl) 
zero_bss:
	cld				            # direction flag:each stosl adds 4 to edi
	movl	$_end, %ecx		    # find end of .bss
	movl	$_bss_start, %edi	# edi = beginning of .bss
	subl	%edi, %ecx		    # ecx = size of .bss in bytes
	shrl	%ecx			    
	shrl	%ecx				# size of .bss in longs(4 bytes)
	
	xorl	%eax, %eax		    # eax=0 to clear out memory
	repne			            # repeat while ecx != 0 and then ecx--
	stosl				        # clear a long in the bss 
								# stosl:4-Bytes 0 saved in the edi position


# Transfer control to main
to_main:	
	call	osStart

shut_down:	
	jmp	shut_down	# Never here	

/* ===== data =========================== */
