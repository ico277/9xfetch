#include <stdbool.h>

#include <stddef.h>
#include <windows.h>

typedef struct {
    DWORD major_version;
    DWORD minor_version;
    DWORD build_number;
    DWORD platform_id;
} os_version_t;

typedef struct {
    size_t mem_used_kib;
    size_t mem_total_kib;
} memory_info_t;

bool get_kernel_version(os_version_t* version);
size_t get_os_name(char* buffer, size_t buffer_size, os_version_t* version);
size_t get_user_name(char* buffer, size_t buffer_size);
size_t get_host_name(char* buffer, size_t buffer_size);
bool get_cpu_name(char* buffer, size_t buffer_size);
bool get_gpu_name(char* buffer, size_t buffer_size);
void get_memory_usage(memory_info_t* meminfo);
DWORD get_uptime_ms();