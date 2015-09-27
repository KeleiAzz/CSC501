/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>


unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int linux_resched();
int origin_resched();
int multiq_resched();
int realq_resched(int new_epoch);
int normalq_resched(int new_epoch);
int is_new_epoch(int current_queue);
int is_runnable(int queue_class);
int switch_to_null();
int sched_class = 0;
int flag = 0;
int current_q = -1;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int resched()
{	
	int state;
	int schedclass = getschedclass();
	if (schedclass == LINUXSCHED)
	{
		if(linux_resched())
		{
			return OK;	
		}
		
	}
	else if (schedclass == MULTIQSCHED)
	{
		if (multiq_resched())
		{
			return OK;	
		}
		
	}
	else
	{	
		kprintf("before %d %d\n", preempt, currpid);
		if(origin_resched())
		{
			return OK;	
		}
		
	}
}

int linux_resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	optr = &proctab[currpid];
	int i;
	
	if (optr -> counter >= QUANTUM)
	{
		optr -> goodness = optr -> goodness - QUANTUM + preempt;
		optr -> counter = optr -> counter - QUANTUM + preempt;	
	}
	else
	{
		optr -> goodness = optr -> goodness - optr -> counter + preempt;
		optr -> counter = preempt;		
	}
	

	if (optr -> counter <= 0)
	{
		optr -> goodness = 0;
		optr -> counter = 0;
	}


	int new_epoch = 1;
	if ( optr -> pstate == PRCURR && (optr -> counter > 0))
	{
		 new_epoch = 0; //If optr is current and counter > 0, then it's not a new epoch	
	}
	
	for (i = q[rdyhead].qnext; i != rdytail; i = q[i].qnext)
	{
		if(proctab[i].pstate == PRREADY && proctab[i].counter > 0)
		{
			new_epoch = 0; //If any ready process has a counter > 0, it's not a new epoch
			break;
		}
	}
	
	if (new_epoch) //If it's a new epoch, initiate all the processes not PRFREE
	{	
		
		for ( i = 0; i < NPROC; i++ )
		{	
			if (proctab[i].pstate != PRFREE)
			{	
				if (proctab[i].counter <= 0 || proctab[i].counter == proctab[i].quantum)
				{
					proctab[i].quantum = proctab[i].pprio;
				}
				else
				{
					proctab[i].quantum = proctab[i].counter / 2 + proctab[i].pprio;
				}
				proctab[i].counter = proctab[i].quantum;
				proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
			}
		}
		preempt = QUANTUM;
		// flag = 0;
	}


	int  max_goodness = 0, new_proc = 0;
	for (i = 0; i < NPROC; i++)
	{
		if (proctab[i].goodness > max_goodness && proctab[i].pstate == PRREADY)
		{
			max_goodness = proctab[i].goodness;
			new_proc = i;
		}
	}

	if(( optr -> pstate == PRCURR) && (optr -> goodness >= max_goodness) && (optr -> goodness > 0))
	{
		if (optr -> counter >= QUANTUM)
		{
			preempt = QUANTUM;
		}
		else
		{
			preempt = optr -> counter;	
		}
		return OK;
	}
	else if( optr -> pstate != PRCURR || optr -> goodness == 0 || optr -> goodness < max_goodness )
	{
		if(optr -> pstate == PRCURR)
		{
			optr -> pstate = PRREADY;
			insert(currpid, rdyhead, optr -> pprio);
		}
		nptr = &proctab[ (currpid = new_proc) ];
		nptr->pstate = PRCURR;		
		dequeue(currpid);
		if (nptr -> counter >= QUANTUM)
		{
			preempt = QUANTUM;
		}
		else
		{
			preempt = nptr -> counter;	
		}
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return OK;
	}
	else
	{
		return -1;
	}
}

