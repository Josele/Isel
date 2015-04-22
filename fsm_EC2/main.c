#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <wiringPi.h>
#include "fsm.h"
#include <stdio.h>
#define GPIO_BUTTON	2
#define GPIO_LED	3
#define GPIO_CUP	4
#define GPIO_COFFEE	5
#define GPIO_MILK	6
#define GPIO_COIN	7
#define GPIO_C1		8
#define GPIO_C2		9
#define GPIO_C3		10

#define CUP_TIME	250
#define COFFEE_PRICE	50
#define COFFEE_TIME	3000
#define MILK_TIME	3000

enum cofm_state {
  COFM_WAITING,
  COFM_CUP,
  COFM_COFFEE,
  COFM_MILK,
};
enum coinm_state {
  COINM_WAITING,
};

static int button = 0;
static int flag_coin = 0;
static int quantity = 0;
static void button_isr (void) { button = 1; }
static void moneda_isr (void) { 
int coin[2][2][2];
coin[0][0][0]=5;
coin[0][0][1]=10;
coin[0][1][0]=20;
coin[0][1][1]=50;
coin[1][0][0]=100;
coin[1][0][1]=200;
coin[1][1][0]=0;
coin[1][1][1]=0;
quantity=+coin[digitalRead (GPIO_C1)][digitalRead (GPIO_C2)][digitalRead (GPIO_C3)];



 }
static int timer = 0;
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


static int coin_introduced (fsm_t* this)
{

return flag_coin;
}
static int button_pressed (fsm_t* this)
{
  

  if((quantity>=COFFEE_PRICE)&&(button==1)){
	 flag_coin=1;

  }
  button = 0;
  return flag_coin;
}

static int timer_finished (fsm_t* this)
{
  int ret = timer;
  timer = 0;
  return ret;
}

static void cup (fsm_t* this)
{
  digitalWrite (GPIO_LED, LOW);
  digitalWrite (GPIO_CUP, HIGH);
  timer_start (CUP_TIME);
//printf("Estado: CUP\n");
}
static void coin_enough (fsm_t* this)
{

int devolver;
  if(flag_coin==1) {
	devolver=quantity- COFFEE_PRICE;
//printf("Su cambio es: %d\n",devolver);
	flag_coin=0;
	devolver=0;
	quantity=0;
		}
}

static void coffee (fsm_t* this)
{
  digitalWrite (GPIO_CUP, LOW);
  digitalWrite (GPIO_COFFEE, HIGH);
  timer_start (COFFEE_TIME);
//printf("Estado: COFFEE\n");
}

static void milk (fsm_t* this)
{
  digitalWrite (GPIO_COFFEE, LOW);
  digitalWrite (GPIO_MILK, HIGH);
  timer_start (MILK_TIME);
//printf("Estado: MILK\n");
}

static void finish (fsm_t* this)
{
  digitalWrite (GPIO_MILK, LOW);
  digitalWrite (GPIO_LED, HIGH);
//printf("Estado: FINISH\n");
}

// Explicit FSM description money
static fsm_trans_t coinm[] = {
  { COINM_WAITING, coin_introduced, COINM_WAITING,coin_enough},
  {-1, NULL, -1, NULL },
};
// Explicit FSM description
static fsm_trans_t cofm[] = {
  { COFM_WAITING, button_pressed, COFM_CUP,     cup    },
  { COFM_CUP,     timer_finished, COFM_COFFEE,  coffee },
  { COFM_COFFEE,  timer_finished, COFM_MILK,    milk   },
  { COFM_MILK,    timer_finished, COFM_WAITING, finish },
  {-1, NULL, -1, NULL },
};


