#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <windows.h>
#include <intrin.h>

// OS info

bool get_kernel_version(DWORD* version) {
    OSVERSIONINFOA os_version_info;
    ZeroMemory(&os_version_info, sizeof(OSVERSIONINFOA));
    // has to be set before calling GetVersionExA
    os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

    if (!GetVersionExA(&os_version_info)) {
        return false;
    }

    // dangerous assumption of size, should be handled better
    version[0] = os_version_info.dwMajorVersion;
    version[1] = os_version_info.dwMinorVersion;
    version[2] = os_version_info.dwPlatformId;

    return true;
}

size_t get_os_name(char* buffer, size_t buffer_size, DWORD* version) {
    /* combine both versions into a singular DWORD
       windows major and minor versions are never too big for it to be a problem
       that they're squished into 16 bits */
    DWORD combined = (version[0] << 16) | version[1];
    char* str = NULL;
    // Windows 9X systems
    if (version[2] != VER_PLATFORM_WIN32_NT) {
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
    // Windows NT systems (this may not report correctly on anything above windows 8)
    // TODO use GetProcAddress to fix it for modern systems
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
                str = "Windows 10/11";
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
    DISPLAY_DEVICEA display_device;
    ZeroMemory(&display_device, sizeof(display_device));
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

// Returns garbage on 64bit systems with more than 4GB RAM
// TODO use GetProcAddress to fix it for modern systems
bool get_memory_usage(char* buffer, size_t buffer_size) {
    MEMORYSTATUS memory;
    memory.dwLength = sizeof(memory);

    GlobalMemoryStatus(&memory);

    snprintf(buffer, buffer_size, "%lu MB/%lu MB", (memory.dwTotalPhys - memory.dwAvailPhys) / (1024 * 1024),  memory.dwTotalPhys / (1024 * 1024));

    return true;
}
