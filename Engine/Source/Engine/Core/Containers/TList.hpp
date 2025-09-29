///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Doubly-linked list container.
///
/// \tparam T The type of elements stored in the list.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TList
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Node structure for the doubly-linked list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Node
    {
        Node* Prev = nullptr;
        Node* Next = nullptr;
        value_type Value;

        template <class... Args>
        explicit Node(Args&&... args)
            : Prev(nullptr)
            , Next(nullptr)
            , Value(std::forward<Args>(args)...)
        {}
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bidirectional iterator for TList.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class iterator
    {
        friend class TList;
        Node* node_ = nullptr;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = TList::value_type;
        using difference_type = TList::difference_type;
        using pointer = value_type*;
        using reference = value_type&;

        iterator() = default;

        explicit iterator(Node* n)
            : node_(n)
        {}

        reference operator*() const
        {
            assert(node_ && "Dereferencing end iterator");
            return node_->Value;
        }

        pointer operator->() const
        {
            assert(node_ && "Dereferencing end iterator");
            return std::addressof(node_->Value);
        }

        iterator& operator++()
        {
            assert(node_ && "Incrementing end iterator");
            node_ = node_->Next;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        iterator& operator--()
        {
            assert(node_ && node_->Prev && "Decrementing begin iterator");
            node_ = node_->Prev;
            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const iterator& o) const { return node_ == o.node_; }

        bool operator!=(const iterator& o) const { return node_ != o.node_; }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Const bidirectional iterator for TList.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class const_iterator
    {
        friend class TList;
        const Node* node_ = nullptr;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = TList::value_type;
        using difference_type = TList::difference_type;
        using pointer = const value_type*;
        using reference = const value_type&;

        const_iterator() = default;

        explicit const_iterator(const Node* n)
            : node_(n)
        {}

        const_iterator(const iterator& it)
            : node_(it.node_)
        {}

        reference operator*() const
        {
            assert(node_ && "Dereferencing end iterator");
            return node_->Value;
        }

        pointer operator->() const
        {
            assert(node_ && "Dereferencing end iterator");
            return std::addressof(node_->Value);
        }

        const_iterator& operator++()
        {
            assert(node_ && "Incrementing end iterator");
            node_ = node_->Next;
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator& operator--()
        {
            assert(node_ && node_->Prev && "Decrementing begin iterator");
            node_ = node_->Prev;
            return *this;
        }

        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const const_iterator& o) const
        {
            return node_ == o.node_;
        }

        bool operator!=(const const_iterator& o) const
        {
            return node_ != o.node_;
        }
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Creates an empty list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor. Cleans up all nodes.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~TList() noexcept { clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another list.
    ///
    /// \param other The list to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList(const TList& other)
        : Head(nullptr)
        , Tail(nullptr)
        , Size(0)
    {
        for (const auto& item: other) { push_back(item); }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The list to assign from.
    ///
    /// \return Reference to this list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList& operator=(const TList& other)
    {
        if (this != &other)
        {
            clear();
            for (const auto& item: other) { push_back(item); }
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor. Moves the contents of another list.
    ///
    /// \param other The list to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList(TList&& other) noexcept
        : Head(other.Head)
        , Tail(other.Tail)
        , Size(other.Size)
    {
        other.Head = other.Tail = nullptr;
        other.Size = 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// \param other The list to move from.
    ///
    /// \return Reference to this list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList& operator=(TList&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            Head = other.Head;
            Tail = other.Tail;
            Size = other.Size;
            other.Head = other.Tail = nullptr;
            other.Size = 0;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reference to the first element.
    ///
    /// \return Reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reference front()
    {
        assert(Head && "TList::front on empty list");
        return Head->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reference to the first element.
    ///
    /// \return Const reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reference front() const
    {
        assert(Head && "TList::front on empty list");
        return Head->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reference to the last element.
    ///
    /// \return Reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reference back()
    {
        assert(Tail && "TList::back on empty list");
        return Tail->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reference to the last element.
    ///
    /// \return Const reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reference back() const
    {
        assert(Tail && "TList::back on empty list");
        return Tail->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the list is empty.
    ///
    /// \return True if the list is empty.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool empty() const noexcept { return Size == 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the number of elements in the list.
    ///
    /// \return The number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_type size() const noexcept { return Size; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an element at the front of the list.
    ///
    /// \tparam Args The types of arguments to forward.
    /// \param args The arguments to forward to the element constructor.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <class... Args>
    iterator push_front(Args&&... args)
    {
        Node* n = nullptr;
        try
        {
            n = new Node(std::forward<Args>(args)...);
        }
        catch (...)
        {
            delete n;
            throw;
        }

        n->Next = Head;
        if (Head) { Head->Prev = n; }
        Head = n;
        if (!Tail) { Tail = n; }
        ++Size;
        return iterator(n);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an element at the back of the list.
    ///
    /// \tparam Args The types of arguments to forward.
    /// \param args The arguments to forward to the element constructor.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <class... Args>
    iterator push_back(Args&&... args)
    {
        Node* n = nullptr;
        try
        {
            n = new Node(std::forward<Args>(args)...);
        }
        catch (...)
        {
            delete n;
            throw;
        }

        n->Prev = Tail;
        if (Tail) { Tail->Next = n; }
        Tail = n;
        if (!Head) { Head = n; }
        ++Size;
        return iterator(n);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the first element from the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void pop_front()
    {
        assert(Head && "TList::pop_front on empty list");
        Node* n = Head;
        Head = Head->Next;
        if (Head) { Head->Prev = nullptr; }
        else { Tail = nullptr; }
        delete n;
        --Size;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the last element from the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void pop_back()
    {
        assert(Tail && "TList::pop_back on empty list");
        Node* n = Tail;
        Tail = Tail->Prev;
        if (Tail) { Tail->Next = nullptr; }
        else { Head = nullptr; }
        delete n;
        --Size;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Adds an element to the front of the list.
    ///
    /// \param value The value to add.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator AddHead(const value_type& value) { return push_front(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Adds an element to the back of the list.
    ///
    /// \param value The value to add.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator AddTail(const value_type& value) { return push_back(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an element at the front of the list.
    ///
    /// \tparam Args The types of arguments to forward.
    /// \param args The arguments to forward to the element constructor.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <class... Args>
    iterator EmplaceHead(Args&&... args)
    {
        return push_front(std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an element at the back of the list.
    ///
    /// \tparam Args The types of arguments to forward.
    /// \param args The arguments to forward to the element constructor.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <class... Args>
    iterator EmplaceTail(Args&&... args)
    {
        return push_back(std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes and returns the first element from the list.
    ///
    /// \return The value of the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    value_type RemoveHead()
    {
        assert(Head && "TList::RemoveHead on empty list");
        value_type result = std::move(Head->Value);
        pop_front();
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes and returns the last element from the list.
    ///
    /// \return The value of the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    value_type RemoveTail()
    {
        assert(Tail && "TList::RemoveTail on empty list");
        value_type result = std::move(Tail->Value);
        pop_back();
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds an element in the list.
    ///
    /// \param value The value to find.
    ///
    /// \return Iterator to the found element, or end() if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator Find(const value_type& value)
    {
        for (Node* cur = Head; cur != nullptr; cur = cur->Next)
        {
            if (cur->Value == value) { return iterator(cur); }
        }
        return end();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds an element in the list (const version).
    ///
    /// \param value The value to find.
    ///
    /// \return Const iterator to the found element, or end() if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator Find(const value_type& value) const
    {
        for (const Node* cur = Head; cur != nullptr; cur = cur->Next)
        {
            if (cur->Value == value) { return const_iterator(cur); }
        }
        return end();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the list contains a specific value.
    ///
    /// \param value The value to check for.
    ///
    /// \return True if the value is found in the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Contains(const value_type& value) const
    {
        return Find(value) != end();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the number of elements in the list.
    ///
    /// \return The number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_type Num() const noexcept { return size(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the list is empty.
    ///
    /// \return True if the list is empty.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsEmpty() const noexcept { return empty(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clears the list of all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Empty() { clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a pointer to the head node.
    ///
    /// \return Pointer to the head node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Node* GetHead() noexcept { return Head; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const pointer to the head node.
    ///
    /// \return Const pointer to the head node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Node* GetHead() const noexcept { return Head; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a pointer to the tail node.
    ///
    /// \return Pointer to the tail node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Node* GetTail() noexcept { return Tail; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const pointer to the tail node.
    ///
    /// \return Const pointer to the tail node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Node* GetTail() const noexcept { return Tail; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts an element before the specified position.
    ///
    /// \param pos The position to insert before.
    /// \param value The value to insert.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator InsertBefore(const iterator& pos, const value_type& value)
    {
        return insert(pos, value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the element at the specified position.
    ///
    /// \param pos The position of the element to remove.
    ///
    /// \return Iterator to the element after the removed one.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator RemoveNode(const iterator& pos) { return erase(pos); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts an element before the specified position.
    ///
    /// \param pos The position to insert before.
    /// \param value The value to insert.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator insert(const iterator& pos, const value_type& value)
    {
        if (pos.node_ == nullptr) { return push_back(value); }
        Node* cur = pos.node_;
        Node* n = nullptr;
        try
        {
            n = new Node(value);
        }
        catch (...)
        {
            delete n;
            throw;
        }

        n->Prev = cur->Prev;
        n->Next = cur;
        if (cur->Prev) { cur->Prev->Next = n; }
        else { Head = n; }
        cur->Prev = n;
        ++Size;
        return iterator(n);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an element in-place before the specified position.
    ///
    /// \tparam Args The types of arguments to forward.
    /// \param pos The position to insert before.
    /// \param args The arguments to forward to the element constructor.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <class... Args>
    iterator emplace(const iterator& pos, Args&&... args)
    {
        if (pos.node_ == nullptr)
        {
            return push_back(std::forward<Args>(args)...);
        }
        Node* cur = pos.node_;
        Node* n = nullptr;
        try
        {
            n = new Node(std::forward<Args>(args)...);
        }
        catch (...)
        {
            delete n;
            throw;
        }

        n->Prev = cur->Prev;
        n->Next = cur;
        if (cur->Prev) { cur->Prev->Next = n; }
        else { Head = n; }
        cur->Prev = n;
        ++Size;
        return iterator(n);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the element at the specified position.
    ///
    /// \param pos The position of the element to remove.
    ///
    /// \return Iterator to the element after the removed one.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator erase(const iterator& pos)
    {
        Node* cur = pos.node_;
        if (!cur) { return iterator(nullptr); }
        iterator nextIt(cur->Next);
        if (cur->Prev) { cur->Prev->Next = cur->Next; }
        else { Head = cur->Next; }
        if (cur->Next) { cur->Next->Prev = cur->Prev; }
        else { Tail = cur->Prev; }
        delete cur;
        --Size;
        return nextIt;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes all elements from the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void clear() noexcept
    {
        Node* cur = Head;
        while (cur)
        {
            Node* next = cur->Next;
            delete cur;
            cur = next;
        }
        Head = Tail = nullptr;
        Size = 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swaps the contents with another list.
    ///
    /// \param other The list to swap with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void swap(TList& other) noexcept
    {
        if (this == &other) { return; }
        std::swap(Head, other.Head);
        std::swap(Tail, other.Tail);
        std::swap(Size, other.Size);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the beginning of the list.
    ///
    /// \return Iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator begin() noexcept { return iterator(Head); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the list.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator begin() const noexcept { return const_iterator(Head); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the list.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator cbegin() const noexcept { return const_iterator(Head); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the end of the list.
    ///
    /// \return Iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator end() noexcept { return iterator(nullptr); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the list.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator end() const noexcept { return const_iterator(nullptr); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the list.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator cend() const noexcept { return const_iterator(nullptr); }

private:
    Node* Head = nullptr;
    Node* Tail = nullptr;
    size_type Size = 0;
};

}   // namespace tkd
