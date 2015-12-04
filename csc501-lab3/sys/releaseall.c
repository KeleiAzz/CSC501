#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int releaseall(int numlocks, int ldes1, ...)
{
	int i;
	int lockdes, has_err = 0;
	int pid = currpid;
	STATWORD ps;
	
	struct lentry *lptr;
	// unsigned long *a = (unsigned long *)(&ldes1);
    for (i = 0 ; i < numlocks ; i++)
	{
        lockdes = *(&ldes1 + i); 
        disable(ps);
        // kprintf("release lockdes: %d\n", lockdes);
		int lock_id = lockdes % NLOCKS;
		lptr = &locks[lock_id];
		if(isbadlock(lock_id) || lptr -> lstate == LFREE )
		{
			// restore(ps);
			has_err++;
			continue;
			// return SYSERR;
		}

		if( lock_tab[pid][lock_id] > 0)
			lock_tab[pid][lock_id] --;
		else
		{
			// restore(ps);
			has_err++;
			continue;
			// kprintf("something goes wrong\n");
			// return SYSERR;
		}

		if(lptr -> lreaders > 0 && lptr -> lwriters == 0)
			lptr -> lreaders --;
		else if(lptr -> lreaders == 0 && lptr -> lwriters == 1)
			lptr -> lwriters --;
	
	// kprintf("currpid: %d, readers: %d, writers: %d\n", currpid, lptr -> lreaders, lptr -> lwriters);
		if(lptr -> lreaders == 0 && lptr -> lwriters == 0)
		{
		//need to turn some process to ready
			int last, item, last_key, tmp;
			last = q[lptr -> lqtail].qprev;
		
			if( last == lptr -> lqhead) 
			{
			// kprintf("empty queue\n");
				continue;
			}
		// kprintf("last in queue type: %d\n",q[last].qtype);
			if(q[last].qtype == READ)
			{
				lptr -> lreaders ++;
				lock_tab[last][lock_id] ++;
				last_key = q[last].qkey;
				item = q[last].qprev;
				dequeue(last);
				ready(last, RESCHNO);
				// kprintf("Last in q: %d, set ready\n", last);
				while(item != lptr -> lqhead)
				{
					if(q[item].qtype == READ && q[item].qkey == last_key)
					{
						tmp = item;
						item = q[item].qprev;
						// kprintf("turn process %d to ready\n", tmp);
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

		}
		restore(ps);	
		// if(release(currpid, lockdes) == SYSERR) has_err = 1;
	}
	
	resched();
	if(has_err == 1) return(SYSERR);
	else return(OK);
}
