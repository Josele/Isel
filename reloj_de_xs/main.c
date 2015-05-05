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


#define F_TOTAL     9
#define P_TOTAL     7
#define R_TIME 		56
volatile int flag=0;
enum ex_machine {
EX_N,
};

static  struct timeval state_period = { 0, 7*1000};

static int timer = 0;
static int pos=0;

static int ex [7][8] = {
{1,0,0,0,0,0,0,1} ,
{0,1,0,0,0,0,1,0} ,
{0,0,1,0,0,1,0,0} ,
{0,0,0,1,1,0,0,0} ,
{0,0,1,0,0,1,0,0} ,
{0,1,0,0,0,0,1,0} ,
{1,0,0,0,0,0,0,1} };
static int led[8];
static void store ( int n){
        led[0] = ex[n][0];
        led[1] = ex[n][1];
        led[2] = ex[n][2];
        led[3] = ex[n][3];
        led[4] = ex[n][4];
        led[5] = ex[n][5];
        led[6] = ex[n][6];
        led[7] = ex[n][7];
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
int temp;
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
 

        printf("%d %d %d %d %d %d %d %d \n " ,led[0],led[1],led[2],led[3],led[4],led[5],led[6],led[7]);


}

 void draw (void){
struct timeval next_activation1;
gettimeofday (&next_activation1, NULL);
//printf("1 algo %d \n",next_activation1.tv_usec);

timeval_add (&next_activation1,&next_activation1, &state_period);
//printf("2 algo %d \n",next_activation1.tv_usec);

        if (pos < P_TOTAL){
                store(pos);

                pos= pos + 1;

	leds();
        } else if(pos >= P_TOTAL ){
        pos = 0;

                }


//printf("aljafma %d \n",state_period.tv_usec);

delay_until(&next_activation1);
}


static int timer_finished (fsm_t* this)
{int n=0;
struct timeval next_activation2;
struct timeval  inicio, fin ,resultado;

for(n;n<8;n++)
{
gettimeofday (&inicio,NULL);
draw();
gettimeofday ( &fin,NULL);



timeval_sub (&resultado,&fin , &inicio);

//printf("time %d     \n",(int)resultado.tv_usec);


}return 1;
}
static void flag_func(fsm_t* this){
flag=1;

}


static fsm_trans_t ex_machine[] = {
{ EX_N, timer_finished, EX_N, flag_func},
{-1, NULL, -1, NULL },
};


int main ()
{
struct timeval clk_period = { 0, 111 * 1000 };
struct timeval next_activation;
int o=0;
struct timespec  inicio, fin ,resultado;
fsm_t* ex_mac = fsm_new (ex_machine);
gettimeofday (&next_activation, NULL);
wiringPiSetup();
 pinMode (GPIO_LED, OUTPUT);
while (1) {

clock_gettime (CLOCK_MONOTONIC,&inicio);
fsm_fire(ex_mac);
clock_gettime (CLOCK_MONOTONIC,&fin);



timespec_sub (&resultado,&fin , &inicio);

 printf("time %d     %d \n",resultado.tv_nsec,o);
o++;
timeval_add (&next_activation, &next_activation, &clk_period);
delay_until (&next_activation);
}}
