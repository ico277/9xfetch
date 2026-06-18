#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <windows.h>
#include <intrin.h>
#include <winternl.h>

#include "../9xfetch.h"

// OS info

typedef NTSTATUS (WINAPI *RtlGetVersion_t)(LPOSVERSIONINFOEXW);

void get_kernel_version(os_version_t* version) {
    // Use RtlGetVersion if on a newer NT system
    HMODULE hmodule = GetModuleHandleW(L"ntdll.dll");
    if (hmodule) {
        RtlGetVersion_t RtlGetVersion_func = (RtlGetVersion_t)GetProcAddress(hmodule, "RtlGetVersion");
        if(RtlGetVersion_func) {
            OSVERSIONINFOEXW os_version_info_ex = {0};
            os_version_info_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
            if (RtlGetVersion_func(&os_version_info_ex) == 0) {
                version->major_version = os_version_info_ex.dwMajorVersion;
                version->minor_version = os_version_info_ex.dwMinorVersion;
                version->build_number = os_version_info_ex.dwBuildNumber;
                version->platform_id = os_version_info_ex.dwPlatformId;
                goto exit;
            }
        }
    }

    // otherwise default to GetVersionExA
    OSVERSIONINFOA os_version_info_a = {0};
    os_version_info_a.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    if(GetVersionExA(&os_version_info_a)) {
        version->major_version = os_version_info_a.dwMajorVersion;
        version->minor_version = os_version_info_a.dwMinorVersion;
        version->build_number = os_version_info_a.dwBuildNumber;
        version->platform_id = os_version_info_a.dwPlatformId;
    }

exit:
    if (hmodule) FreeModule(hmodule);
    return;
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
size_t get_uptime_ms() {
    return (size_t) GetTickCount();
}

// HW info

void get_cpu_name(char* buffer, size_t buffer_size) {
    int cpuid_value[4] = {0, 0, 0, 0};
    char vendor[13] = {0};
    char cpu_name[49] = {0};

    __cpuid(cpuid_value, 0); // 0 = vendor
    memcpy(vendor + 0, &cpuid_value[1], 4); // EBX
    memcpy(vendor + 4, &cpuid_value[3], 4); // EDX
    memcpy(vendor + 8, &cpuid_value[2], 4); // ECX
    vendor[12] = '\0';

    // check if CPU supports brand string
    __cpuid(cpuid_value, 0x80000000);
    if (cpuid_value[0] < 0x80000004) {
        // manual detection
        //strncpy(buffer, "Unknown CPU", buffer_size);
        __cpuid(cpuid_value, 0x1);
        size_t family = (cpuid_value[0] >> 8) & 0x0F;
        size_t model = (cpuid_value[0] >> 4) & 0x0F;

        size_t extended_family = (cpuid_value[0] >> 20) & 0xFF;
        size_t extended_model = (cpuid_value[0] >> 16) & 0x0F;

        size_t mmx = (cpuid_value[3] >> 23) & 1;
        size_t sse = (cpuid_value[3] >> 25) & 1;

        if (family == 0x0F) {
            family += extended_family;
            model += (extended_model << 4);
        }

        // most of this is untested!!!!
        if (strncmp(vendor, "GenuineIntel", 11) == 0) {
            // OG pentium
            if (family == 5) {
                if (mmx) {
                    strncpy(cpu_name, "Pentium MMX", 49);
                } else {
                    strncpy(cpu_name, "Pentium", 49);
                }
            }
            // pentium 2/3
            else if (family == 6) {
                if (sse) {
                    strncpy(cpu_name, "Pentium 3", 49);
                } else {
                    strncpy(cpu_name, "Pentium 2", 49);
                }
            }
            // pretty vendor name
            strncpy(vendor, "Intel", 12);
        } else if ((strncmp(vendor, "AuthenticAMD", 11) == 0)) {
            strncpy(cpu_name, "Unknown AMD", 49);
            // pretty vendor name
            strncpy(vendor, "AMD", 12);
        } else {
            snprintf(cpu_name, 48, "Unknown Fam %lu mod %lu extfam %lu extmod %lu feat %lu", family, model, extended_family, extended_model, cpuid_value[3]);
        }
    } else {
        // brand string
        __cpuid(cpuid_value, 0x80000002); // first 16 bytes
        memcpy(cpu_name, cpuid_value, sizeof(cpuid_value));
        __cpuid(cpuid_value, 0x80000003); // seccond 16 bytes
        memcpy(cpu_name + 16, cpuid_value, sizeof(cpuid_value));
        __cpuid(cpuid_value, 0x80000004); // third 16 bytes
        memcpy(cpu_name + 32, cpuid_value, sizeof(cpuid_value));
        cpu_name[48] = '\0';
    }
    
    snprintf(buffer, buffer_size, "%s %s", vendor, cpu_name);
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
            if (GlobalMemoryStatusEx_func(&memory_status)) {
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
