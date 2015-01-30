#ifndef COMMONS_H
#define COMMONS_H

#include <cstdint>
#include <sys/time.h>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <cassert>

typedef unsigned long long int timestamp_t;

timestamp_t get_timestamp() {
	struct timeval now;
	gettimeofday (&now, NULL);

	return  now.tv_usec + (time_t)now.tv_sec * 1000000;
}

#endif