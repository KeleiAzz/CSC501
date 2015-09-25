#include <conf.h>
#include <kernel.h>
// #include <sched.h>
#include <lab1.h>
#include <stdio.h>


#define LOOP 50

int prA, prB, prC, prD;
int proc_a(), proc_b(), proc_c(), proc_d();
int procsleep(char c);
int proc(char c);
int proc_basic(char c);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;
volatile int d_cnt = 0;

int main() {
        int i;
        int count = 0;
        char buf[8];
        double total_cnt;
        double a_percent, b_percent, c_percent, d_percent;

        kprintf("\n\n########## Test Case1, linux-like scheduling(2 processes):\n");
        setschedclass(LINUXSCHED);
        resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));
        while (count++ < LOOP) {
                kprintf("M");

                for (i = 0; i < 10000000; i++);
        }
        kill(prC);

        count=0;
        kprintf("\n\n########## Test Case2, linux-like scheduling(4 processes):\n");
        resume(prA = create(proc, 2000, 5, "proc A", 1, 'A'));
        resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
        resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));
        while (count++ < LOOP) {
                kprintf("M");

                for (i = 0; i < 10000000; i++);
        }
        kill(prA);
        kill(prB);
        kill(prC);


        kprintf("\n\n########## Test Case3, multi-q scheduling(3 processes):\n");
        setschedclass(MULTIQSCHED);
        total_cnt=0;
        a_percent=0;
        b_percent=0;
        prA = create(proc_a, 2000, 100, "proc A", 1, 'A');
        prB = createReal(proc_b, 2000, 10, "proc B", 1, 'B');
        
        resume(prA);
        resume(prB);
        sleep(5);
        kill(prA);
        kill(prB);
        total_cnt = a_cnt + b_cnt ;
        a_percent = (double) a_cnt / total_cnt * 100;
        b_percent = (double) b_cnt / total_cnt * 100;
        kprintf("Test RESULT: A = %d, B = %d (%d : %d)\n", a_cnt,
                        b_cnt, (int) a_percent, (int) b_percent);

        
        kprintf("\n\n########## Test Case4, multi-q scheduling(4 processes):\n");
        total_cnt=0;
        a_percent=0;
        b_percent=0;
        c_percent=0;
        a_cnt=b_cnt=c_cnt=0;
        prA = create(proc_a, 2000, 100, "proc A", 1, 'A');
        prB = createReal(proc_b, 2000, 10, "proc B", 1, 'B');
        prC = createReal(proc_c, 2000, 10, "proc C", 1, 'C');
        resume(prA);
        resume(prB);
        resume(prC);
        sleep(10);
        kill(prA);
        kill(prB);
        kill(prC);
        total_cnt = a_cnt + b_cnt + c_cnt;
        a_percent = (double) a_cnt / total_cnt * 100;
        b_percent = (double) b_cnt / total_cnt * 100;
        c_percent = (double) c_cnt / total_cnt * 100;
        kprintf("Test RESULT: A = %d, B = %d, C = %d (%d : %d : %d)\n", a_cnt,
                        b_cnt, c_cnt, (int) a_percent, (int) b_percent, (int) c_percent);
}
proc_basic(char c) {
        int count = 0;

        while (count++ < 1000) {
                kprintf("%c", c);
        }
}

proc_a(c)
        char c; {
        int i;

        while (1) {
                for (i = 0; i < 10000; i++)
                        ;
                a_cnt++;
        }
}

proc_b(c)
        char c; {
        int i;

        while (1) {
                for (i = 0; i < 10000; i++)
                        ;
                b_cnt++;
        }
}
proc_c(c)
        char c; {
        int i;

        while (1) {
                for (i = 0; i < 10000; i++)
                        ;
                c_cnt++;
        }
}

proc_d(c)
        char c; {
        int i;

        while (1) {
                for (i = 0; i < 10000; i++)
                        ;
                d_cnt++;
        }
}
proc(char c) {
        int i;
        int count = 0;

        while (count++ < LOOP) {
                kprintf("%c", c);
                for (i = 0; i < 10000000; i++);
        }
}

