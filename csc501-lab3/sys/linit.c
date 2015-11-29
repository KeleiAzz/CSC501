#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

struct lentry locks[NLOCKS];
int nextlock;]
int lock_tab[NPROC][NLOCKS];

void linit()
{
	struct lentry *lptr;
	int i, j;
	nextlock = NLOCKS - 1;
	for(i = 0; i < NLOCKS; i++)
	{
		lptr = &locks[i];
		lptr -> lstate = LFREE;
		lptr -> lockdes = i;
		lptr -> lqtail = 1 + (lptr -> lqhead = newqueue());
		lptr -> lreaders = 0;
		lptr -> lwriters = 0;
	}
	for( i = 0; i < NPROC; i++)
	{
		for( j = 0; j < NLOCKS; j++)
		{
			lock_tab[i][j] = 0;
		}
	}
}