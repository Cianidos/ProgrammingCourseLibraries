#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ranges>


class term
{
};

term* parser(std::string text)
{
    //go throw text
    //build term tree
}

struct token
{
    std::string value;
    token& operator+=(char c)
    {
        value += c;
        return *this;
    }
};

namespace sr = ::std::ranges;
namespace sv = ::std::views;


std::vector<token> token_splitter(std::string text)
{
    std::vector<token> result;
    sr::for_each(text, [&](char c)
    {
        
    });
}




