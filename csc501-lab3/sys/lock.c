#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int lock (int ldes1, int type, int priority)
{
	STATWORD ps;
	int i, should_wait = 0;
	struct lentry *lptr;
	int lock_id = ldes1 % NLOCKS;

	disable(ps);
	if(isbadlock(lock_id) || locks[lock_id].lstate == LFREE )
	{
		restore(ps);
		return SYSERR;
	}
	lptr = &locks[lock_id];

	if( lptr -> lreaders == 0 && lptr -> lwriters == 0) should_wait = 0;
	else if( lptr -> lreaders > 0 && lptr -> lwriters == 0)
	{
		if(type == READ){
			should_wait = 0;
		}else
		{
			should_wait = 1;
		}
	}
	else
	{
		should_wait = 1;
	}
	kprintf("currpid %d, type: %d should wait: %d\n", currpid, type, should_wait);
	if(should_wait)
	{
		proctab[currpid].pstate = PRLWAIT;
		proctab[currpid].plwaitret = OK;
		insert(currpid, lptr -> lqhead, priority);
		q[ldes1].qtype = type;
		resched();
		restore(ps);
		return proctab[currpid].plwaitret;
	}else
	{
		lock_tab[currpid][lock_id] ++;
		if(type == READ) lptr -> lreaders ++;
		else if(type == WRITE) lptr -> lwriters ++;
        restore(ps);
       	return(OK);
	}
}