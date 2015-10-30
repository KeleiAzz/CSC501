/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>
bs_map_t bsm_tab[MAX_ID];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	int i;
	for(i = 0; i < NFRAMES; i++)
  	{
  		bsm_tab[i].bs_status = BSM_UNMAPPED;
  		bsm_tab[i].bs_pid = -1;
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
	for ( i = 0; i < NFRAMES; ++i)
	{
		if(bsm_tab[i].bs_status == BSM_UNMAPPED)
		{
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
	
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}

