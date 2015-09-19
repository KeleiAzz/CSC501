#include<stdio.h>
#include<conf.h>
#include<kernel.h>
#include<proc.h>

static unsigned long *esp;
void printprocstks(int prior)
{
	
	kprintf("\n#4 printprocstk------------------------\n");
	struct pentry *proc;
	int i;
	kprintf("pname\tpid\tpriority\tstack base\tstack size\tstack limit\tstack pointer\n");
	for(i = 0; i < NPROC; i++)
	{
		proc = &proctab[i];
		if(proc->pstate != PRFREE && proc->pprio < prior)
		{	
			if(proc->pstate == PRCURR)
			{
				asm("movl %esp, esp");
				kprintf("%s\t%d\t%d\t%08x\t%d\t%08x\t%08x <---- current proc\n", proc->pname, i, proc->pprio, proc->pbase, proc->pstklen, proc->plimit, esp);
			}
			else
			{	
				esp = (unsigned long *)proc->pesp;
				kprintf("%s\t%d\t%d\t%08x\t%d\t%08x\t%08x\n", proc->pname, i, proc->pprio, proc->pbase, proc->pstklen, proc->plimit, esp);
			}
		}
	}
}
