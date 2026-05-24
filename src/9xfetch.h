#include <stdbool.h>

#include <windows.h>

bool get_kernel_version(DWORD* version);
size_t get_os_name(char* buffer, size_t buffer_size, DWORD* version);
size_t get_user_name(char* buffer, size_t buffer_size);
size_t get_host_name(char* buffer, size_t buffer_size);
bool get_cpu_name(char* buffer, size_t buffer_size);
size_t get_gpu_name(char* buffer, size_t buffer_size);
bool get_memory_usage(char* buffer, size_t buffer_size);