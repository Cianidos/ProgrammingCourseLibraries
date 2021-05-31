#include <iostream>
#include <Nodes.hpp>

template<typename T>
struct functor
{
    size_t operator()(const T& a, const T& b)
    {
        return a < b;
    }
};

template<typename T, size_t  N>
std::ostream& operator << (std::ostream& os, Node<T,N>* node)
{
    NodeIterator it(node, {0, -1, 1});
    while (it) {
        os << *it << ' ';
        ++it;
    }
    return os;
}

template<typename T>
std::ostream& operator << (std::ostream& os, const BinaryTree<T>& node)
{
    auto it = node.begin();
    while (it) {
        os << *it << ' ';
        ++it;
    }
    return os;
}

int main()
{
    BinaryTree<int> t = {1, 5, 89, 42, 9, 10, 55, 22, 34};
    t.set_pattern({1, 0, -1});

    std::cout << t;
    

}
