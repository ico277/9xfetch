#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "../9xfetch.h"

bool init_uts_info(instance_9x* instance) {
    instance->uts_info = malloc(sizeof(struct utsname));
    if (!instance->uts_info) return false;
    return true;
}

bool get_kernel_version(instance_9x* instance) {
    // utsname struct not initialized so go on to initialize it
    if (!instance->uts_info) {
        // init utsname struct
        if (!init_uts_info(instance)) return false;
        // run uname to fill information in
        int return_value = uname(instance->uts_info);
        // error return value
        if (return_value == -1) {
            // cleanup
            free(instance->uts_info);
            instance->uts_info = NULL;
            return false;
        }
    }
         
    // Kernel name
    size_t length = strlen(instance->uts_info->sysname);
    instance->kernel_name = malloc(length + 1);
    if (!instance->kernel_name) {
        instance->kernel_name = NULL;
        return false;
    }
    strcpy(instance->kernel_name, instance->uts_info->sysname);
    // Kernel version
    length = strlen(instance->uts_info->release);
    instance->kernel_version = malloc(length + 1);
    if (!instance->kernel_version) {
        instance->kernel_version = NULL;
        return false;
    }
    strcpy(instance->kernel_version, instance->uts_info->release);

    return true;
}
bool get_os_name(instance_9x* instance) {
    return true;
}
bool get_user_name(instance_9x* instance) {
    return true;
}
bool get_host_name(instance_9x* instance) {
    return true;
}
bool get_cpu_name(instance_9x* instance) {
    return true;
}
bool get_gpu_name(instance_9x* instance) {
    return true;
}
bool get_memory_usage(instance_9x* instance) {
    return true;
}
bool get_uptime_ms(instance_9x* instance) {
    return true;
}