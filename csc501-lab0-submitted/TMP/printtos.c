#include<stdio.h>
#include<conf.h>
#include<kernel.h>

static unsigned long *esp;
void printtos()
{
	kprintf("\n#3 printtos()----------------------\n");
	asm("movl %esp, esp");
	kprintf("Right before function call: %08x\n", esp+2);
	kprintf("Right after function call: %08x\n", esp);
    int i = 1;
    for(i; i <= 6 ; i++)
    {
         kprintf("%d addresses below top of stack: %08x, content: %X\n", i, esp+i, *(esp+i));
    }
}
