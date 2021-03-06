#include <stdlib.h>
#include "tasks.h"
#include <sys/time.h>
#include <stdio.h>
#include <wiringPi.h>
 #include "fsm.h"
#include <time.h>
#include <signal.h>
#define GPIO_BUTTON 2
#define GPIO_LED  3
#define GPIO_CUP  4
#define GPIO_COFFEE 5
#define GPIO_MILK 6
#define GPIO_COIN 7
#define GPIO_C1   8
#define GPIO_C2   9
#define GPIO_C3   10

#define CUP_TIME  250
#define COFFEE_PRICE  50
#define COFFEE_TIME 3000
#define MILK_TIME 3000

 struct timeval diff, diff2, ant, ant2;

enum cofm_state {
  COFM_WAITING,
  COFM_CUP,
  COFM_COFFEE,
  COFM_MILK,
};
enum coinm_state {
  COINM_WAITING,
};
static  pthread_mutex_t mcoin;
static  pthread_mutex_t mbutton;
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
//quantity=+coin[digitalRead (GPIO_C1)][digitalRead (GPIO_C2)][digitalRead (GPIO_C3)];



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
 pthread_mutex_lock(&mbutton);  

  if((quantity>=COFFEE_PRICE)&&(button==1)){
   flag_coin=1;

  }
  button = 0;
pthread_mutex_unlock(&mbutton);
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
printf("Estado: CUP\n");
}
static void coin_enough (fsm_t* this)
{
pthread_mutex_lock(&mcoin);
int devolver;
printf("coin_enough\n");

  if(flag_coin==1) {
  devolver=quantity-COFFEE_PRICE;
printf("Su cambio es: %d\n",devolver);
  flag_coin=0;
  devolver=0;
  quantity=0;
pthread_mutex_unlock(&mcoin);
    }
}

static void coffee (fsm_t* this)
{
  digitalWrite (GPIO_CUP, LOW);
  digitalWrite (GPIO_COFFEE, HIGH);
  timer_start (COFFEE_TIME);
printf("Estado: COFFEE\n");
}

static void milk (fsm_t* this)
{
  digitalWrite (GPIO_COFFEE, LOW);
  digitalWrite (GPIO_MILK, HIGH);
  timer_start (MILK_TIME);
printf("Estado: MILK\n");
}

static void finish (fsm_t* this)
{ 
  digitalWrite (GPIO_MILK, LOW);
  digitalWrite (GPIO_LED, HIGH);
printf("Estado: FINISH\n");
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
void interfaz(fsm_t* this){
int salida=0;
int tempmon;
if(this->current_state==COFM_WAITING){


printf("Introcuzca las monedas pulsando enter para enviar:\n");
//while(salida){
printf("Su credito es de %d \n",quantity);
printf("Introcuce moneda y pulse enter:\n");

// Moneda suponemos que maximo una moneda por un periodo
  scanf("%d", &tempmon);
 pthread_mutex_lock(&mcoin);
  quantity=quantity+tempmon;
 pthread_mutex_unlock(&mcoin);
 printf("¿Ha termindaode intruducir 0=n 1=yes :\n");

  scanf("%d", &salida);

if(salida==1){
printf("Ha presionado el boton:\n");
 pthread_mutex_lock(&mbutton);
  button_isr();
 pthread_mutex_unlock(&mbutton);
    }
}
salida=0;
}


static void
fsm_coin (void)
{

static fsm_t* coin_fsm;

 static struct timeval resultado;


coin_fsm=fsm_new (coinm);
int n=0;
  
for(n;n<200;n++){ 
gettimeofday (&ant, NULL);
timeval_sub (&ant, &ant, &diff);
printf("cofm_fsm:Periodo %d  \n",(int)ant.tv_usec); 
ant=diff;
  

  fsm_fire (coin_fsm);
gettimeofday (&diff, NULL);
}


//printf("coin_fsm:time %d \n",(int)resultado.tv_usec); 
}






static void
fsm_cofm (void)
{

static fsm_t* cofm_fsm;

  static struct timeval resultado;
 


cofm_fsm=fsm_new (cofm);
int n=0;
for(n;n<200;n++){
gettimeofday (&ant2, NULL);
timeval_sub (&ant2, &ant2, &diff2);
printf("cofm_fsm:Periodo %d  \n",(int)ant2.tv_usec); 
ant2=diff2;
interfaz(cofm_fsm);  
 fsm_fire (cofm_fsm); 
gettimeofday (&diff2, NULL);


 //printf("cofm_fsm:time %d  \n",(int)resultado.tv_usec);  




}
}
int
main (void)
{
volatile int e=0;
int n=100;
int tiempos[n];
struct timespec  inicio, fin ,resultado;
   wiringPiSetup();
 pinMode (GPIO_C1, INPUT);
 pinMode (GPIO_C2, INPUT);
 pinMode (GPIO_C3, INPUT);
  pinMode (GPIO_BUTTON, INPUT);
//  wiringPiISR (GPIO_BUTTON, INT_EDGE_FALLING, button_isr);
  pinMode (GPIO_COIN, INPUT);
//  wiringPiISR (GPIO_BUTTON, INT_EDGE_FALLING, moneda_isr);
  pinMode (GPIO_CUP, OUTPUT);
  pinMode (GPIO_COFFEE, OUTPUT);
  pinMode (GPIO_MILK, OUTPUT);
  pinMode (GPIO_LED, OUTPUT);
  digitalWrite (GPIO_LED, HIGH);
 

wiringPiSetup () ;
pinMode (0, OUTPUT);
init_mutex (&mbutton, 2);
init_mutex (&mcoin, 2);
 pthread_t t_coin, t_cofm;
  void* ret;



  create_task (&t_cofm, fsm_cofm, NULL, 500, 1, 1024);
  create_task (&t_coin, fsm_coin, NULL, 250, 2, 1024);
	pthread_join(t_cofm, &ret);
  	pthread_join(t_coin, &ret);
  return 0;
}
