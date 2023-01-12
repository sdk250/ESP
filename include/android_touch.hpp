#pragma once

#include <sys/ioctl.h>
#include <linux/input.h>
#include <dirent.h>
#include <unistd.h>

#include <imgui.h>
#include <global_value>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) (( ((x) - 1) / BITS_PER_LONG) + 1)
#define CBIT(ARRAY, BIT) (\
	(\
		ARRAY[BIT / BITS_PER_LONG] >> BIT % BITS_PER_LONG\
	) & 1\
)
#define NAME_SIZE 64

void Android_Touch(ImGuiIO&);
int getTouchScreenDevice(char*);
int isa_event_device(const struct dirent*);
