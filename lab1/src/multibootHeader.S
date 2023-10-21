.globl start 

/*定义multiboot header必要字段的取值*/
MULTIBOOT_HEADER_MAGIC=0x1BADB002
MULTIBOOT_HEADER_FLAGS=0x00000000
MULTIBOOT_HEADER_CHECKSUM=-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.section ".xxx" #先起一个section的名字
.align 4    # 必须4字节对齐

/*定义必须的multiboot header字段*/
.long   MULTIBOOT_HEADER_MAGIC        # magic
.long   MULTIBOOT_HEADER_FLAGS        # flags
.long   MULTIBOOT_HEADER_CHECKSUM     # checksum


.text       # 进入代码段
.code32     # 32位代码
start:      # 与第一行的声明一致

/*屏幕输出本人学号-姓名缩写*/
movl $0x71427150, 0xB8000   # PB
movl $0x71307132, 0xB8004   # 20 
movl $0x71317131, 0xB8008   # 11
movl $0x71367131, 0xB800C   # 16
movl $0x71357135, 0xB8010   # 55
movl $0x7157712D, 0xB8014   # -L
movl $0x714C714A, 0xB8018   # WJ
movl $0x71757169, 0xB801C   # iu


/*串口输出本人名字缩写*/
movw $0x3F8, %dx    # 设置UART输出端口
movb $0x4C, %al     
out %al, %dx        # 输出L
movb $0x57, %al     
out %al, %dx        # 输出W
movb $0x4A, %al
out %al, %dx        # 输出J
movb $0x0A, %al
out %al, %dx        # 输出换行符

/*停机*/
hlt