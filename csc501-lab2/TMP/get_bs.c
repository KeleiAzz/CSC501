#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
	STATWORD ps;
	if(bs_id < 0 || bs_id > MAX_ID)
	{
		return SYSERR;
	}

	if(npages <= 0 || npages > NPGS)
	{
		return SYSERR;
	}

	disable(ps);
	if(bsm_tab[bs_id].private == 1 )
	{
		restore(ps);
		return SYSERR;
	}

	if(bsm_tab[bs_id].bs_status == BSM_MAPPED)
	{
		restore(ps);
		return bsm_tab[bs_id].bs_npages;
	}

	if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED)
	{
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
		bsm_tab[bs_id].bs_npages = npages;
	}

    return npages;

}


