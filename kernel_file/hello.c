#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/linkage.h>
#include <uapi/linux/time.h>

SYSCALL_DEFINE3(set_time, int, isStart, int, pid, struct timespec*, timestamp)
{    		
	struct timespec now;
	getnstimeofday(&now);

	if (isStart)
		copy_to_user(timestamp, &now, sizeof(now));
	else
	{
		struct timespec startTime;
		copy_from_user(&startTime, timestamp, sizeof(now));
		printk("[project1] %d %lu.%09lu %lu.%09lu\n", pid, startTime.tv_sec, startTime.tv_nsec, now.tv_sec, now.tv_nsec);
	}
	return 0;
}
