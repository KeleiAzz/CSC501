/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
  STATWORD ps;
  disable(ps);
  int i;

  unsigned long cr2 = read_cr2();
  
  kprintf("\ncr2:0x %08x\n", cr2);
  int store, pageth;
  if(bsm_lookup(currpid, cr2, &store, &pageth) == SYSERR)
  {
    // kprintf("bsm_lookup failed\n");
  	kill(currpid);
  	restore(ps);
  	return SYSERR;
  }
  kprintf("bsm_lookup successful, store: %d, pageth: %d %d \n", store, pageth, get_PD(cr2));
  pd_t *pde = proctab[currpid].pdbr + sizeof(pd_t)*(get_PD(cr2));
  kprintf("pid: %d, pd in frame %d, store: %d, pageth: %d\n", currpid, (unsigned int)pde/NBPG - FRAME0, store, pageth);
  if(pde -> pd_pres == 0) // if the pde not present, meaning the page table is not created
  {
  	kprintf("page table not present\n");
  	int pt_frm = createPT(currpid);
  	if(pt_frm == -1)
  	{	
  		kprintf("create pt fail\n");
  		return SYSERR;
  	}
  	pde -> pd_pres = 1;
  	pde -> pd_write = 1;
	   pde -> pd_base = pt_frm + FRAME0; //the frame the page table is on.
  kprintf("dala %d\n", pde -> pd_base);
	// frm_tab[pde -> pd_base - FRAME0].fr_refcnt ++;
  // kprintf("dala %d\n", (unsigned int)pde/NBPG);
  }
  int avail;
  if( get_frm(&avail) == SYSERR)
  {
  	kprintf("No frame available\n");
  	kill(currpid);
  	restore(ps);
  	return SYSERR;
  }
  
  frm_tab[avail].fr_status = FRM_MAPPED;
  frm_tab[avail].fr_pid = currpid;
  frm_tab[avail].fr_vpno = ((unsigned long)cr2)>>12;
  frm_tab[avail].fr_refcnt = 1;
  frm_tab[avail].fr_type = FR_PAGE;
  frm_tab[avail].fr_dirty = 1;
  frm_tab[avail].fr_loadtime = -1;
// kprintf("???\n");
  read_bs((avail + FRAME0) * NBPG, store, pageth);
  pt_t *pte = pde -> pd_base * NBPG + sizeof(pt_t)*(get_PT(cr2));
  pte -> pt_pres = 1;
  pte -> pt_write = 1;
  pte -> pt_dirty = 1;
  pte -> pt_base = avail + FRAME0;
  frm_tab[(unsigned int)pte/NBPG - FRAME0].fr_refcnt ++;
  kprintf("page fault handled!\n");
  write_cr3(proctab[currpid].pdbr);
  
  restore(ps);
  // kprintf("To be implemented!\n");
  return OK;
}


