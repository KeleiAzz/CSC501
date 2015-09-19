/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

extern long zfunction(long param);
int main()
{	
	syscallsummary_start();
	gettime(2500);
	getpid();
	kprintf("\n\nHello World, Xinu lives\n\n");
	kprintf("%08x\n", zfunction(0xaabbccdd));
	printsegaddress();
	printtos();
	printprocstks(30);
	sleep(1);
	sleep(1);
	getpid();
	getpid();
	getpid();
	syscallsummary_stop();
	printsyscalls();
	return 0;
}
