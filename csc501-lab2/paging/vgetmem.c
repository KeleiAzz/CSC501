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
	if (nbytes==0 || proctab[currpid].vmemlist -> mlen <= 0) {
		kprintf("nbytes == 0 or mlen <= 0\n");
		restore(ps);
		return( (WORD *)SYSERR);
	}
	nbytes = (unsigned int) roundmb(nbytes);

	p = &proctab[currpid].vmemlist;
	kprintf("p addr %d nbytes %d mlen %d\n",p, nbytes, proctab[currpid].vmemlist -> mlen);
	if(proctab[currpid].vmemlist -> mlen >= nbytes)
	{
		proctab[currpid].vmemlist -> mlen -= nbytes;
		restore(ps);

		// return ((WORD *) 4096 * NBPG + (proctab[currpid].vhpnpages * NBPG - p -> mlen));
		return((WORD ) 4096 * NBPG);
	}
	

	// for (q= &proctab[currpid].vmemlist,p=proctab[currpid].vmemlist -> mnext ;
	//      p != (struct mblock *) NULL ;
	//      q=p,p=p->mnext)
	// 	kprintf("in vgetmen\n");
	// 	if ( p->mlen == nbytes) {
	// 		q->mnext = p->mnext;
	// 		restore(ps);
	// 		return( (WORD *)p );
	// 	} else if ( p->mlen > nbytes ) {
	// 		leftover = (struct mblock *)( (unsigned)p + nbytes );
	// 		q->mnext = leftover;
	// 		leftover->mnext = p->mnext;
	// 		leftover->mlen = p->mlen - nbytes;
	// 		restore(ps);
	// 		return( (WORD *)p );
	// 	}
	restore(ps);
	return( (WORD *)SYSERR );
	kprintf("To be implemented!\n");
	return( SYSERR );
}


