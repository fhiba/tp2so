#ifndef MUTEX_H
#define MUTEX_H

#include <stdatomic.h>

void acquire_mutex(atomic_flag* mutex);

void release_mutex(atomic_flag* mutex);

#endif