#include "test.hpp"

static inline void setRealtimePriority()
{
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif
}

int main()
{
    setRealtimePriority();
    testByUCCI();
    return 0;
}
