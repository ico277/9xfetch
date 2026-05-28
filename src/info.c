#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <windows.h>
#include <intrin.h>
#include <winternl.h>

#include "9xfetch.h"

// OS info

typedef NTSTATUS (WINAPI *RtlGetVersion_t)(LPOSVERSIONINFOEXW);

bool get_kernel_version(os_version_t* version) {
    // Use RtlGetVersion if on a newer NT system
    HMODULE hmodule = GetModuleHandleW(L"ntdll.dll");
    if (hmodule) {
        RtlGetVersion_t RtlGetVersion_func = (RtlGetVersion_t)GetProcAddress(hmodule, "RtlGetVersion");
        if(!RtlGetVersion_func) {
            FreeLibrary(hmodule);
            return false;
        }
        OSVERSIONINFOEXW os_version_info_ex = {0};
        os_version_info_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
        if (RtlGetVersion_func(&os_version_info_ex) == 0) {
            version->major_version = os_version_info_ex.dwMajorVersion;
            version->minor_version = os_version_info_ex.dwMinorVersion;
            version->build_number = os_version_info_ex.dwBuildNumber;
            version->platform_id = os_version_info_ex.dwPlatformId;
            FreeLibrary(hmodule);
            return true;
        }
        FreeLibrary(hmodule);
        return false;
    }
    // otherwise default to GetVersionExA
    else {
        OSVERSIONINFOA os_version_info_a = {0};
        os_version_info_a.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        if(GetVersionExA(&os_version_info_a)) {
            // dangerous assumption of size, should be handled better
            version->major_version = os_version_info_a.dwMajorVersion;
            version->minor_version = os_version_info_a.dwMinorVersion;
            version->build_number = os_version_info_a.dwBuildNumber;
            version->platform_id = os_version_info_a.dwPlatformId;
            return true;
        }
        return false;
    }
}

size_t get_os_name(char* buffer, size_t buffer_size, os_version_t* version) {
    /* combine both versions into a singular DWORD
       windows major and minor versions are never too big for it to be a problem
       that they're squished into 16 bits */
    DWORD combined = (version->major_version << 16) | version->minor_version;
    char* str = NULL;
    // Windows 9X systems
    if (version->platform_id != VER_PLATFORM_WIN32_NT) {
        switch (combined) {
            case (4 << 16) | 0:
                str = "Windows 95";
                break;
            case (4 << 16) | 10:
                str = "Windows 98";
                break;
            case (4 << 16) | 90:
                str = "Windows ME";
                break;
            default:
                str = "Unknown 9X";
                break;
        }
    } 
    // Windows NT systems
    else {
        switch (combined) {
            case (3 << 16) | 1:
                str = "Windows NT 3.1";
                break;
            case (3 << 16) | 5:
                str = "Windows NT 3.5";
                break;
            case (3 << 16) | 51:
                str = "Windows NT 3.51";
                break;
            case (4 << 16) | 0:
                str = "Windows NT 4.0";
                break;
            case (5 << 16) | 0:
                str = "Windows 2000";
                break;
            case (5 << 16) | 1:
            case (5 << 16) | 2:
                str = "Windows XP";
                break;
            case (6 << 16) | 0:
                str = "Windows Vista";
                break;
            case (6 << 16) | 1:
                str = "Windows 7";
                break;
            case (6 << 16) | 2:
                str = "Windows 8";
                break;
            case (6 << 16) | 3:
                str = "Windows 8.1";
                break;
            case (10 << 16) | 0:
                if (version->build_number >= 20000)
                    str = "Windows 11";
                else
                    str = "Windows 10";
                break;
            default:
                str = "Unknown NT";
                break;
        }
    }

    strncpy(buffer, str, buffer_size);
    return strnlen(buffer, buffer_size);
}

