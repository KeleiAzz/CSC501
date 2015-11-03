/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
  /* sanity check ! */
	page_replace_policy = policy;
  kprintf("set policy to be %d\n", policy);
	if(policy == FIFO)
	{
		int i;
		for(i = 4; i < NFRAMES; i++)
		{
			if(frm_tab[i].fr_type == FR_PAGE || frm_tab[i].fr_type == FR_TBL)
			{
				frm_tab[i].fr_status = FRM_UNMAPPED;
			}
		}
	}
  return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
