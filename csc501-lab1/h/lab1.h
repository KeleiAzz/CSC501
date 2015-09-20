#define LINUXSCHED 1
#define MULTIQSCHED 2


void setschedclass(int schedclass);
int getschedclass();

// void setschedclass(int schedclass)
// {
// 	sched_class = schedclass;
// }
// int getschedclass()
// {
// 	return sched_class;
// }