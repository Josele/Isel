#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <wiringPi.h>
#include "fsm.h"
#include <stdio.h>


#define GPIO_BUTTON	2
#define GPIO_LED	8
#define GPIO_CUP	4
#define GPIO_COFFEE	5
#define GPIO_MILK	6
#define GPIO_IN1    7
#define GPIO_IN2    2
#define GPIO_IN3	9

#define POS_N     5
#define TOTAL_N  16
//volatile int flag=0;
enum pinta_machine {
PINTA,
};

enum hora_machine {
HORA,
};

static  struct timeval state_period = { 0, 3*1000};
int tiempo_h[4];;
static int timer = 0;
static int pos=0;

static int numbers [10][4][7] ={{
{1,1,1,1,1,1,1} ,
{1,0,0,0,0,0,1} ,
{1,0,0,0,0,0,1} ,
{1,1,1,1,1,1,1}  },
{
{0,0,0,0,1,0,0} ,
{0,0,0,0,0,1,0} ,
{0,0,0,0,0,0,1} ,
{1,1,1,1,1,1,1}  },
{
{1,1,1,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,1,0,1,1,1,1}  },
{
{1,0,0,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,1,1,1,1,1,1} },
{
{0,0,0,1,1,1,1} ,
{0,0,0,1,0,0,0} ,
{0,0,0,1,0,0,0} ,
{1,1,1,1,1,1,1}  },
{
{1,0,0,1,1,1,1} ,
{1,0,0,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,1,1,1,0,0,1}  },
{
{1,1,1,1,1,1,1} ,
{1,0,0,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,1,1,1,0,0,1}  },
{
{0,0,0,0,0,0,1} ,
{0,0,0,0,0,0,1} ,
{0,0,0,0,0,0,1} ,
{1,1,1,1,1,1,1} },
{
{1,1,1,1,1,1,1} ,
{1,0,0,1,0,0,1} ,
{1,0,0,1,0,0,1} ,
{1,1,1,1,1,1,1}  },
{
{0,0,0,1,1,1,1} ,
{0,0,0,1,0,0,1} ,
{0,0,0,1,0,0,1} ,
{1,1,1,1,1,1,1}  
}
};
static int led[8];
static void store ( int n,int num){
/*        led[0] = numbers[num][0][n];
        led[1] = numbers[num][1][n];
        led[2] = numbers[num][2][n];
        led[3] = numbers[num][3][n];
        led[4] = numbers[num][4][n];
        led[5] = numbers[num][5][n];
        led[6] = numbers[num][6][n];
  */

 	led[0] = numbers[num][n][0];
        led[1] = numbers[num][n][1];
        led[2] = numbers[num][n][2];
        led[3] = numbers[num][n][3];
        led[4] = numbers[num][n][4];
        led[5] = numbers[num][n][5];
        led[6] = numbers[num][n][6];
      
}



void init_led(void){




}
void timespec_sub (struct timespec *res, struct timespec *a, struct timespec *b) {
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_nsec = a->tv_nsec - b->tv_nsec;
  if (res->tv_nsec < 0) {
    --res->tv_sec;
    res->tv_nsec += 1000000000;
  }
}
void
timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b)
{
res->tv_sec = a->tv_sec - b->tv_sec;
res->tv_usec = a->tv_usec - b->tv_usec;
if (res->tv_usec < 0) {
--res->tv_sec;
res->tv_usec += 1000000;
}
}
// res = a + b
void
timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
res->tv_sec = a->tv_sec + b->tv_sec
+ a->tv_usec / 1000000 + b->tv_usec / 1000000;
res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}
// wait until next_activation (absolute time)
void delay_until (struct timeval* next_activation)
{

struct timeval now, timeout;
gettimeofday (&now, NULL);
//printf("delay %d   %d\n",next_activation->tv_usec,now.tv_usec);
timeval_sub (&timeout, next_activation, &now);
//printf("aljafma %d \n",timeout.tv_usec);

select (0, NULL, NULL, NULL, &timeout);
}


