#include <stdio.h>
#include <stddef.h>

#include <windows.h>

#include "9xfetch.h"

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("hiii");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf(" :3\n");
    
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    char buffer[1024];
    size_t size = 1024;
    DWORD version[3];
    
    DWORD returnvalue;
    returnvalue = get_user_name(buffer, size);
    printf("Username: %s\n", buffer);
    returnvalue = get_host_name(buffer, size);
    printf("Hostname: %s\n", buffer);
    returnvalue = get_kernel_version(version);
    printf("Kernel version: %lu.%lu (%lu)\n", version[0], version[1], version[2]);
    returnvalue = get_os_name(buffer, size, version);
    printf("OS: %s\n", buffer);
    returnvalue = get_cpu_name(buffer, size);
    printf("CPU: %s\n", buffer);
    returnvalue = get_gpu_name(buffer, size);
    printf("GPU: %s\n", buffer);
    returnvalue = get_memory_usage(buffer, size);
    printf("Memory: %s\n", buffer);

    system("pause");
    return 0;
}
