#pragma once
#include <concepts>
#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <sstream>

#include "utils.h"
#include "profiler.h"

namespace testing
{

    template<
        has_output_operator Type1,
        has_output_operator Type2
    >
        void assert_equal
        (
            const Type1& lhs, const Type2& rhs,
            const std::string& hint = {}
        )
    {
        if (!(lhs == rhs))
        {
            std::ostringstream message;
            message << "Assertion error: "
                << lhs << " != " << rhs
                << std::endl << hint << std::endl;
            throw std::runtime_error(message.str());
        }
    }

    inline void assert(
        const bool expression,
        const std::string& hint = {}
    )
    {
        assert_equal(expression, true, hint);
    }

#define ASSERT_EQUAL(lhs, rhs)                          \
{                                                       \
    testing::assert_equal(lhs, rhs,                     \
    std::string(__FILE__) + std::string(": ")           \
    + std::to_string(__LINE__) + std::string(" ")       \
    + std::string(#lhs)                                 \
    + std::string(" != ") + std::string(#rhs));         \
}                                                       

#define ASSERT(expression)                               \
{                                                        \
    testing::assert(expression,                          \
    std::string(__FILE__) + std::string(": ")            \
    + std::to_string(__LINE__) + std::string(" ")        \
    + std::string(#expression) + std::string(" failed"));\
}                                                       

    class test_runner
    {
        size_t tests_ = 0;
        size_t fails_ = 0;
        size_t profiles_ = 0;
        std::ostream& output_;

    public:

        explicit test_runner(std::ostream& output = std::cerr)
            : output_(output) {}

        template<typename TestFunction>
        test_runner& run_test(TestFunction func, const std::string& test_name)
        {
            tests_ += 1;
            try
            {
                func();
                output_ << test_name << " OK" << std::endl;
            }
            catch (const std::exception& e)
            {
                output_ << test_name << " fail: " << e.what() << std::endl;
                fails_ += 1;
            }
            catch (...)
            {
                output_ << "Unknown exception" << std::endl;
                fails_ += 1;
            }
            return *this;
        }

        template<typename TestFunction>
        test_runner& profiled_test(TestFunction func, const std::string& test_name)
        {
            output_ << "Profiling result: ";
            {
                profiler p(test_name, output_);
                func();
            }
            output_ << std::endl;
            profiles_ += 1;
            return *this;
        }

        void tests_stat() const
        {
            output_ << "Tests runs: " << tests_ << std::endl
                << "Tests failed: " << fails_ << std::endl
                << "Profiles complete: " << profiles_ << std::endl;
        }
    };

#define RUN_TEST(TEST_FUNC) .run_test(TEST_FUNC, #TEST_FUNC)
#define RUN_STRESS(TEST_FUNC) .profiled_test(TEST_FUNC, #TEST_FUNC)
}

