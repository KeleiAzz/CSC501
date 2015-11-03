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
      // frm_tab[i].
      // kprintf("got frame %d\n", i);
      return i;
		} /* code */
	}
  // kprintf("all frames are mapped, need to swap out\n");
  if(page_replace_policy == FIFO)
  {
    int frm_id = fifo_head.next -> frm_id;
    fifo_node *swap_out = fifo_head.next;
    fifo_head.next = fifo_head.next -> next;
    freemem(swap_out, sizeof(fifo_node));
    // kprintf("got frame %d, now free this frame\n", frm_id + NFRAMES);
    free_frm(frm_id);
    *avail = frm_id;
    return frm_id;
  }
  if(page_replace_policy == LRU)
  {
    int time = 999999, frm_id;
    for(i = 0 ; i < NFRAMES; i++)
    {
      if(frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_acctime <= time)
      {
        time = frm_tab[i].fr_acctime;
        frm_id = i;
      }
     }
    *avail = frm_id;
    free_frm(frm_id);
    return frm_id;
  }
	return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
  if( i < 0 || i > NFRAMES)
  {
    return SYSERR;
  }
  if(frm_tab[i].fr_type == FR_PAGE)
  {
    int store, pageth;
    if( bsm_lookup(frm_tab[i].fr_pid, frm_tab[i].fr_vpno * NBPG, &store, &pageth) == SYSERR)
    {
      return SYSERR;
    }
    write_bs((i + FRAME0) * NBPG, store, pageth);
    pd_t *pde = proctab[frm_tab[i].fr_pid].pdbr + get_PD(frm_tab[i].fr_vpno * NBPG) * sizeof(pd_t);
    pt_t *pte = (pde -> pd_base) * NBPG + get_PT(frm_tab[i].fr_vpno * NBPG) * sizeof(pt_t);

    if(pte -> pt_pres == 0)
    {
      kprintf("pt present is 0!!\n");
      return SYSERR;
    }
    pte -> pt_pres = 0;
    pte -> pt_write = 1;
    pte -> pt_user = 0;
    pte -> pt_pwt = 0;
    pte -> pt_pcd = 0;
    pte -> pt_acc = 0;
    pte -> pt_dirty = 0;
    pte -> pt_mbz = 0;
    pte -> pt_global = 0;
    pte -> pt_avail = 0;
    pte -> pt_base = 0;

    frm_tab[i].fr_status = FRM_UNMAPPED;
    frm_tab[i].fr_pid = -1;
    frm_tab[i].fr_vpno = -1;
    frm_tab[i].fr_ref = 0;
    frm_tab[i].fr_type = -1;
    frm_tab[i].fr_dirty = 0;
    //frm_tab[i].fr_cookie
    frm_tab[i].fr_loadtime = -1;
    return OK;
  }
  // kprintf("To be implemented!\n");
  return OK;
}



