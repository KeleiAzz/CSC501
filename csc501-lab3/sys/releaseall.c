#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int releaseall(int numlocks, int ldes1, ...)
{
	int i;
	int lockdes, has_err = 0;
	// unsigned long *a = (unsigned long *)(&ldes1);
    for (i = 0 ; i < numlocks ; i++)
	{
        lockdes = *(&ldes1 + i); 
		if(release(currpid, lockdes) == SYSERR) has_err = 1;
	}
	resched();
	if(has_err == 1) return(SYSERR);
	else return(OK);
}


int release(int pid, int lockdes)
{
	kprintf("release pid: %d, lockdes: %d\n", pid, lockdes);
	STATWORD ps;
	struct lentry *lptr;
	int lock_id = lockdes % NLOCKS;
	disable(ps);
	lptr = &locks[lock_id];
	if(isbadlock(lock_id) || lptr -> lstate == LFREE )
	{
		restore(ps);
		return SYSERR;
	}

	if( lock_tab[pid][lock_id] > 0)
		lock_tab[pid][lock_id] --;
	else
	{
		restore(ps);
		return SYSERR;
	}

	if(lptr -> lreaders > 0 && lptr -> lwriters == 0)
		lptr -> lreaders --;
	else if(lptr -> lreaders == 0 && lptr -> lwriters == 1)
		lptr -> lwriters --;
	
	kprintf("readers: %d, writers: %d\n", lptr -> lreaders, lptr -> lwriters);
	if(lptr -> lreaders == 0 && lptr -> lwriters == 0)
	{
		//need to turn some process to ready
		int last, item, last_key, tmp;
		last = q[lptr -> lqtail].qprev;
		
		if( last == lptr -> lqhead) 
		{
			kprintf("empty queue\n");
			return OK;
		}
		kprintf("last in queue type: %d\n",q[last].qtype);
		if(q[last].qtype == READ)
		{
			lptr -> lreaders ++;
			lock_tab[pid][lock_id] ++;
			last_key = q[last].qkey;
			item = q[last].qprev;
			dequeue(last);
			ready(last, RESCHNO);
			kprintf("Last in q: %d, set ready\n", last);
			while(item != lptr -> lqhead)
			{
				if(q[item].qtype == READ && q[item].qkey == last_key)
				{
					tmp = item;
					item = q[item].qprev;
					dequeue(tmp);
					ready(tmp, RESCHNO);
				}
			}
		}
		else
		{
			lptr -> lwriters ++;
			lock_tab[last][lock_id] ++;
			dequeue(last);
			ready(last, RESCHNO);
		}
		// if(nonempty(lptr->lqhead))
		// {
		// 	while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		// 	{
		// 		proctab[pid].plwaitret = DELETED;
		// 		ready(pid, RESCHNO);
		// 	} 
		// 	resched();
		// }
	}
	restore(ps);
	return OK;
}