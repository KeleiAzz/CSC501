/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
	STATWORD ps;    
	struct	mblock	*p, *q, *leftover;

	disable(ps);
//	kprintf("into vmemlist %d %08x\n",proctab[currpid].vmemlist -> mlen, proctab[currpid].vmemlist -> mnext);
	if (nbytes==0 || nbytes > 128*NBPG || proctab[currpid].vmemlist -> mnext == (struct mblock *) NULL) {
//		kprintf("nbytes == 0 or mlen <= 0\n");
		restore(ps);
		return SYSERR;
	}
	nbytes = (unsigned int) roundmb(nbytes);
	p=proctab[currpid].vmemlist -> mnext;
	int len = p->mlen;
	
	for (q= proctab[currpid].vmemlist,p=proctab[currpid].vmemlist -> mnext;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		
		if ( p->mlen == nbytes) 
		{
			q->mnext = p->mnext;
			restore(ps);
			return( (WORD *)p );
		} 
		else if ( p->mlen > nbytes ) 
		{

			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			restore(ps);
			return( (WORD *)p );
		}
	restore(ps);
	// kprintf("To be implemented!\n");
	return SYSERR;
	
	// return( SYSERR );
}


