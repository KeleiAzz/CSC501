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
		return( (WORD *)NULL);
	}
	nbytes = (unsigned int) roundmb(nbytes);

	// p = &proctab[currpid].vmemlist;
	// kprintf("p addr %d nbytes %d mlen %d\n",p, nbytes, proctab[currpid].vmemlist -> mlen);
	// if(proctab[currpid].vmemlist -> mlen >= nbytes)
	// {
	// 	proctab[currpid].vmemlist -> mlen -= nbytes;
	// 	restore(ps);

	// 	// return ((WORD *) 4096 * NBPG + (proctab[currpid].vhpnpages * NBPG - p -> mlen));
	// 	return((WORD ) 4096 * NBPG);
	// }
	
	p=proctab[currpid].vmemlist -> mnext;
	int len = p->mlen;
	// if(p == proctab[currpid].vmemlist -> mnext) kprintf("in vgetmen %08x %d\n", p, p->mlen);
	for (q= proctab[currpid].vmemlist,p=proctab[currpid].vmemlist -> mnext;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		// if(p == proctab[currpid].vmemlist -> mnext) kprintf("in vgetmen %08x %d\n", p, p->mlen);
		// if(p == proctab[currpid].vmemlist -> mnext) kprintf("in vgetmen %08x %d\n", p, p->mlen);
		// kprintf("in vgetmen%08x\n", p);
		
		// kprintf("	in vgetmen%08x\n", p);
		
		if ( p->mlen == nbytes) 
		{
			q->mnext = p->mnext;
			restore(ps);
			return( (WORD *)p );
		} 
		else if ( p->mlen > nbytes ) 
		{
//			kprintf("in vgetmen %08x %d\n", p, p->mlen);
			leftover = (struct mblock *)( (unsigned)p + nbytes );
//			kprintf("in vgetmen%08x\n", leftover);
			q->mnext = leftover;
//			kprintf("in vgetmen%08x\n", q->mnext);
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			restore(ps);
			return( (WORD *)p );
		}
	restore(ps);
	// kprintf("To be implemented!\n");
	return( (WORD *)NULL );
	
	// return( SYSERR );
}


