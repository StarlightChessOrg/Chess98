#include "test.hpp"

static inline void setRealtimePriority()
{
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif
}

std::atomic<bool> ucciValidate{false};

void validateUCCI()
{
    std::string tmp = "";
    std::getline(std::cin, tmp);
    if (tmp == "ucci")
    {
        ucciValidate = true;
    }
}

int main()
{
    setRealtimePriority();
    std::thread v(validateUCCI);
    v.detach();
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "input ucci to enable ucci mode (within 1 second)" << std::endl;
    while (true)
    {
        auto end = std::chrono::high_resolution_clock::now();
        int duration = int(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        if (ucciValidate == true)
        {
            break;
        }
        if (duration > 1000)
        {
            break;
        }
    }
    if (ucciValidate)
    {
        std::cout << "ucciok" << std::endl;
        testByUCCI();
    }
    else
    {
        testByUI();
    }
    return 0;
}
