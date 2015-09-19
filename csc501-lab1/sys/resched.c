/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
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
	optr = &proctab[currpid];
	optr -> goodness = optr -> goodness - QUANTUM;
	optr -> counter = optr -> counter - QUANTUM;

	if (optr -> counter <= 0)
	{
		optr -> goodness = 0;
		optr -> counter = 0;
	}

	int i;
	int new_epoch = 1;
	if ( optr -> pstate == PRCURR && optr -> counter > 0)
	{
		 new_epoch = 0; //If optr is current and counter > 0, then it's not a new epoch	
	}

	for ( i = 0; i < NPROC; i++ )
	{
		if (proctab[i].pstate == PRREADY && proctab[i].counter > 0)
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
				proctab[i].quantum = proctab[i].counter / 2 + proctab[i].pprio;
				proctab[i].counter = proctab[i].quantum;
				proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
				// kprintf("proc %d has quantum %d, counter %d, goodness %d\n", i, proctab[i].quantum, proctab[i].counter, proctab[i].goodness);
			}
		}
		preempt = QUANTUM;
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
		preempt = QUANTUM;
		return OK;
	}
	else if( optr -> pstate != PRCURR || optr -> goodness == 0 || optr -> goodness < max_goodness )
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
		preempt = QUANTUM;
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
