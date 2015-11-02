/* user.c - main */ 
 
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

void halt();

#define TPASSED    1
#define TFAILED    0

#define MYVADDR1   0x40000000
#define MYVPNO1    0x40000
#define MYVADDR2   0x80000000
#define MYVPNO2    0x80000
#define MYBS1      1
#define MAX_BSTORE 16

#ifndef NBPG
#define NBPG       4096
#endif

#define assert(x,error) if(!(x)){ \
      kprintf(error);\
      return;\
      }
      
void test1()
{
  kprintf("\nTest 1: Testing xmmap.\n");
  char *addr1 = (char*)0x40000000; 
  int i = ((unsigned long)addr1) >> 12; 
  
  get_bs(MYBS1, 100);
  if (xmmap(i, MYBS1, 100) == SYSERR) {
    kprintf("xmmap call failed\n");
    kprintf("\tFAILED!\n");
    return;
  }

  for (i = 0; i < 26; i++) {
    *addr1 = 'A'+i;   //trigger page fault for every iteration
    addr1 += NBPG;    //increment by one page each time
  }

  addr1 = (char*)0x40000000;
  for (i = 0; i < 26; i++) {
    if (*addr1 != 'A'+i) {
      kprintf("\tFAILED!\n");
      return;
    }
    addr1 += NBPG;    //increment by one page each time
  }

  xmunmap(0x40000000>>12);
  release_bs(MYBS1);
  kprintf("\tPASSED!\n");
  return;
}
/*----------------------------------------------------------------*/
void proc_test2(int i,int j,int* ret,int s) {
  char *addr;
  int bsize;
  int r;
  bsize = get_bs(i, j);
  if (bsize != 50)  //????
    {
      // kprintf("failed here!!\n");
        *ret = TFAILED;
      }
  r = xmmap(MYVPNO1, i, j);
  if (j<=50 && r == SYSERR){
    *ret = TFAILED;
    // kprintf("!failed here!!\n");
  }
  if (j> 50 && r != SYSERR){
    // kprintf("!!failed here!!\n");
    *ret = TFAILED;
  }
  sleep(s);
  if (r != SYSERR)
    xmunmap(MYVPNO1);
  release_bs(i);
  // kprintf("ret = %d <---------=================== %d %d %d %d\n", *ret, currpid, bsize, r, j);
  return;
}
void test2() {
  int pids[16];
  int mypid;
  int i,j;

  int ret = TPASSED;
  kprintf("\nTest 2: Testing backing store operations\n");

  int bsize = get_bs(1, 100);
  if (bsize != 100)
    {kprintf("failed here!!\n");
        ret = TFAILED;}
  release_bs(1);

  bsize = get_bs(1, 130);
  if (bsize != SYSERR)
  {kprintf("failed here!!!\n");
    ret = TFAILED;}

  bsize = get_bs(1, 0);
  if (bsize != SYSERR)
    {kprintf("failed here!!!!!\n");
        ret = TFAILED;}
  // kprintf("ret = %d <==============================\n", ret);
  mypid = create(proc_test2, 2000, 20, "proc_test2", 4, 1,
                 50, &ret, 4);
  resume(mypid);
  sleep(2);
  for(i=1;i<=5;i++){
    pids[i] = create(proc_test2, 2000, 20, "proc_test2", 4, 1,
                     i*20, &ret, 0);
    resume(pids[i]);
  }
  sleep(3);
  kill(mypid);
  for(i=1;i<=5;i++){
    kill(pids[i]);
  }
  if (ret != TPASSED)
    kprintf("\tFAILED!\n");
  else
    kprintf("\tPASSED!\n");
}
/*-------------------------------------------------------------------------------------*/
void proc1_test3(int i,int* ret) {
  char *addr;
  int bsize;
  int r;

  get_bs(i, 100);
  
  if (xmmap(MYVPNO1, i, 100) == SYSERR) {
    *ret = TFAILED;
    return 0;
  }
  sleep(4);
  xmunmap(MYVPNO1);
  release_bs(i);
  return;
}
void proc2_test3() {
  /*do nothing*/
  sleep(1);
  return;
}
void test3() {
  int pids[16];
  int mypid;
  int i,j;

  int ret = TPASSED;
  kprintf("\nTest 3: Private heap is exclusive\n");

  for(i=0;i<=15;i++){
    pids[i] = create(proc1_test3, 2000, 20, "proc1_test3", 2, i,&ret);
    if (pids[i] == SYSERR){
      ret = TFAILED;
    }else{
      resume(pids[i]);
    }
  }
  sleep(1);
  mypid = vcreate(proc2_test3, 2000, 100, 20, "proc2_test3", 0, NULL);
  if (mypid != SYSERR)
    ret = TFAILED;

  for(i=0;i<=15;i++){
    kill(pids[i]);
  }
  if (ret != TPASSED)
    kprintf("\tFAILED!\n");
  else
    kprintf("\tPASSED!\n");
}
/*-------------------------------------------------------------------------------------*/
void proc1_test4(int* ret) {
  char *addr;
  int i;

  get_bs(MYBS1, 100);

  if (xmmap(MYVPNO1, MYBS1, 100) == SYSERR) {
    kprintf("xmmap call failed\n");
    *ret = TFAILED;
    sleep(3);
    return;
  }
  kprintf("in proc 1\n");
  addr = (char*) MYVADDR1;
  for (i = 0; i < 26; i++) {
    *(addr + i * NBPG) = 'A' + i;
  }
  sleep(7);
  kprintf("%d failed??%d\n ", *ret, proctab[47].pstate);
  /*Shoud see what proc 2 updated*/
  for (i = 0; i < 26; i++) {
    /*expected output is abcde.....*/
    kprintf("-->%c\n", *(addr + i * NBPG));
    if (*(addr + i * NBPG) != 'a'+i){
      *ret = TFAILED;
      // break;    
    }
  }

  xmunmap(MYVPNO1);
  release_bs(MYBS1);
  return;
}
void proc2_test4(int *ret) {
  char *addr;
  int i;

  get_bs(MYBS1, 100);

  if (xmmap(MYVPNO2, MYBS1, 100) == SYSERR) {
    kprintf("xmmap call failed\n");
    *ret = TFAILED;
    sleep(3);
    return;
  }
  kprintf("in proc 2\n");
  addr = (char*) MYVADDR2;

  /*Shoud see what proc 1 updated*/
  for (i = 0; i < 26; i++) {
    /*expected output is ABCDEF.....*/
    kprintf("%c\n", *(addr + i * NBPG));
    if (*(addr + i * NBPG) != 'A'+i){
      *ret = TFAILED;
      break;
    }
  }
  kprintf("%d failed proc 2??\n ", *ret);
  /*Update the content, proc1 should see it*/

  for (i = 0; i < 26; i++) {
    *(addr + i * NBPG) = 'a' + i;
    kprintf("proc 2-->%c\n", *(addr + i * NBPG));
  }
  // sleep(3);
  // for (i = 0; i < 26; i++) {
  //   // *(addr + i * NBPG) = 'a' + i;
  //   kprintf("proc 2-->%c\n", *(addr + i * NBPG));
  // }
  xmunmap(MYVPNO2);
  release_bs(MYBS1);
  return;
}
void test4() {
  int pid1;
  int pid2;
  int ret = TPASSED;
  kprintf("\nTest 4: Shared backing store\n");

  pid1 = create(proc1_test4, 2000, 20, "proc1_test4", 1, &ret);
  pid2 = create(proc2_test4, 2000, 20, "proc2_test4", 1, &ret);

  resume(pid1);
  sleep(4);
  resume(pid2);

  sleep(10);
  kill(pid1);
  kill(pid2);
  // for (i = 0; i < 26; i++) {
  //   *(addr + i * NBPG) = 'a' + i;
  //   kprintf("proc 2-->%c\n", *(addr + i * NBPG));
  // }
  // kprintf("%x\n", (char **)BACKING_STORE_BASE + (1<<19) + 0*NBPG);
  // kprintf("%x\n", (char **)BACKING_STORE_BASE + (1<<19) + 1*NBPG);
  if (ret != TPASSED)
    kprintf("\tFAILED!\n");
  else
    kprintf("\tPASSED!\n");
}
/*-------------------------------------------------------------------------------------*/
void proc1_test5(int* ret) {
  int *x;
  int *y;
  int *z;

  //kprintf("ready to allocate heap space\n");
  x = vgetmem(1024);
  if ((x == NULL) || (x < 0x1000000)
      || (x > 0x1000000 + 128 * NBPG - 1024)) {
    kprintf("x= %08x\n", x);
    *ret = TFAILED;
  }
  if (x == NULL)
    return;

  *x = 100;
  *(x + 1) = 200;

  if ((*x != 100) || (*(x+1) != 200)) {
    kprintf("failed x+1\n");
    *ret = TFAILED;
  }
  vfreemem(x, 1024);

  x = vgetmem(129*NBPG); //try to acquire a space that is bigger than size of one backing store
  if (x != NULL) {
    kprintf("failed 129\n");
    *ret = TFAILED;
  }

  x = vgetmem(50*NBPG);
  kprintf("x= %08x\n", x);
  y = vgetmem(50*NBPG);
  kprintf("y= %08x\n", y);
  z = vgetmem(50*NBPG);
  kprintf("z= %08x\n", z);
  if ((x == NULL) || (y == NULL) || (z != NULL)){
    *ret = TFAILED;
    if (x != NULL) vfreemem(x, 50*NBPG);
    if (y != NULL) vfreemem(y, 50*NBPG);
    if (z != NULL) vfreemem(z, 50*NBPG);
    return;
  }
  vfreemem(y, 50*NBPG);
  z = vgetmem(50*NBPG);
  if (z == NULL){
    *ret = TFAILED;
  }
  if (x != NULL) vfreemem(x, 50*NBPG);
  if (z != NULL) vfreemem(z, 50*NBPG);
  return;


}
void test5() {
  int pid1;
  int ret = TPASSED;

  kprintf("\nTest 5: vgetmem/vfreemem\n");
  pid1 = vcreate(proc1_test5, 2000, 100, 20, "proc1_test5", 1, &ret);

  //kprintf("pid %d has private heap\n", pid1);
  resume(pid1);
  sleep(3);
  kill(pid1);
  if (ret != TPASSED)
    kprintf("\tFAILED!\n");
  else
    kprintf("\tPASSED!\n");
}
/*-------------------------------------------------------------------------------------*/
void proc1_test6(int *ret) {

  char *vaddr, *addr0, *addr_lastframe, *addr_last;
  int i, j;
  int tempaddr;
  int addrs[1200];

  int maxpage = (NFRAMES - (5 + 1 + 1 + 1));
  
  int vaddr_beg = 0x40000000;
  int vpno;
  
  for(i = 0; i < MAX_BSTORE; i++){
    tempaddr = vaddr_beg + 100 * NBPG * i; 
    vaddr = (char *) tempaddr;
    vpno = tempaddr >> 12;
    get_bs(i, 100);
    if (xmmap(vpno, i, 100) == SYSERR) {
      *ret = TFAILED;
      kprintf("xmmap call failed\n");
      sleep(3);
      return;
    }

    for (j = 0; j < 100; j++) {
      *(vaddr + j * NBPG) = 'A' + i;
    }
    
    for (j = 0; j < 100; j++) {
      if (*(vaddr + j * NBPG) != 'A'+i){
        *ret = TFAILED;
        break;
      }
    }    
    xmunmap(vpno);
    release_bs(i);    
  }

  return;
}

void test6(){
  int pid1;
  int ret = TPASSED;
  kprintf("\nTest 6: Stress testing\n");

  pid1 = create(proc1_test6, 2000, 50, "proc1_test6",1,&ret);

  resume(pid1);
  sleep(4);
  kill(pid1);
  if (ret != TPASSED)
    kprintf("\tFAILED!\n");
  else
    kprintf("\tPASSED!\n");
}

int main() {
  kprintf("\n\nHello World, Xinu lives\n\n");
 
  // test1();
  // test2();
  // test3();

  // test4();
  test5();
  // test6();

  return 0;
}