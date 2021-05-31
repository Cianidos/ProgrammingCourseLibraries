#pragma once
#include "dynamic_array.h"
#include "sequence.h"

namespace collections
{
    template<typename T>
    using array_sequence = sequence<dynamic_array<T>>;
}
