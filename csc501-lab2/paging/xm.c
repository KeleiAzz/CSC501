/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  /* sanity check ! */

  if ( (virtpage < 4096) || ( source < 0 ) || ( source > MAX_ID) ||(npages < 1) || ( npages > NPGS)){
//	kprintf("xmmap call error: parameter error! \n");
	return SYSERR;
  }
  STATWORD ps;
  disable(ps);
  if(bsm_tab[source].bs_status == BSM_UNMAPPED)
  {
 //   kprintf("This bs %d is empty\n", source);
    restore(ps);
    return SYSERR;
  }
  if(bsm_tab[source].bs_npages < npages)
  {
  //  kprintf("Not enough space in bs\n");
    restore(ps);
    return SYSERR;
  }
  if(bsm_map(currpid, virtpage, source, npages) == SYSERR)
  {
   // kprintf("xmmap. failed bsm_map.\n");
    restore(ps);
    return SYSERR;
  }
  // kprintf("xmmap successful, virtpage: %d\n", virtpage);
  restore(ps);
  return OK;
  // kprintf("xmmap - to be implemented!\n");
  // return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
  /* sanity check ! */
  if ( (virtpage < 4096) ){ 
//	kprintf("xmummap call error: virtpage (%d) invalid! \n", virtpage);
	return SYSERR;
  }
  if(bsm_unmap(currpid, virtpage, 0) == SYSERR)
  {
 //   kprintf("unmap failed\n");  
    return SYSERR;
  }
  else return OK;
}

