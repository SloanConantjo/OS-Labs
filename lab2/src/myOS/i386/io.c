/* 
 * IO 操作(与端口进行交互)
 * 理解了outb函数后实现inb函数
 * C语言内嵌汇编实现
 */

//从端口port_from获取一个字节
unsigned char inb(unsigned short int port_from) {
    unsigned char value;
    __asm__ __volatile__("inb %w1,%0":"=a"(value) : "Nd"(port_from));
    return value;
}

//将一字节的value发送到端口port_to
void outb(unsigned short int port_to, unsigned char value) {
    __asm__ __volatile__ ("outb %b0,%w1"::"a" (value),"Nd" (port_to));
}