int is_new_epoch(int current_q)
{	
	if (current_q == -1)
	{
		return 1;
	}
	register struct	pentry	*optr;
	optr = &proctab[currpid];
	int i;
	if (optr -> pstate == PRCURR)
	{
		if( optr -> is_real == 0 && optr -> goodness > 0) return 0;
		if( optr -> is_real == 1 && optr -> counter > 0) return 0;
	}
	if (current_q == NORMALQUEUE)
	{
		for(i = 0; i < NPROC; i++)
		{
			if(proctab[i].pstate == PRREADY && proctab[i].is_real == 0 && proctab[i].goodness > 0)
			{
				return 0;		
			}
		}
	}
	if (current_q == REALQUEUE)
	{
		for(i = 0; i < NPROC; i++)
		{
			if(proctab[i].pstate == PRREADY && proctab[i].is_real == 1 && proctab[i].counter > 0)
			{
				return 0;		
			}
		}	
	}
	return 1;
}

int is_runnable(int queue_class)
{
	int i;
	if (proctab[currpid].pstate == PRCURR && proctab[currpid].is_real == queue_class) return currpid;
	for (i = 1; i < NPROC; i++)
	{
		if (proctab[i].pstate == PRREADY && proctab[i].is_real == queue_class)
		{
			return i;
		}
	}
	return 0;
}

int switch_to_null()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	optr = &proctab[currpid];
	currpid = 0;
	nptr = &proctab[currpid];
	nptr->pstate = PRCURR;	
	// dequeue(currpid);
	preempt = QUANTUM;
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	return OK;	
}

int multiq_resched()
{	
	register struct	pentry	*optr;	/* pointer to old process entry */
	int new_epoch = 1;
	optr = &proctab[currpid];

	if ( optr -> is_real == 0 )
	{	
		if (optr -> counter > QUANTUM)
		{
			optr -> goodness = optr -> goodness - QUANTUM + preempt;
			optr -> counter = optr -> counter - QUANTUM + preempt;	
		}
		else
		{
			optr -> goodness = optr -> goodness - optr -> counter + preempt;
			optr -> counter = preempt;
		}
		

		if (optr -> counter <= 0)
		{
			optr -> goodness = 0;
			optr -> counter = 0;
		}
	}
	if ( optr -> is_real == 1)
	{
		optr -> counter = preempt;

		if (optr -> counter <= 0)
		{
			optr -> counter = 0;
		}
	}

	int runnable_normal = is_runnable(NORMALQUEUE);
	int runnable_real = is_runnable(REALQUEUE);

	if (runnable_normal == 0 && runnable_real == 0)
	{
		return switch_to_null();
	}

	new_epoch = is_new_epoch(current_q);

	int flag = 1;
	if (new_epoch)
	{
		int rand_num = rand() % 10;
		rand_num = rand() % 10;
		if (rand_num < 7)
		{
			if (runnable_real) 
			{
				current_q = REALQUEUE;
				return realq_resched(new_epoch);	
			}
			else
			{
				current_q = NORMALQUEUE;
				return normalq_resched(new_epoch);
			}
			
		}
		else
		{
			if (runnable_normal)
			{
				current_q = NORMALQUEUE;
				return normalq_resched(new_epoch);
			}
			else
			{
				current_q = REALQUEUE;
				return realq_resched(new_epoch);
			}
			
		}
	}
	else
	{
		if ( current_q == NORMALQUEUE )
		{
			if (runnable_normal)
			{
				current_q = NORMALQUEUE;
				return normalq_resched(new_epoch);
			}
			else
			{
				current_q = REALQUEUE;
				return realq_resched(new_epoch);
			}	
		}
		else if ( current_q == REALQUEUE )
		{
			if (runnable_real) 
			{
				current_q = REALQUEUE;
				return realq_resched(new_epoch);	
			}
			else
			{
				current_q = NORMALQUEUE;
				return normalq_resched(new_epoch);
			}
		}
	}
	return -1;
}

