#ifndef COMMONS_H
#define COMMONS_H

#include <cstdint>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <fstream>

typedef unsigned long long int timestamp_t;

timestamp_t get_timestamp() {
	struct timeval now;
	gettimeofday (&now, NULL);

	return  now.tv_usec + (time_t)now.tv_sec * 1000000;
}

static void* memory_map(std::string filename) {
    struct stat st;
    stat(filename.c_str(), &st);

    int fd = open(filename.c_str(), O_RDONLY, 0);
    assert(fd != -1);

    // Try mapping with huge-pages support
    void *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    assert(data != (void * )-1);

    return data;
  }

#endif