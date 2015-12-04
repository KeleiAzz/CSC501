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
	if(isbadlock(lock_id) || locks[lock_id].lstate == LFREE || ldes1 != locks[lock_id].lockdes)
	{
		restore(ps);
		return SYSERR;
	}
	// lptr = &locks[lock_id];

	if( locks[lock_id].lreaders == 0 && locks[lock_id].lwriters == 0) should_wait = 0;
	else if( locks[lock_id].lreaders > 0 && locks[lock_id].lwriters == 0)
	{
		if(type == READ){
			// if a writer with higher priority is waiting, then this reader should wait.
			should_wait = 0;
			int item;
			for(item = q[locks[lock_id].lqtail].qprev; item != locks[lock_id].lqhead && q[item].qkey > priority; item = q[item].qprev)
			{
				if(q[item].qtype == WRITE )
				{
					should_wait = 1;
					break;
				}
			}
			
		}else
		{
			should_wait = 1;
		}
	}
	else
	{
		should_wait = 1;
	}
	// kprintf("ldes %d, currpid %d, type: %d should wait: %d, readers: %d, writers: %d\n", ldes1, currpid, type, should_wait, lptr -> lreaders, lptr -> lwriters);
	if(should_wait)
	{
		// int pid = currpid;
		struct  pentry  *pptr;
		pptr = &proctab[currpid];
		pptr -> pstate = PRLWAIT;
		pptr -> plwaitret = OK;
		insert(currpid, locks[lock_id].lqhead, priority);
		q[currpid].qtype = type;
		resched();
		restore(ps);
		// kprintf("plwaitret: %d\n", pptr -> plwaitret);
		return pptr -> plwaitret;
	}else
	{
		lock_tab[currpid][lock_id] ++;
		if(type == READ) locks[lock_id].lreaders ++;
		else if(type == WRITE) locks[lock_id].lwriters ++;
        restore(ps);
       	return(OK);
	}
}