int realq_resched(int new_epoch)
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	optr = &proctab[currpid];
	if(new_epoch)
	{	
		int i;
		for ( i = 1; i < NPROC; i++ )
		{	
			if (proctab[i].pstate != PRFREE && proctab[i].is_real)
			{	
				proctab[i].quantum = 100;
				proctab[i].counter = 100;
			}
		}
		preempt = 100;
	}

	if(proctab[currpid].is_real == 0)
	{
        if(optr -> pstate == PRCURR) // if current proc is normal proc, store it
        {
            optr -> pstate = PRREADY;
            insert(currpid, rdyhead, optr -> pprio);
        }
		int i; // choose the next to run.
        for(i = rdytail; i != rdyhead; i = q[i].qprev)
        {
        	if(proctab[i].is_real == 1 && proctab[i].counter > 0) break;	
        } 
        currpid = i;
        nptr = &proctab[currpid];
        nptr->pstate = PRCURR;
        dequeue(currpid);
        preempt = nptr -> counter;
        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
        return OK;
	}
	else
	{
		if((optr->pstate == PRCURR) && (optr->counter > 0))
		{
			return OK;	
		} 
		else if ((optr->pstate != PRCURR) || (optr -> counter <= 0))
		{
			if(optr -> pstate == PRCURR) // if current proc is running, insert to ready queue
            {
                optr -> pstate = PRREADY;
                insert(currpid, rdyhead, optr -> pprio);
            }
			int i; // choose the next to run.
			for(i = rdytail; i != rdyhead; i = q[i].qprev)
			{
				if(proctab[i].is_real == 1 && proctab[i].counter > 0) break;
			}
			currpid = i;
			nptr = &proctab[currpid];
			nptr->pstate = PRCURR;	
			dequeue(currpid);
			preempt = nptr -> counter;
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;	
		}
	}
	return -1;
}


int normalq_resched(int new_epoch)
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	optr = &proctab[currpid];

	if (new_epoch) //If it's a new epoch, initiate all the processes not PRFREE
	{	
		int i;
		for ( i = 0; i < NPROC; i++ )
		{	
			if (proctab[i].pstate != PRFREE && proctab[i].is_real == 0)
			{	
				if (proctab[i].counter <= 0 || proctab[i].counter == proctab[i].quantum)
				{
					proctab[i].quantum = proctab[i].pprio;
				}
				else
				{
					proctab[i].quantum = proctab[i].counter / 2 + proctab[i].pprio;
				}
				proctab[i].counter = proctab[i].quantum;
				proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
			}
		}
		preempt = QUANTUM;
	}

	int  max_goodness = 0, i, new_proc;
	for (i = q[rdyhead].qnext, new_proc = i, max_goodness = proctab[i].goodness; i != rdytail; i = q[i].qnext)
	{
		if(proctab[i].goodness > max_goodness && proctab[i].is_real == 0)
		{
			max_goodness = proctab[i].goodness;
			new_proc = i;
		}
	} 

	if (optr -> is_real == 1)
	{
		if(optr -> pstate == PRCURR) 
        {
            optr -> pstate = PRREADY;
            insert(currpid, rdyhead, optr -> pprio);
        }
        currpid = new_proc;
        nptr = &proctab[currpid];
        nptr->pstate = PRCURR;
        dequeue(currpid);
        if (nptr -> counter >= QUANTUM)
		{
			preempt = QUANTUM;
		}
		else
		{
			preempt = nptr -> counter;	
		}
        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
        return OK;
	}
	else
	{
		if(( optr -> pstate == PRCURR) && (optr -> goodness >= max_goodness) && (optr -> goodness > 0))
		{
			if (optr -> counter >= QUANTUM)
			{
				preempt = QUANTUM;
			}
			else
			{
				preempt = optr -> counter;	
			}
			return OK;
		}
		else
		{
			if(optr -> pstate == PRCURR)
			{
				optr -> pstate = PRREADY;
				insert(currpid, rdyhead, optr -> pprio);
			}
			nptr = &proctab[ (currpid = new_proc) ];
			nptr->pstate = PRCURR;		
			dequeue(currpid);
			if (nptr -> counter >= QUANTUM)
			{
				preempt = QUANTUM;
			}
			else
			{
				preempt = nptr -> counter;	
			}
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;
		}
	}
	return -1;
}

int origin_resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/
	
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
	   	kprintf("currpid %d\n", currpid);
		return(OK);
	}
	// kprintf("currpid: %d\n", currpid);
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}
	
	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	// kprintf("before new currpid: %d\n", currpid);
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	// kprintf("new currpid: %d\n", currpid);
	return OK;
}


void setschedclass(int schedclass)
{	
	sched_class = schedclass;
}
int getschedclass()
{
	return sched_class;
}
