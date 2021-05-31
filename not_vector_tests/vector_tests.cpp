#include "pch.h"
#include "not_vector.h"
#include <algorithm>

using namespace std;

TEST(vector, math_operations)
{
    math_objects::vector<int, 10> a;
    math_objects::vector<int, 10> b;

    iota(begin(a), end(a), 1);
    iota(rbegin(a), rend(a), 10);


}