// Utility functions, should be elsewhere
void timespec_sub (struct timespec *res, struct timespec *a, struct timespec *b) {
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_nsec = a->tv_nsec - b->tv_nsec;
  if (res->tv_nsec < 0) {
    --res->tv_sec;
    res->tv_nsec += 1000000000;
  }
}
// res = a - b
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
void
timespec_add (struct timespec *res, struct timespec *a,
struct timespec *b) 
{
  res->tv_sec = a->tv_sec + b->tv_sec
    + a->tv_nsec / 1000000000 + b->tv_nsec / 1000000000;
  res->tv_nsec = a->tv_nsec % 1000000000 + b->tv_nsec % 1000000000;
}
/*
// wait until next_activation (absolute time)
int delay_until (struct timespec* next_activation)
{
  struct timespec now, timeout;
  int temp;
  
  clock_gettime (CLOCK_MONOTONIC,&now);

  timespec_sub (&timeout,next_activation, &now);
 temp=timeout.tv_nsec;
// select (0, NULL, NULL, NULL, &timeout);
nanosleep(CLOCK_MONOTONIC,&timeout);  
return temp;
}*/
// wait until next_activation (absolute time)
int delay_until (struct timeval* next_activation)
{
int temp;
  struct timeval now, timeout;
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
 temp=timeout.tv_usec;

 select (0, NULL, NULL, NULL, &timeout);
return temp;
}
/*
int interfaz(fsm_t* this){
int salida=1;
int tempmon;
if(this->current_state==COFM_WAITING){


printf("Introcuzca las monedas pulsando enter para enviar:\n");
while(salida){
	printf("Su credito es de %d \n",quantity);
	printf("Introcuce moneda y pulse enter:\n");
	scanf("%d", &tempmon);
	quantity=quantity+tempmon;
	printf("¿Ha termindaode intruducir? 1=no/0=yes:\n");
	scanf("%d", &salida);
}
	printf("Presione el boton 'c':\n");
char* l=0;

	scanf(" %c", &l);
printf("boton %c:\n",l);
	if(l == 99)
		{
printf("Ha presionado el boton:\n");
	button_isr();
		}
}
salida=0;
return 0;
}*/
int interfaz(fsm_t* this){
int salida=1;
int tempmon;
if(this->current_state==COFM_WAITING){


//printf("Introcuzca las monedas pulsando enter para enviar:\n");
//while(salida){
	//printf("Su credito es de %d \n",quantity);
	//printf("Introcuce moneda y pulse enter:\n");

// Moneda suponemos que maximo una moneda por un periodo
	scanf("%d", &tempmon);
	quantity=quantity+tempmon;
	//printf("¿Ha termindaode intruducir? 1=no/0=yes:\n")
//boton
	scanf("%d", &salida);
//}
	//printf("Presione el boton 'c':\n");
//char* l=0;

//	scanf(" %c", &l);
//printf("boton %c:\n",l);
//	if(l == 99)
if(salida==1){
//printf("Ha presionado el boton:\n");
	button_isr();
		}
}
salida=0;
return 0;
}

int main (int argc,char *argv[])
{ int n=100;
 int tiempos[n][3];
  struct timespec medio, inicio, fin, resultado;
  struct timeval clk_period = { 0, 250* 1000 };
  struct timeval next_activation;
  int phase = 0;
  fsm_t* cofm_fsm = fsm_new (cofm);
  fsm_t* coin_fsm = fsm_new (coinm);
  wiringPiSetup();
 pinMode (GPIO_C1, INPUT);
 pinMode (GPIO_C2, INPUT);
 pinMode (GPIO_C3, INPUT);
  pinMode (GPIO_BUTTON, INPUT);
  wiringPiISR (GPIO_BUTTON, INT_EDGE_FALLING, button_isr);
  pinMode (GPIO_COIN, INPUT);
  wiringPiISR (GPIO_BUTTON, INT_EDGE_FALLING, moneda_isr);
  pinMode (GPIO_CUP, OUTPUT);
  pinMode (GPIO_COFFEE, OUTPUT);
  pinMode (GPIO_MILK, OUTPUT);
  pinMode (GPIO_LED, OUTPUT);
  digitalWrite (GPIO_LED, HIGH);

  
 //printf("Ha presionado el boton:\n");

 
 gettimeofday (&next_activation, NULL);
int e=0;

  for ( e;e<n;e++) {
 
interfaz(cofm_fsm);
 tiempos[e][0]=delay_until (&next_activation);

    switch (phase) {
    case 0:
   
  clock_gettime (CLOCK_MONOTONIC,&inicio);
  

    fsm_fire (cofm_fsm);
 clock_gettime (CLOCK_MONOTONIC,&medio);
    fsm_fire (coin_fsm);
 clock_gettime (CLOCK_MONOTONIC,&fin);

 
timespec_sub (&resultado,&medio , &inicio);
tiempos[e][1]=resultado.tv_nsec;
timespec_sub (&resultado,&fin,&medio);

tiempos[e][2]=resultado.tv_nsec;
break;
    }
    timeval_add (&next_activation, &next_activation, &clk_period);
    phase = (phase + 1) % 1;
  }e=0;
  for ( e;e<n;e++) {
 printf("Fsm_time1 %d  Fsm_time2 %d \n Sobrante %d \n",tiempos[e][1],tiempos[e][2],tiempos[e][0]*1000);

}
}

