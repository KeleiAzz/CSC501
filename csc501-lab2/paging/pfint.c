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
  if(page_replace_policy == LRU)
  {
    pd_t *pde0; 
    pt_t *pte0;
    for(i = 0; i < NFRAMES; i++)
    {
      if(frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_pid == currpid)
      {
       int vaddr = frm_tab[i].fr_vpno * NBPG;
        pde0 = proctab[frm_tab[i].fr_pid].pdbr + (unsigned int)(vaddr>>22) * sizeof(pd_t);
        pte0 = (pde0 -> pd_base) * NBPG + ((unsigned int)(vaddr>>12) & 0x000003ff) * sizeof(pt_t);
        if(pte0 -> pt_acc == 1)
         {
          frm_tab[i].fr_acctime = timeCount;
          pte0 -> pt_acc = 0;
         }
       }
     }
    timeCount += 1;
  }
  int store, pageth;
  if(bsm_lookup(currpid, cr2, &store, &pageth) == SYSERR)
  {
  	kill(currpid);
  	restore(ps);
  	return SYSERR;
  }
  pd_t *pde = proctab[currpid].pdbr + sizeof(pd_t)*(get_PD(cr2));
  if(pde -> pd_pres == 0) // if the pde not present, meaning the page table is not created
  {
  	int pt_frm = createPT(currpid);
  	if(pt_frm == -1)
  	{	
  		return SYSERR;
  	}
  	pde -> pd_pres = 1;
  	pde -> pd_write = 1;
	  pde -> pd_base = pt_frm + FRAME0; //the frame the page table is on.
  }
  int avail;
  if( get_frm(&avail) == SYSERR)
  {
  	kill(currpid);
  	restore(ps);
  	return SYSERR;
  }
  // kprintf("got frame %d\n", avail);
  frm_tab[avail].fr_status = FRM_MAPPED;
  frm_tab[avail].fr_pid = currpid;
  frm_tab[avail].fr_vpno = ((unsigned long)cr2)>>12;
  frm_tab[avail].fr_refcnt = 1;
  frm_tab[avail].fr_type = FR_PAGE;
  frm_tab[avail].fr_dirty = 1;
  frm_tab[avail].fr_loadtime = -1;
  if(page_replace_policy == FIFO)
  {
    fifo_node *new = getmem(sizeof(fifo_node));
    fifo_node *node = &fifo_head;
    while(node -> next !=  NULL)
    {
       node = node -> next;
    }
    new -> frm_id = avail;
    new -> next = (struct fifo_node*)NULL;
    node -> next = new;
  }
  read_bs((avail + FRAME0) * NBPG, store, pageth);
  pt_t *pte = pde -> pd_base * NBPG + sizeof(pt_t)*(get_PT(cr2));
  pte -> pt_pres = 1;
  pte -> pt_write = 1;
  pte -> pt_dirty = 1;
  pte -> pt_base = avail + FRAME0;
  frm_tab[(unsigned int)pte/NBPG - FRAME0].fr_refcnt ++;

  write_cr3(proctab[currpid].pdbr);
  
  restore(ps);
  return OK;
}


