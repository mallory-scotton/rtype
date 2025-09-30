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
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    using ValueType = T;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Reference = ValueType&;
    using ConstReference = const ValueType&;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Node structure for the doubly-linked list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Node
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        Node* prev = nullptr;
        Node* next = nullptr;
        ValueType value;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructor that forwards arguments to construct the node's
        ///
        /// \tparam Args The types of arguments to forward.
        ///
        /// \param args The arguments to forward to the value constructor.
        ///
        ///////////////////////////////////////////////////////////////////////
        template <class... Args>
        explicit Node(Args&&... args)
            : prev(nullptr)
            , next(nullptr)
            , value(std::forward<Args>(args)...)
        {}
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    Node* m_head = nullptr;   //<! The head node.
    Node* m_tail = nullptr;   //<! The tail node.
    SizeType m_size = 0;      //<! Number of elements in the list.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Bidirectional iterator for TList.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        friend class TList;
        Node* m_node = nullptr;

    public:
        ///////////////////////////////////////////////////////////////////////
        // Class Aliases
        ///////////////////////////////////////////////////////////////////////
        using IteratorCategory = std::bidirectional_iterator_tag;
        using ValueType = TList::ValueType;
        using DifferenceType = TList::DifferenceType;
        using Pointer = ValueType*;
        using Reference = ValueType&;

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator(void) = default;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructor that initializes the iterator to point to a
        /// node.
        ///
        /// \param n The node to point to.
        ///
        ///////////////////////////////////////////////////////////////////////
        explicit Iterator(Node* node)
            : m_node(node)
        {}

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator.
        ///
        /// \return Reference to the element's value.
        ///
        ///////////////////////////////////////////////////////////////////////
        Reference operator*(void) const
        {
            assert(m_node && "Dereferencing end iterator");
            return m_node->Value;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Access the member of the element pointed to by the iterator.
        ///
        /// \return Pointer to the element's value.
        ///
        ///////////////////////////////////////////////////////////////////////
        Pointer operator->(void) const
        {
            assert(m_node && "Dereferencing end iterator");
            return std::addressof(m_node->Value);
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Increment operator (prefix).
        ///
        /// \return Reference to this iterator after incrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator& operator++(void)
        {
            assert(m_node && "Incrementing end iterator");
            m_node = m_node->Next;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Increment operator (postfix).
        ///
        /// \return A copy of the iterator before incrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Decrement operator (prefix).
        ///
        /// \return Reference to this iterator after decrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator& operator--(void)
        {
            assert(m_node && m_node->Prev && "Decrementing begin iterator");
            m_node = m_node->Prev;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Decrement operator (postfix).
        ///
        /// \return A copy of the iterator before decrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator operator--(int)
        {
            Iterator tmp = *this;
            --*this;
            return tmp;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Checks equality between two iterators.
        ///
        /// \param o The other iterator to compare with.
        ///
        /// \return True if the iterators point to the same node.
        ///
        ///////////////////////////////////////////////////////////////////////
        bool operator==(const Iterator& o) const { return m_node == o.m_node; }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Checks inequality between two iterators.
        ///
        /// \param o The other iterator to compare with.
        ///
        /// \return True if the iterators point to different nodes.
        ///
        ///////////////////////////////////////////////////////////////////////
        bool operator!=(const Iterator& o) const { return m_node != o.m_node; }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Const bidirectional iterator for TList.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class ConstIterator
    {
    private:
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        friend class TList;
        const Node* m_node = nullptr;

    public:
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        using IteratorCategory = std::bidirectional_iterator_tag;
        using ValueType = TList::ValueType;
        using DifferenceType = TList::DifferenceType;
        using Pointer = const ValueType*;
        using Reference = const ValueType&;

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator(void) = default;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructor that initializes the iterator to point to a
        /// specific node.
        ///
        ///////////////////////////////////////////////////////////////////////
        explicit ConstIterator(const Node* node)
            : m_node(node)
        {}

        ///////////////////////////////////////////////////////////////////////
        /// \brief Import an Iterator as a ConstIterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator(const Iterator& it)
            : m_node(it.m_node)
        {}

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator.
        ///
        /// \return Reference to the element's value.
        ///
        ///////////////////////////////////////////////////////////////////////
        Reference operator*(void) const
        {
            assert(m_node && "Dereferencing end iterator");
            return m_node->Value;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Access the member of the element pointed to by the iterator.
        ///
        /// \return Pointer to the element's value.
        ///
        ///////////////////////////////////////////////////////////////////////
        Pointer operator->(void) const
        {
            assert(m_node && "Dereferencing end iterator");
            return std::addressof(m_node->Value);
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Increment operator (prefix).
        ///
        /// \return Reference to this iterator after incrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator& operator++(void)
        {
            assert(m_node && "Incrementing end iterator");
            m_node = m_node->Next;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Increment operator (postfix).
        ///
        /// \return A copy of the iterator before incrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator operator++(int)
        {
            ConstIterator tmp = *this;
            ++*this;
            return tmp;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Decrement operator (prefix).
        ///
        /// \return Reference to this iterator after decrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator& operator--(void)
        {
            assert(m_node && m_node->Prev && "Decrementing begin iterator");
            m_node = m_node->Prev;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Decrement operator (postfix).
        ///
        /// \return A copy of the iterator before decrementing.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator operator--(int)
        {
            ConstIterator tmp = *this;
            --*this;
            return tmp;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Checks equality between two const iterators.
        ///
        /// \param o The other iterator to compare with.
        ///
        /// \return True if the iterators point to the same node.
        ///
        ///////////////////////////////////////////////////////////////////////
        bool operator==(const ConstIterator& o) const
        {
            return m_node == o.m_node;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Checks inequality between two const iterators.
        ///
        /// \param o The other iterator to compare with.
        ///
        /// \return True if the iterators point to different nodes.
        ///
        ///////////////////////////////////////////////////////////////////////
        bool operator!=(const ConstIterator& o) const
        {
            return m_node != o.m_node;
        }
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Creates an empty list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList(void) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor. Cleans up all nodes.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~TList() noexcept { Clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another list.
    ///
    /// \param other The list to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TList(const TList& other)
        : m_head(nullptr)
        , m_tail(nullptr)
        , m_size(0)
    {
        for (const auto& item: other) { PushBack(item); }
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
            Clear();
            for (const auto& item: other) { PushBack(item); }
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
        : m_head(other.m_head)
        , m_tail(other.m_tail)
        , m_size(other.m_size)
    {
        other.m_head = other.m_tail = nullptr;
        other.m_size = 0;
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
            Clear();
            m_head = other.m_head;
            m_tail = other.m_tail;
            m_size = other.m_size;
            other.m_head = other.m_tail = nullptr;
            other.m_size = 0;
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reference to the first element.
    ///
    /// \return Reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference Front(void)
    {
        assert(m_head && "TList::front on empty list");
        return m_head->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reference to the first element.
    ///
    /// \return Const reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference Front(void) const
    {
        assert(m_head && "TList::front on empty list");
        return m_head->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reference to the last element.
    ///
    /// \return Reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference Back(void)
    {
        assert(m_tail && "TList::back on empty list");
        return m_tail->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reference to the last element.
    ///
    /// \return Const reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference Back(void) const
    {
        assert(m_tail && "TList::back on empty list");
        return m_tail->Value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the list is empty.
    ///
    /// \return True if the list is empty.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Empty(void) const noexcept { return m_size == 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the number of elements in the list.
    ///
    /// \return The number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Size(void) const noexcept { return m_size; }

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
    Iterator PushFront(Args&&... args)
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

        n->Next = m_head;
        if (m_head) { m_head->Prev = n; }
        m_head = n;
        if (!m_tail) { m_tail = n; }
        ++m_size;
        return Iterator(n);
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
    Iterator PushBack(Args&&... args)
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

        n->Prev = m_tail;
        if (m_tail) { m_tail->Next = n; }
        m_tail = n;
        if (!m_head) { m_head = n; }
        ++m_size;
        return Iterator(n);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the first element from the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PopFront(void)
    {
        assert(m_head && "TList::pop_front on empty list");
        Node* n = m_head;
        m_head = m_head->Next;
        if (m_head) { m_head->Prev = nullptr; }
        else { m_tail = nullptr; }
        delete n;
        --m_size;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the last element from the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PopBack(void)
    {
        assert(m_tail && "TList::pop_back on empty list");
        Node* n = m_tail;
        m_tail = m_tail->Prev;
        if (m_tail) { m_tail->Next = nullptr; }
        else { m_head = nullptr; }
        delete n;
        --m_size;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Adds an element to the front of the list.
    ///
    /// \param value The value to add.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator AddHead(const ValueType& value) { return PushFront(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Adds an element to the back of the list.
    ///
    /// \param value The value to add.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator AddTail(const ValueType& value) { return PushBack(value); }

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
    Iterator EmplaceHead(Args&&... args)
    {
        return PushFront(std::forward<Args>(args)...);
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
    Iterator EmplaceTail(Args&&... args)
    {
        return PushBack(std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes and returns the first element from the list.
    ///
    /// \return The value of the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ValueType RemoveHead(void)
    {
        assert(m_head && "TList::RemoveHead on empty list");
        ValueType result = std::move(m_head->Value);
        PopFront();
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes and returns the last element from the list.
    ///
    /// \return The value of the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ValueType RemoveTail(void)
    {
        assert(m_tail && "TList::RemoveTail on empty list");
        ValueType result = std::move(m_tail->Value);
        PopBack();
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
    Iterator Find(const ValueType& value)
    {
        for (Node* cur = m_head; cur != nullptr; cur = cur->Next)
        {
            if (cur->Value == value) { return Iterator(cur); }
        }
        return End();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds an element in the list (const version).
    ///
    /// \param value The value to find.
    ///
    /// \return Const iterator to the found element, or end() if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator Find(const ValueType& value) const
    {
        for (const Node* cur = m_head; cur != nullptr; cur = cur->Next)
        {
            if (cur->Value == value) { return ConstIterator(cur); }
        }
        return End();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the list contains a specific value.
    ///
    /// \param value The value to check for.
    ///
    /// \return True if the value is found in the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Contains(const ValueType& value) const
    {
        return Find(value) != End();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the number of elements in the list.
    ///
    /// \return The number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Num(void) const noexcept { return Size(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the list is empty.
    ///
    /// \return True if the list is empty.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsEmpty(void) const noexcept { return Empty(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a pointer to the head node.
    ///
    /// \return Pointer to the head node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Node* GetHead(void) noexcept { return m_head; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const pointer to the head node.
    ///
    /// \return Const pointer to the head node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Node* GetHead(void) const noexcept { return m_head; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a pointer to the tail node.
    ///
    /// \return Pointer to the tail node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Node* GetTail(void) noexcept { return m_tail; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const pointer to the tail node.
    ///
    /// \return Const pointer to the tail node.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const Node* GetTail() const noexcept { return m_tail; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts an element before the specified position.
    ///
    /// \param pos The position to insert before.
    /// \param value The value to insert.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator InsertBefore(const Iterator& pos, const ValueType& value)
    {
        return Insert(pos, value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the element at the specified position.
    ///
    /// \param pos The position of the element to remove.
    ///
    /// \return Iterator to the element after the removed one.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator RemoveNode(const Iterator& pos) { return Erase(pos); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts an element before the specified position.
    ///
    /// \param pos The position to insert before.
    /// \param value The value to insert.
    ///
    /// \return Iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Insert(const Iterator& pos, const ValueType& value)
    {
        if (pos.node_ == nullptr) { return PushBack(value); }
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
        else { m_head = n; }
        cur->Prev = n;
        ++m_size;
        return Iterator(n);
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
    Iterator Emplace(const Iterator& pos, Args&&... args)
    {
        if (pos.node_ == nullptr)
        {
            return PushBack(std::forward<Args>(args)...);
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
        else { m_head = n; }
        cur->Prev = n;
        ++m_size;
        return Iterator(n);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the element at the specified position.
    ///
    /// \param pos The position of the element to remove.
    ///
    /// \return Iterator to the element after the removed one.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Erase(const Iterator& pos)
    {
        Node* cur = pos.node_;
        if (!cur) { return Iterator(nullptr); }
        Iterator nextIt(cur->Next);
        if (cur->Prev) { cur->Prev->Next = cur->Next; }
        else { m_head = cur->Next; }
        if (cur->Next) { cur->Next->Prev = cur->Prev; }
        else { m_tail = cur->Prev; }
        delete cur;
        --m_size;
        return nextIt;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes all elements from the list.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Clear(void) noexcept
    {
        Node* cur = m_head;
        while (cur)
        {
            Node* next = cur->Next;
            delete cur;
            cur = next;
        }
        m_head = m_tail = nullptr;
        m_size = 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swaps the contents with another list.
    ///
    /// \param other The list to swap with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Swap(TList& other) noexcept
    {
        if (this == &other) { return; }
        std::swap(m_head, other.m_head);
        std::swap(m_tail, other.m_tail);
        std::swap(m_size, other.m_size);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the beginning of the list.
    ///
    /// \return Iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Begin(void) noexcept { return Iterator(m_head); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the list.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator Begin(void) const noexcept { return ConstIterator(m_head); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the list.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator CBegin(void) const noexcept { return ConstIterator(m_head); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the end of the list.
    ///
    /// \return Iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator End(void) noexcept { return Iterator(nullptr); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the list.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator End(void) const noexcept { return ConstIterator(nullptr); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the list.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator CEnd(void) const noexcept { return ConstIterator(nullptr); }
};

}   // namespace tkd
