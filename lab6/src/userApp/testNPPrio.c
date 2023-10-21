#define WHITE 0x7
extern void schedule(int msg);
extern int createTsk(void (*tskBody)(void));
extern int myPrintf(int color,const char *format, ...);

char *message2 = "********************************\n";

void NPPrioTsk0(void){      
	myPrintf(WHITE, message2);
	myPrintf(WHITE, "*      Tsk0 : Prority=40      *\n");
	myPrintf(WHITE, message2);
	
	schedule(0);   //the task is end
}

void NPPrioTsk1(void){
	myPrintf(WHITE, message2);
	myPrintf(WHITE, "*      Tsk1 : Prority=20      *\n");
	myPrintf(WHITE, message2);
	
	schedule(0);   //the task is end
}

void NPPrioTsk2(void){
	myPrintf(WHITE, message2);
	myPrintf(WHITE, "*      Tsk2 : Prority=10       *\n");
	myPrintf(WHITE, message2);
	
	schedule(0);  //the task is end
}

void NPPrioTsk3(void){
	myPrintf(WHITE, message2);
	myPrintf(WHITE, "*      Tsk3 : Prority=30       *\n");
	myPrintf(WHITE, message2);
	
	schedule(0);  //the task is end
}
