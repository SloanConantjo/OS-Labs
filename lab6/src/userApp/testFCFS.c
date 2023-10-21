#define WHITE 0x7
extern void schedule(int msg);
extern int createTsk(void (*tskBody)(void));
extern int myPrintf(int color,const char *format, ...);

char *message1 = "********************************\n";

void FCFSTsk0(void){      
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk0: HELLO WORLD!       *\n");
	myPrintf(WHITE, "*     Prority:40               *\n");
	myPrintf(WHITE, message1);
	
	schedule(0);   //the task is end
}

void FCFSTsk1(void){
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk1: HELLO WORLD!       *\n");
	myPrintf(WHITE, "*     Prority:20               *\n");
	myPrintf(WHITE, message1);
	
	schedule(0);   //the task is end
}

void FCFSTsk2(void){
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk2: HELLO WORLD!       *\n");
	myPrintf(WHITE, "*     Prority:10               *\n");
	myPrintf(WHITE, message1);
	
	schedule(0);  //the task is end
}

void FCFSTsk3(void){
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk3: HELLO WORLD!       *\n");
	myPrintf(WHITE, "*     Prority:30               *\n");
	myPrintf(WHITE, message1);
	
	schedule(0);  //the task is end
}
