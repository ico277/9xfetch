#include <stdio.h>
#include <stddef.h>

#include "9xfetch.h"

int main() {
#ifdef _9XFETCH_BACKEND_WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("hiii");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf(" :3\n");
    
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
    instance_9x instance = {0};
    
    bool returnvalue;
    returnvalue = get_user_name(&instance);
    printf("Username: %s\n", instance.user_name);
    returnvalue = get_host_name(&instance);
    printf("Hostname: %s\n", instance.host_name);
    get_kernel_version(&instance);
    printf("Kernel version: %s %s\n", instance.kernel_name, instance.kernel_version);
    returnvalue = get_os_name(&instance);
    printf("OS: %s\n", instance.os_pretty_name);
    returnvalue = get_uptime_ms(&instance);
    printf("Uptime: %lus\n", instance.uptime_ms / 1000);
    get_cpu_name(&instance);
    printf("CPU: %s %s\n", instance.cpu_vendor, instance.cpu_brand);
    //returnvalue = get_gpu_name(&instance);
    //printf("GPU: %s\n", instance.gpu_names[0]);
    get_memory_usage(&instance);
    printf("Memory: %luMiB/%luMiB\n", instance.memory_used_kib / 1024, instance.memory_total_kib / 1024);

    //system("pause");
    return 0;
}
