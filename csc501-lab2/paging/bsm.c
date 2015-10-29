/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>
bs_map_t bsm_tab[NBS];
// int bs_pid_map[NPROC];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	int i;
	for(i = 0; i < NBS; i++)
  	{
  		bsm_tab[i].bs_status = BSM_UNMAPPED;
  		bsm_tab[i].bs_pid = 1;
  		bsm_tab[i].bs_vpno = -1;
  		bsm_tab[i].bs_npages = 0;
  		bsm_tab[i].bs_sem = -1;
  		bsm_tab[i].private = 0;
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
	for ( i = 0; i < NBS; ++i)
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
  kprintf("%d bsm_lookup vpno %d\n", vpno, vv);
  int i;
  for(i = 0; i < NBS; i++)
  {
    if(proctab[pid].bs_pid_map[i].bs_status == BSM_MAPPED && \
      proctab[pid].bs_pid_map[i].bs_vpno <= vpno && \
      proctab[pid].bs_pid_map[i].bs_vpno + proctab[pid].bs_pid_map[i].bs_npages > vpno)
    {
      *store = i;
      *pageth = vpno - proctab[pid].bs_pid_map[i].bs_vpno;
      return OK;
    }
  }
  *store = -1;
  *pageth = -1;
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
    return SYSERR;
  }
  if(bsm_tab[source].bs_status == BSM_UNMAPPED)
  {
    bsm_tab[source].bs_status = BSM_MAPPED;
    bsm_tab[source].bs_npages = npages;
    proctab[pid].bs_pid_map[source].bs_status = BSM_MAPPED;
    proctab[pid].bs_pid_map[source].bs_pid = pid;
    proctab[pid].bs_pid_map[source].bs_vpno = vpno;
    proctab[pid].bs_pid_map[source].bs_npages = npages;
    proctab[pid].bs_pid_map[source].bs_sem = -1;
    proctab[pid].bs_pid_map[source].private = 0;
    return OK;
    // bsm_tab[source].pid *= bs_pid_map[pid];
  }
  else if(bsm_tab[source].bs_status == BSM_MAPPED && bsm_tab[source].private == 0 && proctab[pid].bs_pid_map[source].bs_pid != pid)
  { //If this bsm is mapped by other process but is not private heap, and haven't mapped by pid, still can be mapped
    proctab[pid].bs_pid_map[source].bs_status = BSM_MAPPED;
    proctab[pid].bs_pid_map[source].bs_pid = pid;
    proctab[pid].bs_pid_map[source].bs_vpno = vpno;
    proctab[pid].bs_pid_map[source].bs_npages = npages;
    proctab[pid].bs_pid_map[source].bs_sem = -1;
    proctab[pid].bs_pid_map[source].private = 0;
  }
  else return SYSERR;
  
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
  int *store, *pageth;
  if(bsm_lookup(pid, vpno * NPGS, store, pageth) == SYSERR)
  {
    //no mapping to vpno for pid
    return SYSERR;
  }
  


}


