#pragma once
#include <functional>
#include <map>
#include <set>
#include <string>
#include <optional>
#include <variant>

struct arguments_processor
{
    using key = std::set<std::string>;

    template <typename KeyType>
    using key_processor = std::function<void(std::optional<KeyType>)>;

    template <typename KeyType>
    using map_case = std::pair<key, key_processor<KeyType>>;

    using possible_processors = std::variant<
        key_processor<int>, key_processor<std::string>, key_processor<void>>;

    std::map<std::string, possible_processors> commands;
    
    template <typename KeyType>
    arguments_processor& add_case(map_case<KeyType> _case)
    {
        for (auto& i : _case.first)
        {
            commands[i] = _case.second;
        }
        return *this;
    }
};


template <typename ArgType>
struct arg
{
    std::set<std::string> names;
    void add_name(std::string name);
};

struct command
{
    template<typename ArgType>
    void add_arg(arg<ArgType> a) { }
};

#include <algorithm>
#include <memory>
#include <vector>
#include <iterator>


