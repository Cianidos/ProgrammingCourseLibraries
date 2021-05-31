#pragma once
#include <chrono>
#include <iostream>
#include <utility>

namespace testing
{

    class profiler
    {
        using clock = std::chrono::system_clock;
        std::chrono::time_point<clock> start_;
        std::string message_;
        std::ostream& output_;

    public:
        profiler() = delete;
        profiler(const profiler&) = delete;
        profiler(profiler&&) = delete;

        profiler& operator=(const profiler&) = delete;
        profiler& operator=(profiler&&) = delete;

        explicit profiler(
            std::string profiling_message = {},
            std::ostream& otp = std::cerr
        )
            : start_(clock::now()),
            message_(std::move(profiling_message)),
            output_(otp) {}

        ~profiler()
        {
            output_ << message_ << " : " << std::chrono::duration_cast<
                std::chrono::milliseconds>(clock::now() - start_).count()
                << " ms";
        }
    };

#define START_PROFILING(name) { testing::profiler _profiler_unique_private_name{name};
#define END_PROFILING }
}
