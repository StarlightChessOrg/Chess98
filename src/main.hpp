#pragma once
#if !defined(__OPTIMIZE__) && !defined(_DEBUG)
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <errno.h>
#include <sys/resource.h>
#include <unistd.h>
#endif
#endif

inline void set_process_high_priority()
{
#if !defined(__OPTIMIZE__) && !defined(_DEBUG)
#ifdef _WIN32
    HANDLE hProcess = GetCurrentProcess();
    if (!SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS)) {
        if (GetLastError() == ERROR_ACCESS_DENIED) {
            SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
        }
    }
#elif defined(__linux__)
    errno = 0;
    int result = nice(-17);
    if (result == -1 && errno == EPERM) {
        nice(-5);
    }
#elif defined(__APPLE__)
    errno = 0;
    int result = nice(-17);
    if (result == -1 && errno == EPERM) {
        nice(0);
    }
#endif
#endif
}

int main(int argc, char* argv[]);
