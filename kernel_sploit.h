#ifndef _KERNEL_SPLOIT_H
#define _KERNEL_SPLOIT_H

#include <mach/mach.h>

mach_port_t get_kernel_task_port(mach_port_t host_priv, uint64_t* kernel_base, uint64_t* realhost);

#endif
