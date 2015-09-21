#define LINUXSCHED 1
#define MULTIQSCHED 2

#define NORMALQUEUE	1
#define REALQUEUE	2


void setschedclass(int schedclass);
int getschedclass();
