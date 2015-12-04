#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

SYSCALL ldelete(int lockdescriptor)
{
	STATWORD ps;
	int pid, i;
	struct lentry *lptr;
	int lock_id = lockdescriptor % NLOCKS;

	disable(ps);
	if(isbadlock(lock_id) || locks[lock_id].lstate == LFREE )
	{
		restore(ps);
		return SYSERR;
	}
	lptr = &locks[lock_id];
	lptr -> lstate = LFREE;
	lptr -> lreaders = 0;
	lptr -> lwriters = 0;
	if(nonempty(lptr->lqhead))
	{
		while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		{
			proctab[pid].plwaitret = DELETED;
			ready(pid, RESCHNO);
		} 
		resched();
	}

	for( i = 0; i < NPROC; i++)
	{
		lock_tab[i][lock_id] = 0;
	}
	restore(ps);
	return OK;

}
