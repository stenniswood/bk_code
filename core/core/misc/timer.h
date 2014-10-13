#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef  __cplusplus
extern "C" {
#endif

// NOTE Makefile needs a CFLAG with "-lrt" for realtime clock to compile this!
extern timer_t 		timerid;
extern struct		sigevent sev;
extern struct		itimerspec its;
extern long long	freq_nanosecs;
extern sigset_t		mask;
extern struct		sigaction sa;


void print_siginfo	( siginfo_t *si						);
void ts_handler   	( int sig, siginfo_t *si, void *uc	);

void set_handler	( struct sigaction* msa, 
					  void (*mHandler)(int sig, siginfo_t *si, void *uc) );
void create_timer	( struct sigevent* msev,  timer_t*  mtimerid 	 	 );
void start_timer	( struct itimerspec* mits, long long mfreq_nanosecs, 
					  timer_t  mtimerid );

void block_timer	(						);
void unblock_timer	(						);

// test handler:
void ts_handler	  ( int sig, siginfo_t *si, void *uc);
int  test		  ( int argc, char *argv[]			);

#ifdef  __cplusplus
}
#endif

#endif
