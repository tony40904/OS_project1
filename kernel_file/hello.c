#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/linkage.h>
#include <uapi/linux/time.h>

/*asmlinkage long sys_hello(int isStart, int pid, struct timespec *start) {
	printk("[project1] pid = %d preparing", pid);

	struct timespec end;
	getnstimeofday(&end);

	printk("[project1] pid = %d running", pid);
	if (isStart == 1)
		getnstimeofday(start);
	else
		printk("[project1] %d %lu.%09lu %lu.%09lu\n", pid, start->tv_sec, start->tv_nsec, end.tv_sec, end.tv_nsec);

	return 0;
}*/

SYSCALL_DEFINE0(hello_0)
{
    printk("[project1] Hello!_0\n");
    return 0;
}

SYSCALL_DEFINE3(hello_3, int, state, int, pid, struct timespec*, start)
{    		
	struct timespec now;
	getnstimeofday(&now);

	if (state)
		copy_to_user(start, &now, sizeof(now));
	else
	{
		struct timespec s;
		copy_from_user(&s, start, sizeof(now));
		printk("[project1] %d %lu.%09lu %lu.%09lu\n", pid, s.tv_sec, s.tv_nsec, now.tv_sec, now.tv_nsec);
	}
	return 0;
}
