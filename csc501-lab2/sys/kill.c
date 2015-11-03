/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	int i;
	for(i = 0; i <= MAX_ID; i++)
	{
		if(proctab[pid].bs_pid_map[i].bs_status == BSM_MAPPED)
		{
			proctab[pid].bs_pid_map[i].bs_status = BSM_UNMAPPED;
			proctab[pid].bs_pid_map[i].bs_pid = -1;
    		proctab[pid].bs_pid_map[i].bs_vpno = -1;
    		proctab[pid].bs_pid_map[i].bs_npages = 0;
    		proctab[pid].bs_pid_map[i].bs_sem = -1;
    		proctab[pid].bs_pid_map[i].private = 0; 
    		bsm_tab[i].bs_num_proc--;
    		if(bsm_tab[i].bs_num_proc == 0)
    		{
    			release_bs(i);
    		}
		}
	}
	for(i = 0; i < NFRAMES; i++)
	{
		if(frm_tab[i].fr_pid == pid)
		{
			frm_tab[i].fr_status = FRM_UNMAPPED;
			frm_tab[i].fr_pid = -1;
  			frm_tab[i].fr_vpno = -1;
  			frm_tab[i].fr_refcnt = 0;
  			frm_tab[i].fr_type = -1;
  			frm_tab[i].fr_dirty = 0;
  			frm_tab[i].fr_loadtime = -1;
		}
	}


	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
