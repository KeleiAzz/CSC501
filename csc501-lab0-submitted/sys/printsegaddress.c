#include <stdio.h>

extern int etext, edata, end;
void printsegaddress(){
	kprintf("\n#2 Printsegaddress--------------------------\n");
	kprintf("Current address:\n");
	kprintf("etext: %08x, edata: %08x, bss: %08x\n", &etext, &edata, &end);
	kprintf("\nCurrent content:\n");
	kprintf("etext: %08x, edata: %08x, bss: %08x\n", etext, edata, end);
	kprintf("\nPreceding address:\n");
        kprintf("etext: %08x, edata: %08x, bss: %08x\n", &etext - 1, &edata - 1, &end - 1);
        kprintf("\nPreceding content:\n");
        kprintf("etext: %08x, edata: %08x, bss: %08x\n", *(&etext - 1), *(&edata - 1), *(&end - 1));
	
	kprintf("\nIncoming address:\n");
        kprintf("etext: %08x, edata: %08x, bss: %08x\n", &etext + 1, &edata + 1, &end + 1);
        kprintf("\nIncoming content:\n");
        kprintf("etext: %08x, edata: %08x, bss: %08x\n",*(&etext + 1), *(&edata + 1), *(&end + 1));
}
