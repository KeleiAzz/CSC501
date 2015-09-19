/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
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
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	optr= &proctab[currpid];

	if (optr -> counter >= QUANTUM)
	{
		optr -> goodness = optr -> goodness - QUANTUM;
		optr -> counter = optr -> counter - QUANTUM;
	}
	else
	{
		optr -> goodness = optr -> goodness - optr -> counter;
		optr -> counter = 0;
	}

	int i;
	int new_epoch = 1;
	for ( i = 0; i < NPROC; i++ )
	{
		if (proctab[i].pstate == PRREADY && proctab[i].counter != 0)
		{
			new_epoch = 0;
			break;
		}
	}
	

	if (new_epoch)
	{
		for ( i = 0; i < NPROC; i++ )
		{
			if (proctab[i].pstate != PRFREE)
			{	
				proctab[i].quantum = proctab[i].counter / 2 + proctab[i].pprio;
				proctab[i].counter = proctab[i].quantum;
				proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
			}
		}
		// preempt = QUANTUM;
	}


	int  max_goodness = 0, proc_id;
	for (i = 0; i < NPROC; i++)
	{
		if (proctab[i].goodness > max_goodness)
		{
			max_goodness = proctab[i].goodness;
			proc_id = i;
		}
	}

	if(( optr -> pstate == PRCURR) && (optr -> goodness >= max_goodness) && (optr -> goodness > 0))
	{
		if (optr -> counter < QUANTUM)
		{
			preempt = optr -> counter;
		}
		else
		{
			preempt = QUANTUM;
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
		currpid = proc_id;
		nptr = &proctab[currpid];
		nptr->pstate = PRCURR;		
		dequeue(currpid);
		if (nptr -> counter > QUANTUM)
		{
			preempt = QUANTUM;
		}
		else
		{
			preempt = nptr -> counter;		/* reset preemption counter	*/	
		}
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return OK;
	}
	else
	{
		return -1;
	}
}

int origin_resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
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
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
