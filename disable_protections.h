#ifndef _DISABLE_PROTECTIONS_H
#define _DISABLE_PROTECTIONS_H

#include <mach/mach.h>

void disable_protections(uint64_t kernel_base, uint64_t realhost, char* p_comm);
void fix_launchd_after_sandbox_escape(mach_port_t real_service, mach_port_t mitm_port);
mach_port_t get_amfid_task_port();
mach_port_t get_containermanagerd_task_port();
void unsandbox_pid(pid_t target_pid);

#endif
