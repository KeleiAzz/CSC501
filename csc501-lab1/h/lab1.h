#define LINUXSCHED 1
#define MULTIQSCHED 2

#define NORMALQUEUE	0
#define REALQUEUE	1


void setschedclass(int schedclass);
int getschedclass();
