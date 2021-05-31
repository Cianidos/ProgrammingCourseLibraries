// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <vector>
#include <concepts>
#include <ranges>
#include <algorithm>

#include "utils.h"
#include "linked_list.h"
#include "testing.h"
#include "advanced_io.h"
#include "not_vector.h"
    
using namespace adio;
using namespace std;

template <has_output_operator T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
{
    for (const T& ii : v)
    {
        os << ii << ' ';
    }
    return os;
}


template <
has_output_operator Ty1,
has_output_operator Ty2
>
std::ostream& operator << (std::ostream& os, const std::pair<Ty1, Ty2>& p)
{
    os << "{ " << p.first << ", " << p.second << " }";
    return os;
}

using namespace collections;

template <has_input_operator T>
std::istream& operator >> (std::istream& os, const sequence<linked_list<T>>& v)
{
    T tmp;
    while (os >> tmp)
    { v.append_back(std::move(tmp)); }

    return os;
}

template <has_output_operator T>
std::ostream& operator << (std::ostream& os, const sequence<linked_list<T>>& v)
{
    for (const T& ii : v)
    {
        os << ii << ' ';
    }
    return os;
}

int main()
{
    script_builder builder;
    sequence<linked_list<int>> lli;
    sequence<linked_list<float>> llf;

    script_builder builder2;
    auto fill_sequence_int = builder2
    .read_data<sequence<linked_list<int>>>(lli)
    .build();

    script_builder builder3;
    auto fill_sequence_float = builder3
    .read_data<sequence<linked_list<float>>>(llf)
    .build();

    script_builder builder4;
    auto create_sequence = builder4
    .add_choose_q_enumerated("Choose type", 
    {{"int", "float"}}, 
    {[&]() { fill_sequence_int->start(); },
            [&]() { fill_sequence_float->start(); }
    })
    .build();

    auto script = builder
        .add_choose_q_enumerated(
            "Choose action", 
            {{
                "Create sequence",
                "Delete sequence",
                "Print sequence"}}, 
        {[&](){ create_sequence->start(); },
                [&](){},
                [&](){cout << lli; }})
    .build();

    while(true)
        script->start();

    return 0;
}
