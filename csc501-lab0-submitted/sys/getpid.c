/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
extern int getpid_count[NPROC];
extern int trace;
SYSCALL getpid()
{
	if(trace) getpid_count[currpid]++;
	return(currpid);
}
