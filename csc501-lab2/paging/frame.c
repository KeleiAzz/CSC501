/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
fr_map_t frm_tab[NFRAMES];
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
  // kprintf("To be implemented!\n");
	int i;
	for(i = 0; i < NFRAMES; i++)
  	{
  		frm_tab[i].fr_status = FRM_UNMAPPED;
  		frm_tab[i].fr_pid = -1;
  		frm_tab[i].fr_vpno = -1;
  		frm_tab[i].fr_refcnt = 0;
  		frm_tab[i].fr_type = -1;
  		frm_tab[i].fr_dirty = 0;
  		frm_tab[i].fr_loadtime = -1;
  	}

  	// return OK;
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  // kprintf("To be implemented!\n");
  // return OK;
	int i;
	for ( i = 0; i < NFRAMES; ++i)
	{
		if(frm_tab[i].fr_status == FRM_UNMAPPED)
		{
			*avail = i;
      kprintf("got frame %d\n", i);
      return i;
		} /* code */
	}
	return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  kprintf("To be implemented!\n");
  return OK;
}



