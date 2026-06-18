#include <stdbool.h>

#include <stddef.h>

#if defined(_9XFETCH_BACKEND_WIN32)
#include <windows.h>
#elif defined(_9XFETCH_BACKEND_LINUX)
#include <sys/utsname.h>
#endif

typedef struct {
    // os info
    char* kernel_name;
    char* kernel_version;
    char* os_pretty_name;
    char* user_name;
    char* host_name;
    size_t uptime_ms;
    // hardware info
    size_t memory_used_kib;
    size_t memory_total_kib;
    size_t memory_free_kib;
    char* cpu_vendor;
    char* cpu_brand;
    char** gpu_names;
    size_t gpu_count;
    // os specific (DO NOT USE)
#if defined(_9XFETCH_BACKEND_WIN32)
    DWORD win32_major_version;
    DWORD win32_minor_version;
    DWORD win32_build_number;
    DWORD win32_platform_id;
#elif defined(_9XFETCH_BACKEND_LINUX)
    struct utsname* uts_info;
#endif
} instance_9x;


bool get_kernel_version(instance_9x* instance);
bool get_os_name(instance_9x* instance);
bool get_user_name(instance_9x* instance);
bool get_host_name(instance_9x* instance);
bool get_cpu_name(instance_9x* instance);
bool get_gpu_name(instance_9x* instance);
bool get_memory_usage(instance_9x* instance);
bool get_uptime_ms(instance_9x* instance);