/*实现shell程序
  通过UART串口输入指令,在shell窗口和QEMU窗口都需要回显,都需要显示结果*/

#include "io.h"
#include "myPrintk.h"
#include "uart.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"
#include "wallClock.h"


typedef struct myCommand {
    char name[80];  //命令名(可以作为唯一标识符使用)
    char help_content[200]; //该命令的使用说明
    int (*func)(int argc, char(*argv)[8]); //函数指针的概念，用于指向该命令的处理函数
    //你可以添加自定义的命令信息
}myCommand;

myCommand commands[64];//所有命令
int N;//命令总数目

//字符串比较(忽略字母大小写):0--相同;1--a>b;-1--a<b
int stringCmp(char* a, char* b) {
    for (;*a != '\0' && *b != '\0';a++, b++) {
        if (*a == *b) continue;
        else if (*a > *b) {
            if (*a - *b == 32 && *a >= 97 && *a <= 122) continue;
            else return 1;
        }
        else {
            if (*b - *a == 32 && *b >= 97 && *b <= 122) continue;
            else return -1;
        }
    }
    if (*a == *b) return 0;
    else if (*a != '\0') return 1;
    else return -1;
}

//cmd命令对应函数
int func_cmd(int argc, char(*argv)[8]) {
    myPrintf(0x2, "list all commands:\n");
    for (int i = 0;i < N;i++) 
        myPrintf(0x02, "%s\n", commands[i].name);
    return 1;
}

//help指令对应的函数
int func_help(int argc, char(*argv)[8]) {
    if (argc == 1) {    //help
        myPrintf(0x2, "%s", commands[1].help_content);
        return 1;
    }
    //检测参数
    int j;
    for (j = 0;j < N;j++)  //逐个检测
        if (stringCmp(commands[j].name, argv[1]) == 0) break;
    if (j == N) return 0;   //未定义指令
    myPrintf(0x2, "%s", commands[j].help_content);
    return 1;
}

//初始化指令数组
void initCommands(void) {
    //cmd
    {
        myCommand cmd = { "cmd\0","List all commands\n\0",func_cmd };
        commands[0] = cmd;
        N++;
    }

    //help
    {
        myCommand help = { "help\0","Usage: help [command]\nDisplay info about [command]\n\0",func_help };
        commands[1] = help;
        N++;
    }
}

//shell程序的主函数
void startShell(void) {
    initCommands();
    
    char BUF[256];  //输入缓存区
    int BUF_len=0;	//输入缓存区的长度
    
	int argc;
    char argv[8][8];

    do{
        BUF_len=0; 
        myPrintk(0x07, "WJL>>\0");
        //读取命令行输入至BUF缓冲区,并同时通过VGA和UART回显
        while ((BUF[BUF_len] = uart_get_char()) != '\r') {
            myPrintf(0x7, "%c", BUF[BUF_len]);
            BUF_len++;  //BUF数组的长度加1
        }
        uart_put_chars(" -pseudo_terminal\0");
        myPrintf(0x7, "\n");

        //将BUF缓冲区中的字符串解析为单词存入argv
        argc = 0;
        int wlen = 0;
        for (int i = 0;i <= BUF_len;i++) {
            if (BUF[i] == ' ' || BUF[i] == '\r') {    //遇到空格或行尾
                if (wlen == 0) continue;
                else {  //终止某个单词
                    argv[argc][wlen] = '\0';
                    argc++, wlen = 0;
                }
            }
            else {
                argv[argc][wlen] = BUF[i];
                wlen++;
            }
        }

        //从各个单词解析出指令并执行
        if (argc == 0) continue;    //没有指令
        else {
            int num;
            for (num = 0;num < N;num++) //匹配指令名
                if (stringCmp(commands[num].name, argv[0]) == 0) break;
            if (num == N)  //未定义指令
                myPrintf(0x02, "The cmd is not defined\n");
            else if (commands[num].func(argc, argv) == 0) 
                myPrintf(0x02, "Wrong arguments.\n");
        }
    }while(1);
}