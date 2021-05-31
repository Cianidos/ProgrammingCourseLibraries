#pragma once

#include <array>
#include <concepts>
#include <functional>
#include <stack>
#include <utility>

template <typename T, size_t N>
struct Node
{
    T data{};
    Node* parent = nullptr;
    std::array<Node*, N> children = {nullptr};
    int my_index = -1;

    void link(size_t index, Node* child)
    {
        children[index] = child;
        child->parent = this;
        child->my_index = index;
    }
    void unlink()
    {
        parent->children[my_index] = nullptr;
        parent = nullptr;
    }
};


template <typename T, size_t N>
struct NodeIterator
{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    using node = Node<T, N>;

    node* root = nullptr;
    node* current = nullptr;
    
    std::array<int, N + 1> pattern{};
    std::stack<int> state{};
    
    void next()
    {
        if (state.empty()) 
            return;
        if (state.top() == (N + 1))
        {
            current = current->parent;
            state.pop();
            return next();
        }
        if (pattern.at(state.top()) == -1)
        {
            state.top() += 1;
            return;
        }
        auto* child = current->children.at(pattern.at(state.top())); 
        if (child == nullptr)
        {
            state.top() += 1;
            return next();
        }
        state.top() += 1;

        current = child;
        state.push(0);

        return next();
    }


    NodeIterator(node* root, std::array<int, N + 1> pattern)
        : root(root), current(root), pattern(pattern)
    {
        if (root != nullptr)
        {
            state.push(0);
            next();
        }
    }

    NodeIterator& operator++()
    {
        next();
        return *this;
    }

    reference operator*()
    {
        return current->data;
    }

    pointer operator->()
    {
        return &current->data;
    }

    bool operator==(const NodeIterator& other) const
    {
        return static_cast<bool>(other) == static_cast<bool>(*this);
    }


    operator bool() const
    {
        return !state.empty();
    }
};


template<typename T, size_t N, typename Selector>
requires std::is_invocable_r_v<bool, Selector, const T&, const T&> && (N == 2)
    || std::is_invocable_r_v<size_t, Selector, const T&, const T&> && (N != 2)

std::pair<Node<T, N>*, bool>
node_search(Node<T, N>* tree, const T& value)
{
    static Selector selector{};
    using node = Node<T, N>;

    size_t child_less_id = selector(value, tree->data);
    size_t child_greater_id = selector(tree->data, value);

    if(!child_less_id && !child_greater_id)
        return {tree, true};
    
    node* child = tree->children.at(child_greater_id);

    if (child == nullptr)
        return {tree, false};

    return node_search<T, N, Selector>(child, value);
}


template<
std::totally_ordered T,
std::relation<T, T> Compare = std::less<T>>
struct BinaryTree
{
    using iterator = NodeIterator<T, 2>;

private:
    using BNode = Node<T, 2>;

    std::function<std::pair<Node<T, 2>*, bool>(Node<T, 2>*, const T&)>
    b_search = node_search<T, 2, Compare>;

    size_t _size = 0;
    Compare _cmp{};

    mutable std::array<int, 3> _pattern{0, -1, 1};

public:
    BNode* _root = nullptr;

    BinaryTree() = default;
    BinaryTree(const BinaryTree& other)
    {
        auto tmp = other._pattern;
        other.set_pattern({-1, 0, 1});
        for (auto i : other)
        {
            insert(i);
        }
        other.set_pattern(tmp);
    }

    BinaryTree(BinaryTree&& other) noexcept
    {
        _root = std::move(other._root);
        _size = std::move(other._size);
        _pattern = std::move(other._pattern);
    }

    template<std::input_iterator IteratorType>
    BinaryTree(IteratorType begin_it, IteratorType end_it)
    {
        for (;begin_it != end_it; ++begin_it)
        {
            insert(*begin_it);
        }
    }

    BinaryTree(std::initializer_list<T> list)
    {
        for (auto i : list)
        {
            insert(i);
        }
    }

    ~BinaryTree() noexcept
    {
        _pattern = {0, 1, -1};
        auto it = begin();
        while(it)
        {
            auto tmp = it.current;
            ++it;
            remove(tmp);
        }
    }

    void set_pattern(std::array<int, 3> pattern) const
    { _pattern = pattern; }

    size_t size() const
    {
        return _size;
    }

    bool insert(T value)
    {
        auto* node = new BNode{ value };
        if (_root == nullptr)
        {
            _root = node;
            _size += 1;
            return true;
        }
        else
            return insert(node);
    }


    bool contains(const T& value) const
    {
        return b_search(_root, value).second;
    }

    bool remove(const T& value)
    {
        auto [leaf, inside] = b_search(_root, value);
        return ((inside) ? remove(leaf) : inside);
    }

    iterator begin() noexcept
    {
        return {_root, _pattern};
    }
    iterator end() noexcept
    {
        return {nullptr, _pattern};
    }

    iterator begin() const noexcept
    {
        return {_root, _pattern};
    }
    iterator end() const noexcept
    {
        return {nullptr, _pattern};
    }

private:

    bool insert(BNode* node)
    {
        if (node == nullptr)
            return false;
        auto [leaf, inside] = b_search(_root, node->data);
        if (inside) 
            return false;
        leaf->link(!_cmp(node->data, leaf->data), node);
        _size += 1;
        return true;
    }

    bool remove(BNode* node) noexcept
    {
        auto [left, right] = node->children;
        if (node == _root)
        {
            remove_root();
        }
        else
        {
            node->unlink();
            delete node;
            _size -= insert(left);
            _size -= insert(right);
            _size -= 1;
        }
        return true;
    }

    bool remove_root()
    {
        auto children = _root->children;
        auto* root = _root;
        auto it = std::find_if(children.begin(), children.end(), [](auto x) { return x != nullptr;});
        if (it != children.end())
        {
            _root = *it;
            _size -= insert((it == children.begin()) ? children[1] : children[0]);
        }
        else
        {
            _root = nullptr;
        }
        delete root;
        _size -= 1;
        return true;
    }
};

template<std::input_iterator IteratorType>
BinaryTree(IteratorType begin_it, IteratorType end_it)
-> BinaryTree<typename std::iterator_traits<IteratorType>::value_type>;

