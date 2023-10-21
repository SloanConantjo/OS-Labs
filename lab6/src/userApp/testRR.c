#define WHITE 0x7
extern void schedule(int msg);
extern int createTsk(void (*tskBody)(void));
extern int myPrintf(int color,const char *format, ...);


void RRTsk0(void){      
    int j;
    for (int i = 0;i <= 10000000;i++) {
        j = i * (i - 1) * (i + 1) - (i * i - i + 1) * i;
        if (i % 5000000 == 0)
            myPrintf(WHITE, "       Task0(Priority=40) counts to %d\n", i);
    }
    myPrintf(WHITE, "       Task0 finishes.\n");
	schedule(0);   
}

void RRTsk1(void){
	int j;
    for (int i = 0;i <= 20000000;i++) {
        j = i * (i - 1) * (i + 1) - (i * i - i + 1) * i;
        if (i % 5000000 == 0)
            myPrintf(WHITE, "       Task1(Priority=20) counts to %d\n", i);
    }
    myPrintf(WHITE, "       Task1 finishes.\n");
	schedule(0);   
}

void RRTsk2(void){
	int j;
    for (int i = 0;i <= 10000000;i++) {
        j = i * (i - 1) * (i + 1) - (i * i - i + 1) * i;
        if (i % 5000000 == 0)
            myPrintf(WHITE, "       Task2(Priority=10) counts to %d\n", i);
    }
    myPrintf(WHITE, "       Task2 finishes.\n");
	schedule(0);     
}

void RRTsk3(void){
	int j;
    for (int i = 0;i <= 10000000;i++) {
        j = i * (i - 1) * (i + 1) - (i * i - i + 1) * i;
        if (i % 5000000 == 0)
            myPrintf(WHITE, "       Task3(Priority=30) counts to %d\n", i);
    }  
    myPrintf(WHITE, "       Task3 finishes.\n");
	schedule(0);   
}
