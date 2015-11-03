#include <conf.h>
#include <kernel.h>
#include <proc.h>
// #include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
	if(bsm_tab[bs_id].bs_status == BSM_MAPPED && bsm_tab[bs_id].bs_num_proc == 0)
	{
		bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
  		bsm_tab[bs_id].bs_pid = 1;
  		bsm_tab[bs_id].bs_vpno = -1;
  		bsm_tab[bs_id].bs_npages = 0;
  		bsm_tab[bs_id].bs_sem = -1;
  		bsm_tab[bs_id].private = 0;
  		// kprintf("Release bs successful\n");
  		return OK;
	}
	else
	{
		kprintf("this bs %d is not mapped or there are processes mapped to this bs\n", bs_id);
		return SYSERR;
	}
   

}

