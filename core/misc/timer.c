#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "bk_system_defs.h"
#include "timer.h"


timer_t 	timerid;
struct		sigevent  sev;
struct		sigaction sa;
struct		itimerspec its;
long long	freq_nanosecs;
sigset_t	mask;

//BOOL (*handler)(int sig, siginfo_t *si, void *uc) = NULL;	
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define errExit(msg)    \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

// function pointer variable:
void set_handler( struct sigaction* msa, 
		  void (*mHandler)(int sig, siginfo_t *si, void *uc) )
{
   /* Establish handler for timer signal */   
   msa->sa_flags 	 = SA_SIGINFO;
   msa->sa_sigaction = mHandler;
   sigemptyset( &(msa->sa_mask) );
   if ( sigaction(SIG, msa, NULL) == -1 )
	   errExit("sigaction");
}

/* Create the timer */
void create_timer( struct sigevent* msev,  timer_t*  mtimerid )
{
   msev->sigev_notify 		 	= SIGEV_SIGNAL;
   msev->sigev_signo  		 	= SIG;
   msev->sigev_value.sival_ptr 	= mtimerid;
   if (timer_create(CLOCKID, msev, mtimerid) == -1)
	   errExit("timer_create");
}

void start_timer(struct itimerspec* mits, 
				 long long mfreq_nanosecs, 
				 timer_t  mtimerid )
{
   /* Start the timer */
   mits->it_value.tv_sec     = mfreq_nanosecs / 1000000000.;	// billion
   mits->it_value.tv_nsec    = mfreq_nanosecs % 1000000000;		// billion
   mits->it_interval.tv_sec  = mits->it_value.tv_sec;
   mits->it_interval.tv_nsec = mits->it_value.tv_nsec;
   printf("start_timer:  interval=%d . %d\n", mits->it_interval.tv_sec, mits->it_interval.tv_nsec );
   if (timer_settime(mtimerid, 0, mits, NULL) == -1)
		errExit("timer_settime");
}

void block_timer()
{
   sigemptyset(&mask);
   sigaddset(&mask, SIG);
   if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	   errExit("sigprocmask");
}
void unblock_timer()
{
   /* Unlock the timer signal, so that timer notification
	  can be delivered */
   if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
	   errExit("sigprocmask"); 
}

/* A simple handler which informs the user.  Each app can 
   define it's own handler 
*/
int count = 0;
void ts_handler(int sig, siginfo_t *si, void *uc)
{
   /* Note: calling printf() from a signal handler is not
	  strictly correct, since printf() is not async-signal-safe;
	  see signal(7) */
   count++;
   printf("Caught signal %d - %d\n", sig, count);
   //print_siginfo(si);   
   // signal(sig, SIG_IGN);
}

int test(int argc, char *argv[])
{
   if (argc != 3) {
	   fprintf(stderr, "Usage: %s <sleep-secs> <freq-nanosecs>\n",
			   argv[0]);
	   exit(EXIT_FAILURE);
   }

   printf("Establishing handler for signal %d\n", SIG);
   set_handler( &sa, ts_handler );
	
   /* Block timer signal temporarily */
   printf("Blocking signal %d\n", SIG);
   //block_timer();

   /* Create the timer */
   create_timer( &sev, timerid );
   printf("timer ID is 0x%lx\n", (long) timerid);

   /* Start the timer */
   start_timer( &its, atoll(argv[2]), timerid );

   /* Sleep for a while; meanwhile, the timer may expire
	  multiple times */
    printf("Sleeping for %d seconds\n", atoi(argv[1]));
	while (1)
	{   sleep(atoi(argv[1]));	}

   //printf("Unblocking signal %d\n", SIG);
   //unblock_timer( );
   exit(EXIT_SUCCESS);
}

/* Print Information about the Seg event */
void print_siginfo(siginfo_t *si)
{
   timer_t *tidp;
   int or;
   tidp = si->si_value.sival_ptr;
   printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
   printf("    *sival_ptr = 0x%lx\n", (long) *tidp);
   or = timer_getoverrun(*tidp);
   if (or == -1)
	   errExit("timer_getoverrun");
   else
	   printf("    overrun count = %d\n", or);
}

