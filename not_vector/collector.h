#pragma once
#include <functional>
#include <string>
#include <unordered_map>


#include "testing.h"


namespace testing::collector
{
    using test_name = std::string;
    using test_group = std::string;


    namespace impl
    {
    template<typename K, typename V>
    using dictionary = std::unordered_map<K, V>;
#ifndef NDEBUG
        inline
        dictionary<test_group, dictionary<test_name, test_type>>
        tests_collector;
#endif
    }

}

#define TEST_BEGIN(GROUP, NAME)             \
void test_##GROUP##_##NAME ();              \
void test_##GROUP##_##NAME () {             \
::testing::collector::impl::tests_collector \
[#GROUP][#NAME] = test_##GROUP##_##NAME;    \
