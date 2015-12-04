#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

LOCAL int newlock();

SYSCALL lcreate()
{
	STATWORD ps;
	int lockdes;

	disable(ps);
	if( (lockdes=newlock()) == SYSERR){
		restore(ps);
		return SYSERR;
	}
	restore(ps);
	return lockdes;
}

LOCAL int newlock()
{
	int lock, i, j;
	for( i = 0; i < NLOCKS; i++)
	{
		lock = nextlock--;
		if(nextlock < 0)
			nextlock = NLOCKS - 1;
		if(locks[lock].lstate == LFREE)
		{
			locks[lock].lstate = LUSED;
			locks[lock].lockdes += NLOCKS;
			locks[lock].lreaders = 0;
			locks[lock].lwriters = 0;

			for(j = 0; j < NPROC; j++) lock_tab[j][lock] = 0;
			return locks[lock].lockdes;
		}
	}
	return SYSERR;
}