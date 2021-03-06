#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>



int globalPT[4];
int createPT(int pid)
{
	int *avail;
	if(get_frm(avail) < 0)
	{
		return -1;
	}
	frm_tab[*avail].fr_status = FRM_MAPPED;
	frm_tab[*avail].fr_pid = pid;
  	frm_tab[*avail].fr_refcnt = 0;
  	frm_tab[*avail].fr_type = FR_TBL;
  	frm_tab[*avail].fr_dirty = 0;
  	frm_tab[*avail].fr_loadtime = -1;
    int i;
  	for(i = 0; i < 1024; i++)
  	{
  		pt_t *pte = (*avail + FRAME0) * NBPG + i * sizeof(pt_t);
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
  	}
  	return *avail;
}

int createPD(int pid)
{
	int *avail;
	if(get_frm(avail) < 0)
	{
   // kprintf("no frame available for creating PD for proc %d\n", pid);
		return -1;
	}
	frm_tab[*avail].fr_status = FRM_MAPPED;
	frm_tab[*avail].fr_pid = pid;
  	// frm_tab[*avail].fr_vpno = FRAME0 + i;
  	frm_tab[*avail].fr_refcnt = 0;
  	frm_tab[*avail].fr_type = FR_DIR;
  	frm_tab[*avail].fr_dirty = 0;
  	frm_tab[*avail].fr_loadtime = 0;
  	proctab[pid].pdbr = (unsigned long) (FRAME0 + *avail) * NBPG;
  	int i;
  	for(i = 0; i < 1024; i++)
  	{
  		pd_t *pde = proctab[pid].pdbr + i * sizeof(pd_t);
  		pde -> pd_pres = 0;
  		pde -> pd_write = 1;
  		pde -> pd_user = 0;
  		pde -> pd_pwt = 0;
  		pde -> pd_pcd = 0;
  		pde -> pd_acc = 0;
      pde -> pd_fmb = 0;
  		pde -> pd_mbz = 0;
  		pde -> pd_global = 0;
  		pde -> pd_avail = 0;
  		pde -> pd_base = 0;
  		if(i < 4)
  		{
  			pde -> pd_pres = 1;
  			pde -> pd_base = globalPT[i];
  		}
  	}
  return *avail;
}

int create_global_PT()
{
	int i, j, frm;
	for(i = 0; i < 4; i++)
	{
		frm = createPT(0);
		globalPT[i] = FRAME0 + frm;
		for(j = 0; j < 1024; j++)
		{
			pt_t *pte = globalPT[i] * NBPG + j * sizeof(pt_t);
			pte -> pt_pres = 1;
			pte -> pt_write = 1;
			pte -> pt_base = i * 1024 + j;
			frm_tab[frm].fr_refcnt++;
		}
	}
	return OK;
}

int get_PD(int vaddr)
{
  return (unsigned int)vaddr >> 22;
}
int get_PT(int vaddr)
{
  return (unsigned int)(vaddr >> 12) & 0x000003ff;
}

