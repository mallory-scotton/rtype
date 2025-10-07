///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Template array container with fixed size at compile time.
///
/// \tparam T The type of elements stored in the array.
/// \tparam N The number of elements in the array.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
class TArray
{
public:
    static_assert(N > 0, "Array size must be greater than 0");

public:
    ///////////////////////////////////////////////////////////////////////////
    // Type definitions
    ///////////////////////////////////////////////////////////////////////////
    using ValueType = T;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Reference = ValueType&;
    using ConstReference = const ValueType&;
    using Pointer = ValueType*;
    using ConstPointer = const ValueType*;
    using Iterator = Pointer;
    using ConstIterator = ConstPointer;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    ValueType m_data[N];   //<! Array data storage.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Default-initializes all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray(void) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs the array with the given initializer list.
    ///
    /// \param init The initializer list containing values to copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray(std::initializer_list<ValueType> init)
    {
        std::copy(
            init.begin(), init.begin() + std::min(init.size(), N), m_data
        );
        if (init.size() < N)
        {
            std::fill(m_data + init.size(), m_data + N, ValueType{});
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor. Creates a copy of another array.
    ///
    /// \param other The array to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray(const TArray& other)
    {
        std::copy(other.Begin(), other.End(), m_data);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor. Moves the contents of another array.
    ///
    /// \param other The array to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray(TArray&& other) noexcept
    {
        std::move(other.Begin(), other.End(), m_data);
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The array to assign from.
    /// \return Reference to this array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray& operator=(const TArray& other)
    {
        if (this != &other) { std::copy(other.Begin(), other.End(), m_data); }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// \param other The array to move from.
    /// \return Reference to this array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray& operator=(TArray&& other) noexcept
    {
        if (this != &other) { std::move(other.Begin(), other.End(), m_data); }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified index.
    ///
    /// \param index The index of the element to access.
    /// \return Reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference operator[](SizeType index) { return m_data[index]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified index (const version).
    ///
    /// \param index The index of the element to access.
    ///
    /// \return Const reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference operator[](SizeType index) const { return m_data[index]; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified position with bounds
    /// checking.
    ///
    /// \param pos The position of the element to access.
    ///
    /// \return Reference to the element.
    ///
    /// \throws std::out_of_range if pos >= size().
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference At(SizeType pos)
    {
        if (pos >= N)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return m_data[pos];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified position with bounds
    /// checking (const version).
    ///
    /// \param pos The position of the element to access.
    ///
    /// \return Const reference to the element.
    ///
    /// \throws std::out_of_range if pos >= size().
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference At(SizeType pos) const
    {
        if (pos >= N)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return m_data[pos];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the first element.
    ///
    /// \return Reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference Front(void) { return m_data[0]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the first element (const version).
    ///
    /// \return Const reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference Front(void) const { return m_data[0]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the last element.
    ///
    /// \return Reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference Back(void) { return m_data[N - 1]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the last element (const version).
    ///
    /// \return Const reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference Back(void) const { return m_data[N - 1]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the address of the first element.
    ///
    /// \return Pointer to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Pointer Data(void) { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the address of the first element (const version).
    ///
    /// \return Const pointer to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstPointer Data(void) const { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the beginning of the array.
    ///
    /// \return Iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Begin(void) { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the array.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator Begin(void) const { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the array.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator CBegin(void) const { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the end of the array.
    ///
    /// \return Iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator End(void) { return m_data + N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the array.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator End(void) const { return m_data + N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the array.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator CEnd(void) const { return m_data + N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reverse iterator to the reverse beginning.
    ///
    /// \return Reverse iterator to the first element of the reversed array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ReverseIterator RBegin(void) { return ReverseIterator(End()); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse beginning.
    ///
    /// \return Const reverse iterator to the first element of the reversed
    /// array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator RBegin(void) const
    {
        return ConstReverseIterator(End());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse beginning.
    ///
    /// \return Const reverse iterator to the first element of the reversed
    /// array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator CRBegin(void) const
    {
        return ConstReverseIterator(End());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reverse iterator to the reverse end.
    ///
    /// \return Reverse iterator to one past the last element of the reversed
    /// array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ReverseIterator REnd(void) { return ReverseIterator(Begin()); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse end.
    ///
    /// \return Const reverse iterator to one past the last element of the
    /// reversed array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator REnd(void) const
    {
        return ConstReverseIterator(Begin());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse end.
    ///
    /// \return Const reverse iterator to one past the last element of the
    /// reversed array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator CREnd(void) const
    {
        return ConstReverseIterator(Begin());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks whether the array is empty.
    ///
    /// \return True if the array is empty, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr bool Empty(void) const noexcept { return N == 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the number of elements in the array.
    ///
    /// \return The number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr SizeType Size(void) const noexcept { return N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the maximum possible number of elements.
    ///
    /// \return The maximum number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr SizeType MaxSize(void) const noexcept { return N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces all elements with the specified value.
    ///
    /// \param value The value to assign to all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Fill(const ValueType& value) { std::fill(m_data, m_data + N, value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swaps the contents with another array.
    ///
    /// \param other The array to swap with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Swap(TArray& other) noexcept(std::is_nothrow_swappable_v<ValueType>)
    {
        std::swap_ranges(m_data, m_data + N, other.m_data);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assigns values to all elements (deprecated, use fill instead).
    ///
    /// \param value The value to assign to all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    [[deprecated("Use fill() instead")]] void Assign(const ValueType& value)
    {
        Fill(value);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if two arrays are equal.
///
/// \tparam T The element type.
/// \tparam N The array size.
///
/// \param lhs The first array.
/// \param rhs The second array.
///
/// \return True if all corresponding elements are equal.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator==(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return std::equal(lhs.Begin(), lhs.End(), rhs.Begin());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if two arrays are not equal.
///
/// \tparam T The element type.
/// \tparam N The array size.
///
/// \param lhs The first array.
/// \param rhs The second array.
///
/// \return True if any corresponding elements are not equal.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator!=(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lexicographically compares two arrays.
///
/// \tparam T The element type.
/// \tparam N The array size.
///
/// \param lhs The first array.
/// \param rhs The second array.
///
/// \return True if lhs is lexicographically less than rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator<(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return std::lexicographical_compare(
        lhs.Begin(), lhs.End(), rhs.Begin(), rhs.End()
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lexicographically compares two arrays.
///
/// \tparam T The element type.
/// \tparam N The array size.
///
/// \param lhs The first array.
/// \param rhs The second array.
///
/// \return True if lhs is lexicographically less than or equal to rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator<=(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return !(rhs < lhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lexicographically compares two arrays.
///
/// \tparam T The element type.
/// \tparam N The array size.
///
/// \param lhs The first array.
/// \param rhs The second array.
///
/// \return True if lhs is lexicographically greater than rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator>(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return rhs < lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lexicographically compares two arrays.
///
/// \tparam T The element type.
/// \tparam N The array size.
///
/// \param lhs The first array.
/// \param rhs The second array.
///
/// \return True if lhs is lexicographically greater than or equal to rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator>=(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return !(lhs < rhs);
}

}   // namespace tkd
