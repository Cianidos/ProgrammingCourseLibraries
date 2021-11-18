#pragma once
#include <chrono>
#include <iostream>
#include <utility>
#include <functional>

namespace testing
{

    class profiler
    {
        using clock = std::chrono::system_clock;
        std::chrono::time_point<clock> start_;
        std::string message_;
        std::ostream& output_;
        std::optional<std::function<void(long long)>> callback = std::nullopt;

    public:
        profiler() = delete;
        profiler(const profiler&) = delete;
        profiler(profiler&&) = delete;

        profiler& operator=(const profiler&) = delete;
        profiler& operator=(profiler&&) = delete;

        explicit profiler(
            std::string profiling_message = {},
            std::ostream& otp = std::cerr,
			std::optional<std::function<void(long long)>> callback = std::nullopt
        )
            : start_(clock::now()),
            message_(std::move(profiling_message)),
            output_(otp),
            callback(std::move(callback)) {}



        ~profiler()
        {
            auto now = clock::now();
            auto result = std::chrono::duration_cast<std::chrono::microseconds>(now - start_).count();

        	output_ << message_ << " : " << result << " mics";
            if (callback.has_value())
                callback.value()(result);
        }
    };

#define START_PROFILING(name) { testing::profiler _profiler_unique_private_name{name};
#define END_PROFILING }
}
