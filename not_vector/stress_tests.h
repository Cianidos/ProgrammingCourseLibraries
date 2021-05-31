#pragma once

#include <functional>

namespace testing::stress
{
    
    inline auto code_iteration(std::function<void(void)> code, size_t iterations = 10'000)
    {
        return [=]()
        {
            for (size_t i = 0; i < iterations; ++i)
            {
                code();
            }
        };
    }
}
