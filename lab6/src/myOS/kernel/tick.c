extern void oneTickUpdateWallClock(void);       //TODO: to be generalized

void (*tick_hook)(int msg) = 0;

int tick_number = 0;
void tick(void){
     tick_number++;	

     oneTickUpdateWallClock();

     if(tick_hook) tick_hook(1);  //schedule:1-time slice out
}
