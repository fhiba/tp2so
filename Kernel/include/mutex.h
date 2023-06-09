// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef MUTEX_H
#define MUTEX_H

#include <stdatomic.h>

void acquire_mutex(atomic_flag* mutex);

void release_mutex(atomic_flag* mutex);

#endif