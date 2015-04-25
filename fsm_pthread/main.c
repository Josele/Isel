#include <pthread.h>
#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <sys/time.h>

static
void
create_task (pthread_t* tid, void *(*f)(void *), void* arg,
             int period_ms, int prio, int stacksize)
{
  pthread_attr_t attr;
  struct sched_param sparam;
  struct timespec next_activation;
  struct timespec period = { 0, 0L };

  sparam.sched_priority = sched_get_priority_min (SCHED_FIFO) + prio;
  clock_gettime (CLOCK_REALTIME, &next_activation);
  next_activation.tv_sec += 1;
  period.tv_sec  = period_ms / 1000;
  period.tv_nsec = (period_ms % 1000) * 1000L;

  pthread_attr_init (&attr);
  pthread_attr_setstacksize (&attr, stacksize);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
  pthread_attr_setschedparam (&attr, &sparam);
  pthread_create (tid, &attr, f, arg);
  pthread_make_periodic_np (pthread_self(), &next_activation, &period);
}

static
void
init_mutex (pthread_mutex_t* m, int prioceiling)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  // pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_PROTECT);
  pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init (m, &attr);
  // pthread_mutex_setprioceiling
  //   (&m, sched_get_priority_min (SCHED_FIFO) + prioceiling);
}

static int cnt;
static pthread_mutex_t cnt_mutex;

static
void*
toggle_led0 (void* arg)
{
  static int value = HIGH;
  while (1) {
    pthread_wait_np ((unsigned long*) arg);
    digitalWrite (0, value);
    if (value) {
      pthread_mutex_lock (&cnt_mutex);
      ++cnt;
      pthread_mutex_unlock (&cnt_mutex);
    }
    value = !value; 
  }
}

static
void*
print_seconds (void* arg)
{
  int secs;
  while (1) {
    pthread_wait_np ((unsigned long*) arg);
    pthread_mutex_lock (&cnt_mutex);
    secs = cnt;
    pthread_mutex_unlock (&cnt_mutex);
    printf ("Tiempo: %d\n", secs);
  }
}

int
main (void)
{
  pthread_t tled0, tprsecs;
  void* ret;

  init_mutex (&cnt_mutex, 2);
  create_task (&tled0,   toggle_led0,   NULL,   500, 1, 1024);
  create_task (&tprsecs, print_seconds, NULL, 10000, 2, 1024);

  pthread_join(tid1, &ret);
  pthread_join(tid2, &ret);
  return 0;
}
