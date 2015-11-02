/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	int bsm_id;
	if( get_bsm(&bsm_id) == SYSERR)
	{
		kprintf("no bsm available\n");
		return SYSERR;
	}
	int pid = create(procaddr, ssize, priority, name, nargs, args);
	if(pid == SYSERR) return SYSERR;
	
	if(bsm_map(pid, 4096, bsm_id, hsize) == SYSERR)
	{
		kprintf("bsm mapping failed for %d", pid);
		return SYSERR;
	}
	bsm_tab[bsm_id].private = 1;
	proctab[pid].bs_pid_map[bsm_id].private = 1;
	// proctab[pid].bs_pid_map[bsm_id].bs_status = BSM_MAPPED;
	proctab[pid].store = bsm_id;
	proctab[pid].vhpno = 4096;
	proctab[pid].vhpnpages = hsize;

	kprintf("what is vmemlist\n");
	// proctab[pid].vmemlist -> mlen = hsize * NBPG;
	// resume(pid);
	// sleep(1);
	int *addr;
	addr = (int*) BACKING_STORE_BASE + bsm_id*0x00080000;
	kprintf("addr is %08x\n", addr);
	*addr = (struct mblock *) NULL;
	kprintf("in addr is %d\n", *addr);
	*(addr + 1) = hsize * NBPG;
	kprintf("in addr is %d\n", *(addr+1));
	struct mblock *mptr = (struct mblock *) BACKING_STORE_BASE + bsm_id*0x00080000;
	// struct mblock *mptr = (unsigned int) roundmb(4096 * NBPG);
	// struct mblock *mptr = getmem(sizeof(struct mblock));
	// mptr -> mlen = hsize * NBPG;
	// mptr -> mnext = NULL;
	proctab[pid].vmemlist -> mlen = hsize * NBPG;
	proctab[pid].vmemlist -> mnext = (struct mblock *) 0x1000000;	
	// mptr

	kprintf("To be implemented!\n");
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
