#include "pch.h"

#include "ThreadTools.h"

#include <thread>
#include <chrono>


namespace Ftool
{
    //static 
    void ThreadTools::Sleep(int intervalMs)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }

    //static
    int ThreadTools::GetHwConcurrency()
    {
        return std::thread::hardware_concurrency();
    }

} // namespace Ftool

