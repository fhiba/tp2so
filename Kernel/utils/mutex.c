#include <mutex.h>
 
void acquire_mutex(atomic_flag* mutex)
{
	while(atomic_flag_test_and_set(mutex))
	{
		__builtin_ia32_pause();
	}
}

void release_mutex(atomic_flag* mutex)
{
	atomic_flag_clear(mutex);
}