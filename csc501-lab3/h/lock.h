/* lock.h */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCKS
#define	NLOCKS		50	/* number of locks, if not defined	*/
#endif

#define READ	1
#define WRITE	2

#define	LFREE	'\01'		/* this lock is free		*/
#define	LUSED	'\02'		/* this lock is used		*/

struct	lentry	{		/* lock table entry		*/
	char	lstate;		/* the state SFREE or SUSED		*/
	int	lockcnt;		/* count for this lock		*/
	int	lqhead;		/* q index of head of list		*/
	int	lqtail;		/* q index of tail of list		*/
	int lockdes;
	int lreaders;
	int lwriters;
};
extern	struct	lentry	locks[];
extern	int	nextlock;
extern	int lock_tab[][NLOCKS];


#define	isbadlock(s)	(s<0 || s>=NLOCKS)

void linit();
int lcreate();
int ldelete(int lockdescriptor);
int lock(int ldes1, int type, int priority);
int releaseall(int numlocks, int ldes1, ...);

#endif
