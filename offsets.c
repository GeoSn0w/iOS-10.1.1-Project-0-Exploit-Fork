#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/utsname.h>

#include "offsets.h"
//Only change the offsets if you know what you are doing! Check https://youtube.com/c/fce365official for a tutorial on that.
// offsets from the main kernel 0xfeedfacf
uint64_t allproc_offset;
uint64_t kernproc_offset;

// offsets in struct proc
uint64_t struct_proc_p_pid_offset;
uint64_t struct_proc_task_offset;
uint64_t struct_proc_p_uthlist_offset;
uint64_t struct_proc_p_ucred_offset;
uint64_t struct_proc_p_comm_offset;

// offsets in struct kauth_cred
uint64_t struct_kauth_cred_cr_ref_offset;

// offsets in struct uthread
uint64_t struct_uthread_uu_ucred_offset;
uint64_t struct_uthread_uu_list_offset;

// offsets in struct task
uint64_t struct_task_ref_count_offset;
uint64_t struct_task_itk_space_offset;

// offsets in struct ipc_space
uint64_t struct_ipc_space_is_table_offset;

// offsets in struct ipc_port
uint64_t struct_ipc_port_ip_kobject_offset;
//here
void init_iPhone8_1_iOS10_1_1() { //this is
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}

void init_iPhone7_2_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad6_4_S8000() { //This is for iPad, don't change.
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad5_1_S5L8960X() {
    allproc_offset = 0x5a4128;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad5_1_T7001() {
    allproc_offset = 0x5b4228;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone8_1_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad4_3_S5L8960X() {
    allproc_offset = 0x5a4128;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad5_1_T7000() {
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone7_1_T7000() {
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone8_4_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad5_4_T7000() {
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone9_4_T8010() {
    allproc_offset = 0x5ec178;
    kernproc_offset = 0x5f20e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone8_2_T7000() {
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone7_2_T7000() {
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone6_1_S5L8960X() {
    allproc_offset = 0x5a4128;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad6_8_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone9_1_T8010() {
    allproc_offset = 0x5ec178;
    kernproc_offset = 0x5f20e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad5_4_S5L8960X() {
    allproc_offset = 0x5a4128;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone9_3_T8010() {
    allproc_offset = 0x5ec178;
    kernproc_offset = 0x5f20e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad6_3_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPad5_4_T7001() {
    allproc_offset = 0x5b4228;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPod7_1_T7000() {
    allproc_offset = 0x5b4168;
    kernproc_offset = 0x5ba0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone8_2_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}
void init_iPhone7_1_S8000() {
    allproc_offset = 0x5a4148;
    kernproc_offset = 0x5aa0e0;
    
    struct_proc_p_pid_offset = 0x10;
    struct_proc_task_offset = 0x18;
    struct_proc_p_uthlist_offset = 0x98;
    struct_proc_p_ucred_offset = 0x100;
    struct_proc_p_comm_offset = 0x26c;
    
    struct_kauth_cred_cr_ref_offset = 0x10;
    
    struct_uthread_uu_ucred_offset = 0x168;
    struct_uthread_uu_list_offset = 0x170;
    
    struct_task_ref_count_offset = 0x10;
    struct_task_itk_space_offset = 0x300;
    
    struct_ipc_space_is_table_offset = 0x20;
    
    struct_ipc_port_ip_kobject_offset = 0x68;
}

//here end
void unknown_build() {
    printf("This is an unknown kernel build - the offsets are likely to be incorrect and it's very unlikely this exploit will work\n");
    printf("You need to find these two kernel symbols:\n");
    printf("  allproc\n");
    printf("  kernproc\n\n");
    printf("and update the code\n");
}

void init_offsets() {
    struct utsname u = { 0 };
    int err = uname(&u);
    if (err == -1) {
        printf("I can't perform uname. Are you sure this is iOS?\n");
        printf("There's no way this will work, but trying anyway!\n");
        init_iPad4_3_S5L8960X();
        return;
    }
    
    printf("sysname: %s\n", u.sysname);
    printf("nodename: %s\n", u.nodename);
    printf("release: %s\n", u.release);
    printf("version: %s\n", u.version);
    printf("machine: %s\n", u.machine);
    
    if (strstr(u.machine, "iPad5,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
            printf("device iPad5,1 matched\n");
            init_iPad5_1_S5L8960X();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad5,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
            printf("I found an iPad5,1 that matched\n");
            init_iPad5_1_T7001();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone8,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("device iPhone8,1 matched\n");
            init_iPhone8_1_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad4,3")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
            printf("device iPad4,3 matched\n");
            init_iPad4_3_S5L8960X();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad5,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("device iPad5,1 matched\n");
            init_iPad5_1_T7000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone7,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("I found iPhone an iPhone 6 Plus that matched!\n");
            init_iPhone7_1_T7000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone8,4")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("I found iPhone an iPhone SE that matched!\n");
            init_iPhone8_4_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad6,4")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("device iPad6,4 matched\n");
            init_iPad6_4_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone7,2")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("I found iPhone an iPhone 6 that matched!\n");
            init_iPhone7_2_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad5,4")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("device iPad5,4 matched\n");
            init_iPad5_4_T7000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone9,4")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
            printf("I found iPhone an iPhone 7 Plus that matched!\n");
            init_iPhone9_4_T8010();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone8,2")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("I found iPhone an iPhone 6s Plus that matched!\n");
            init_iPhone8_2_T7000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone7,2")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("I found iPhone an iPhone 6 that matched!\n");
            init_iPhone7_2_T7000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone6,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
            printf("device iPhone6,1 matched\n");
            init_iPhone6_1_S5L8960X();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad6,8")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("device iPad6,8 matched\n");
            init_iPad6_8_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone9,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
            printf("device iPhone9,1 matched\n");
            init_iPhone9_1_T8010();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad5,4")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
            printf("device iPad5,4 matched\n");
            init_iPad5_4_S5L8960X();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone9,3")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
            printf("device iPhone9,3 matched\n");
            init_iPhone9_3_T8010();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad6,3")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("device iPad6,3 matched\n");
            init_iPad6_3_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPad5,4")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
            printf("device iPad5,4 matched\n");
            init_iPad5_4_T7001();
            return;
        }
    }
    
    if (strstr(u.machine, "iPod7,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("device iPod7,1 matched\n");
            init_iPod7_1_T7000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone8,2")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("I found iPhone an iPhone 6s PLUS that matched!\n");
            init_iPhone8_2_S8000();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone8,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
            printf("I found iPhone an iPhone 6S that matched!\n");
            init_iPhone8_1_iOS10_1_1();
            return;
        }
    }
    
    if (strstr(u.machine, "iPhone7,1")) {
        if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
            printf("I found iPhone an iPhone 6 Plus that matched!\n");
            init_iPhone7_1_S8000();
            return;
        }
    }
    
    
    
    printf("I have no idea what device are you using\n");
    unknown_build();
    init_iPad4_3_S5L8960X(); //Trying some default bundle, most likely won't work but yeah, worth a try.
}