static void timer_isr (union sigval arg) { timer = 1; }



static void timer_start (int ms)
{
timer_t timerid;
struct itimerspec value;
struct sigevent se;
se.sigev_notify = SIGEV_THREAD;
se.sigev_value.sival_ptr = &timerid;
se.sigev_notify_function = timer_isr;
se.sigev_notify_attributes = NULL;
value.it_value.tv_sec = ms / 1000;
value.it_value.tv_nsec = (ms % 1000) * 1000000;
value.it_interval.tv_sec = 0;
value.it_interval.tv_nsec = 0;
timer_create (CLOCK_REALTIME, &se, &timerid);
timer_settime (timerid, 0, &value, NULL);
}
void leds (void) {
 

        printf(" %d %d %d %d %d %d %d \n" ,led[0]/*numbers [3][0][0]*/ ,led[1],led[2],led[3],led[4],led[5],led[6]);


}

 void draw (int num){
struct timeval next_activation1;
gettimeofday (&next_activation1, NULL);
//printf("1 algo %d \n",next_activation1.tv_usec);

timeval_add (&next_activation1,&next_activation1, &state_period);
//printf("2 algo %d \n",next_activation1.tv_usec);

        if (pos < POS_N){
                store(pos,tiempo_h[num]);

                pos= pos + 1;
if(pos!=5){


	leds();
}else{printf("\n");}
        } else if(pos >= POS_N ){
        pos = 0;

                }


//wprintf("aljafma %d \n",state_period.tv_usec);

delay_until(&next_activation1);
}


static int draw_state (fsm_t* this)
{int n=0;
struct timeval next_activation2;
struct timeval  inicio, fin ,resultado;
int num;
for(n;n<TOTAL_N;n++)
{
num=(int)n/4;


gettimeofday (&inicio,NULL);
draw(num);
gettimeofday ( &fin,NULL);



timeval_sub (&resultado,&fin , &inicio);

//printf("time %d     \n",(int)resultado.tv_usec);


}return 1;
}
static void flag_func(fsm_t* this){
//flag=1;

}
static int hora_state (fsm_t* this)
{
//printf("time %d     \n",(int)resultado.tv_usec);
time_t tiempo = time(0);
struct tm *tlocal = localtime(&tiempo);
 tiempo_h[2]=(int)tlocal->tm_min/10;
 tiempo_h[3]=tlocal->tm_min%10;
 tiempo_h[0]=(int)tlocal->tm_hour/10;
 tiempo_h[1]=tlocal->tm_hour%10;
//printf("minutos %d    %d \n",tiempo_h[1],tiempo_h[0]);

return 1;
}
static void flag_func2(fsm_t* this){
//flag=1;

}


static fsm_trans_t pinta_machine[] = {
{ PINTA, draw_state, PINTA, flag_func},
{-1, NULL, -1, NULL },
};
static fsm_trans_t hora_machine[] = {
{ HORA, hora_state, HORA, flag_func2},
{-1, NULL, -1, NULL },
};


int main ()
{
struct timeval clk_period = { 0, 111 * 1000 };
struct timeval next_activation;
int o=0;
 int phase = 0;
struct timespec  inicio, fin ,resultado;
fsm_t* pinta_mac = fsm_new (pinta_machine);
fsm_t* hora_mac = fsm_new (hora_machine);

gettimeofday (&next_activation, NULL);
wiringPiSetup();
 pinMode (GPIO_LED, OUTPUT);
while (1) {
switch (phase) {
    case 0:
clock_gettime (CLOCK_MONOTONIC,&inicio);
fsm_fire(pinta_mac);
clock_gettime (CLOCK_MONOTONIC,&fin);

fsm_fire(hora_mac);

break;
}
phase = (phase + 1) % 1;

timespec_sub (&resultado,&fin , &inicio);

// printf("time %d     %d \n",resultado.tv_nsec,o);
o++;
timeval_add (&next_activation, &next_activation, &clk_period);
delay_until (&next_activation);
}}
