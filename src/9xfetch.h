#include <windows.h>

WINBOOL get_kernel_version(DWORD* version);
DWORD get_os_name(char* buffer, DWORD buffer_size, DWORD* version);
DWORD get_user_name(char* buffer, DWORD buffer_size);
DWORD get_host_name(char* buffer, DWORD buffer_size);
WINBOOL get_cpu_name(char* buffer, DWORD buffer_size);
DWORD get_gpu_name(char* buffer, DWORD buffer_size);
WINBOOL get_memory_usage(char* buffer, DWORD buffer_size);