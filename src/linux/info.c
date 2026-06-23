#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "../9xfetch.h"

instance_9x* init_instance_9x(instance_9x *instance) {
    memset(instance, 0, sizeof(instance_9x));
    return instance;
}

bool fill_uts_info(instance_9x* instance) {
    if (!instance->uts_info) {
        // init utsname struct
        instance->uts_info = malloc(sizeof(struct utsname));
        if (!instance->uts_info) return false;
        // run uname to fill information in
        int return_value = uname(instance->uts_info);
        // error return value
        if (return_value == -1) {
            // cleanup
            free(instance->uts_info);
            instance->uts_info = NULL;
            return false;
        }
        return true;
    }

    // already exists
    return true;
}

bool fill_sys_info(instance_9x* instance) {
    if (!instance->sys_info) {
        // init sysinfo struct
        instance->sys_info = malloc(sizeof(struct sysinfo));
        if (!instance->sys_info) return false;
        // run uname to fill information in
        int return_value = sysinfo(instance->sys_info);
        // error return value
        if (return_value == -1) {
            // cleanup
            free(instance->sys_info);
            instance->sys_info = NULL;
            return false;
        }
        return true;
    }

    // already exists
    return true;
}

bool fill_passwd_info(instance_9x* instance) {
    if (!instance->passwd) {
        uid_t uid = getuid();
        instance->passwd = getpwuid(uid);
        if(!instance->passwd) return false;
    }

    return true;
}

bool get_kernel_version(instance_9x* instance) {
    if (instance->kernel_version && instance->kernel_name) return true;
    // be sure utsname struct is populated
    if (!fill_uts_info(instance)) return false;
         
    // kernel name
    size_t length = strlen(instance->uts_info->sysname);
    instance->kernel_name = malloc(length + 1);
    if (!instance->kernel_name) {
        instance->kernel_name = NULL;
        return false;
    }
    strcpy(instance->kernel_name, instance->uts_info->sysname);
    // kernel version
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
    // TODO distro detection
    // FILE* meminfo_f = fopen("/etc/os-release", "r");
    // if (!meminfo_f) return false;

    // while (fgets(instance->line_buf, 1024, meminfo_f)) {
    //     if (strncmp(instance->line_buf, "NAME=", 5) == 0) {
            
    //     } 
    // }
    return true;
}
bool get_user_name(instance_9x* instance) {
    if (instance->user_name) return true;
    if (!fill_passwd_info(instance)) return false;
    
    size_t length = strlen(instance->passwd->pw_name);
    instance->user_name = malloc(length + 1);
    if (!instance->user_name) {
        instance->user_name = NULL;
        return false;
    }
    strcpy(instance->user_name, instance->passwd->pw_name);

    return true;
}
bool get_host_name(instance_9x* instance) {
    if (instance->host_name) return true;
    if (!fill_uts_info(instance)) return false;

    size_t length = strlen(instance->uts_info->nodename);
    instance->host_name = malloc(length + 1);
    if (!instance->host_name) {
        instance->host_name = NULL;
        return false;
    }
    strcpy(instance->host_name, instance->uts_info->nodename);

    return true;
}
bool get_cpu_name(instance_9x* instance) {
    return true;
}
bool get_gpu_name(instance_9x* instance) {
    return true;
}
bool get_memory_usage(instance_9x* instance) {
    if (instance->memory_free_kib && instance->memory_total_kib && instance->memory_used_kib)
        return true;
#ifdef _9XFETCH_FEATURE_LINUX_SYSINFO
    if (!fill_sys_info(instance)) return false;

    printf("%lu totalram MiB\n%lu freeram MiB\n%lu bufferram MiB\n", instance->sys_info->totalram / 1024, instance->sys_info->freeram / 1024, instance->sys_info->bufferram / 1024);
    instance->memory_free_kib = instance->sys_info->freeram / 1024;
    instance->memory_total_kib = instance->sys_info->totalram / 1024;
    instance->memory_used_kib = (instance->sys_info->totalram - instance->sys_info->freeram) / 1024;
#else
    FILE* meminfo_f = fopen("/proc/meminfo", "r");
    if (!meminfo_f) return false;

    while (fgets(instance->line_buf, 1024, meminfo_f)) {
        if (strncmp(instance->line_buf, "MemTotal:", 9) == 0 && instance->memory_total_kib == 0) {
            instance->memory_total_kib = strtoul(instance->line_buf + 10, NULL, 10);
        } else if (strncmp(instance->line_buf, "MemAvailable:", 13) == 0 && instance->memory_free_kib == 0) {
            instance->memory_free_kib = strtoul(instance->line_buf + 13, NULL, 10);
        }
    }

    fclose(meminfo_f);

    if (instance->memory_total_kib != 0 && instance->memory_free_kib != 0) {
        instance->memory_used_kib = instance->memory_total_kib - instance->memory_free_kib;
        return true;
    }
#endif
    return false;
}
bool get_uptime_ms(instance_9x* instance) {
    if (instance->uptime_ms) return true;
    if (!fill_sys_info(instance)) return false;

    instance->uptime_ms = instance->sys_info->uptime * 1000;

    return true;
}