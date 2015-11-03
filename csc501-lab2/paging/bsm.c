/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>
bs_map_t bsm_tab[MAX_ID+1];
// int bs_pid_map[NPROC];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	int i;
	for(i = 0; i <= MAX_ID; i++)
  	{
  		bsm_tab[i].bs_status = BSM_UNMAPPED;
  		bsm_tab[i].bs_pid = 1;
  		bsm_tab[i].bs_vpno = -1;
  		bsm_tab[i].bs_npages = 0;
  		bsm_tab[i].bs_sem = -1;
  		bsm_tab[i].private = 0;
      bsm_tab[i].bs_num_proc = 0;
  	}

  	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	int i;
	for ( i = 0; i <= MAX_ID; ++i)
	{
		if(bsm_tab[i].bs_status == BSM_UNMAPPED)
		{
			*avail = i;
			return i;
		}
		/* code */
	}
  return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	  if( bsm_tab[i].bs_status == BSM_UNMAPPED || bsm_tab[i].bs_num_proc != 0)
    {
      return SYSERR;
    }
    // bsm_tab[i].bs_num_proc -= 1;
    
      bsm_tab[i].bs_status = BSM_UNMAPPED;
      bsm_tab[i].bs_pid = -1;
      bsm_tab[i].bs_vpno = -1;
      bsm_tab[i].bs_npages = 0;
      bsm_tab[i].bs_sem = -1;
      bsm_tab[i].private = 0;
    

  	return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	int  vpno = ((unsigned long)vaddr)>>12;
  int vv = (vaddr/4096) & 0x000fffff;
  // kprintf("%d bsm_lookup vpno %d\n", vpno, vv);
  int i;
  for(i = 0; i <= MAX_ID; i++)
  {
    if(proctab[pid].bs_pid_map[i].bs_status == BSM_MAPPED && proctab[pid].bs_pid_map[i].bs_vpno <= vpno && proctab[pid].bs_pid_map[i].bs_vpno + proctab[pid].bs_pid_map[i].bs_npages > vpno)
    {
      *store = i;
      *pageth = vpno - proctab[pid].bs_pid_map[i].bs_vpno;
      // kprintf("bsm_lookup successful, store: %d, pageth: %d %d \n", *store, *pageth, i);
      // kprintf("%d %d %d %d \n",proctab[pid].bs_pid_map[0].bs_status, proctab[pid].bs_pid_map[1].bs_status,proctab[pid].bs_pid_map[2].bs_status,proctab[pid].bs_pid_map[3].bs_status);
      return OK;
    }
  }
  *store = -1;
  *pageth = -1;
//  kprintf("bsm lookup failed %d %d\n", pid, proctab[pid].bs_pid_map[0].bs_status);
  return SYSERR;

}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
  if(vpno < 4096 || source < 0 || source > MAX_ID || npages < 0 || npages > NPGS)
  {
    //kprintf("failed in first if\n");
    return SYSERR;
  }
  if(bsm_tab[source].bs_status == BSM_UNMAPPED)
  {
    bsm_tab[source].bs_status = BSM_MAPPED;
    bsm_tab[source].bs_npages = npages;
    bsm_tab[source].bs_num_proc += 1;
    proctab[pid].bs_pid_map[source].bs_status = BSM_MAPPED;
    proctab[pid].bs_pid_map[source].bs_pid = pid;
    proctab[pid].bs_pid_map[source].bs_vpno = vpno;
    proctab[pid].bs_pid_map[source].bs_npages = npages;
    proctab[pid].bs_pid_map[source].bs_sem = -1;
    proctab[pid].bs_pid_map[source].private = 0;

    // kprintf("(unmapped)bsm map successful, pid: %d, vpno: %d source %d npages: %d, addr: %08x\n", pid, vpno, source, npages, vpno*NBPG);
    return OK;
    // bsm_tab[source].pid *= bs_pid_map[pid];
  }
  else if(bsm_tab[source].bs_status == BSM_MAPPED && bsm_tab[source].private == 0)
  { //If this bsm is mapped by other process but is not private heap, and haven't mapped by pid, still can be mapped
    bsm_tab[source].bs_num_proc += 1;
    proctab[pid].bs_pid_map[source].bs_status = BSM_MAPPED;
    proctab[pid].bs_pid_map[source].bs_pid = pid;
    proctab[pid].bs_pid_map[source].bs_vpno = vpno;
    proctab[pid].bs_pid_map[source].bs_npages = npages;
    proctab[pid].bs_pid_map[source].bs_sem = -1;
    proctab[pid].bs_pid_map[source].private = 0;
    // kprintf("(mapped)bsm map successful, pid: %d, vpno: %d source %d npages: %d\n", pid, vpno, source, npages);
    return OK;
  }
  else 
  {
    kprintf("failed in the last else %d %d %d\n", bsm_tab[source].bs_status, bsm_tab[source].private, proctab[pid].bs_pid_map[source].bs_pid);
    return SYSERR;  
  }
  
  
  // proctab[pid].bs_pid_map[source].bs_ref = 0;

}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
  if(vpno < 4096)
  {
    return SYSERR;
  }
  int store, pageth;
  // kprintf("In bsm_unmap, pid: %d vpno: %d, vaddr: %d\n", pid, vpno, vpno*NBPG);
  if(bsm_lookup(pid, vpno * NBPG, &store, &pageth) == SYSERR)
  {
    //no mapping to vpno for pid
    // kprintf("no mapping to vpno %d for pid %d\n", vpno, pid);
    return SYSERR;
  }
  else
  {
    // write dirty page to bs
    int j;
    for(j = 0; j < NFRAMES; j ++)
    {
    //if((frm_tab[i].fr_pid == currpid) && (frm_tab[i].fr_status == MAPPED) && (frm_tab[i].fr_type == FR_PAGE))
      if((frm_tab[j].fr_status == FRM_MAPPED) && (frm_tab[j].fr_type == FR_PAGE) && (frm_tab[j].fr_pid == pid) && (frm_tab[j].fr_dirty == 1))
      {
          int tmp_store, tmp_pageth;
          if(bsm_lookup(frm_tab[j].fr_pid, frm_tab[j].fr_vpno * NBPG, &tmp_store, &tmp_pageth) != SYSERR) 
          {
          //kprintf("pfint. bsm_lookup fail when writing back frame %d.\n", i);
          //return SYSERR;
            write_bs((j + FRAME0)*NBPG, tmp_store, tmp_pageth);
            // kprintf("write to bs for proc %d, in %d %d %d\n", frm_tab[j].fr_pid, tmp_store, tmp_pageth, currpid);
          // frm_tab[i].fr_status = FRM_UNMAPPED;
          // pt_t *pte
          }
          
      }
    }
    bsm_tab[store].bs_num_proc -= 1;
    // kprintf("in bsm unmap, num proc of bs %d is %d\n", store,bsm_tab[store].bs_num_proc);
    proctab[pid].bs_pid_map[store].bs_status = BSM_UNMAPPED;
    proctab[pid].bs_pid_map[store].bs_pid = -1;
    proctab[pid].bs_pid_map[store].bs_vpno = -1;
    proctab[pid].bs_pid_map[store].bs_npages = 0;
    proctab[pid].bs_pid_map[store].bs_sem = -1;
    proctab[pid].bs_pid_map[store].private = 0; 
    return OK;
  }

  


}