size_t get_user_name(char* buffer, size_t buffer_size) {
    DWORD chars_read = buffer_size;

    if (!GetUserNameA(buffer, &chars_read)) {
        return 0;
    }

    return chars_read;
}

size_t get_host_name(char* buffer, size_t buffer_size) {
    DWORD chars_read = buffer_size;

    if (!GetComputerNameA(buffer, &chars_read)) {
        return 0;
    }

    return chars_read;
}

// TODO find a way to make it not break after like 50ish days
DWORD get_uptime_ms() {
    return GetTickCount();
}

// HW info

bool get_cpu_name(char* buffer, size_t buffer_size) {
    int cpuid_value[4] = {0, 0, 0, 0};
    char vendor[13];
    char cpu_name[49];

    __cpuid(cpuid_value, 0); // 0 = vendor
    memcpy(vendor + 0, &cpuid_value[1], 4); // EBX
    memcpy(vendor + 4, &cpuid_value[3], 4); // EDX
    memcpy(vendor + 8, &cpuid_value[2], 4); // ECX
    vendor[12] = '\0';

    //TODO add manual Pentium CPU detection
    __cpuid(cpuid_value, 0x80000000);
    if (cpuid_value[0] < 0x80000004) {
        strncpy(buffer, "Unknown CPU", buffer_size);
        return false;
    }
    
    __cpuid(cpuid_value, 0x80000002); // first 16 bytes
    memcpy(cpu_name, cpuid_value, sizeof(cpuid_value));
    __cpuid(cpuid_value, 0x80000003); // seccond 16 bytes
    memcpy(cpu_name + 16, cpuid_value, sizeof(cpuid_value));
    __cpuid(cpuid_value, 0x80000004); // third 16 bytes
    memcpy(cpu_name + 32, cpuid_value, sizeof(cpuid_value));
    cpu_name[48] = '\0';

    snprintf(buffer, buffer_size, "%s %s", vendor, cpu_name);

    return true;
}

bool get_gpu_name(char* buffer, size_t buffer_size) {
    DISPLAY_DEVICEA display_device = {0};
    display_device.cb = sizeof(display_device);

    DWORD i = 0;
    // loop through devices
    while (EnumDisplayDevicesA(NULL, i, &display_device, 0)) {
        // check if primary display device
        if (display_device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {
            strncpy(buffer, display_device.DeviceString, buffer_size);
            return true;
        }
        i++;
    }

    return 0;
}


typedef BOOL (WINAPI *GlobalMemoryStatusEx_t)(LPMEMORYSTATUSEX);

void get_memory_usage(memory_info_t* meminfo) {
    HMODULE hmodule = GetModuleHandleW(L"kernel32.dll");
    if (hmodule) {
        // check if the newer GlobalMemoryStatusEx function is available
        GlobalMemoryStatusEx_t GlobalMemoryStatusEx_func = (GlobalMemoryStatusEx_t)GetProcAddress(hmodule, "GlobalMemoryStatusEx");
        if (GlobalMemoryStatusEx_func) {
            MEMORYSTATUSEX memory_status = {0};
            memory_status.dwLength = sizeof(MEMORYSTATUSEX);
            if (GlobalMemoryStatusEx(&memory_status)) {
                meminfo->mem_total_kib = memory_status.ullTotalPhys / 1024;
                meminfo->mem_used_kib = (memory_status.ullTotalPhys - memory_status.ullAvailPhys) / 1024;
                goto exit;
            }
        }
    }

    // otherwise use the old GlobalMemoryStatus function
    MEMORYSTATUS memory_status;
    memory_status.dwLength = sizeof(MEMORYSTATUS);

    GlobalMemoryStatus(&memory_status);
    
    meminfo->mem_total_kib = memory_status.dwTotalPhys / 1024;
    meminfo->mem_used_kib = (memory_status.dwTotalPhys - memory_status.dwAvailPhys) / 1024;
exit:
    if (hmodule) FreeModule(hmodule);
    return;
}
