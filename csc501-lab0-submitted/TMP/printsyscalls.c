#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>

int getpid_count[NPROC];
int gettime_count[NPROC];
int kill_count[NPROC];
int signal_count[NPROC];
int sleep_count[NPROC];
int wait_count[NPROC];
int trace = 0;

void syscallsummary_start()
{	
	int i;
	for(i = 0; i < NPROC; i++ )
	{
		getpid_count[i] = 0;
		gettime_count[i] = 0;
		kill_count[i] = 0;
		signal_count[i] = 0;
		sleep_count[i] = 0;
		wait_count[i] = 0;
	}
	trace = 1;
}

void syscallsummary_stop()
{
	trace = 0;
}

void printsyscalls()
{	
	kprintf("\n#5 printsyscalls--------------------\n");
	kprintf("PID\tgetpid\tgettime\tkill\tsignal\tsleep\twait\n");
	int i;
	for( i = 0; i < NPROC; i++ )
	{
		kprintf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", i, getpid_count[i], gettime_count[i], kill_count[i], signal_count[i], sleep_count[i], wait_count[i]);
	}
	
}
