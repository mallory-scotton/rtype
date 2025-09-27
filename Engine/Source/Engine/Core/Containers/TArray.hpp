///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

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
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    value_type data_[N];   //!< Array data storage.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Default-initializes all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray() = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs the array with the given initializer list.
    ///
    /// \param init The initializer list containing values to copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray(std::initializer_list<value_type> init)
    {
        std::copy(
            init.begin(), init.begin() + std::min(init.size(), N), data_
        );
        if (init.size() < N)
        {
            std::fill(data_ + init.size(), data_ + N, value_type{});
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
        std::copy(other.begin(), other.end(), data_);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor. Moves the contents of another array.
    ///
    /// \param other The array to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TArray(TArray&& other) noexcept
    {
        std::move(other.begin(), other.end(), data_);
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
        if (this != &other) { std::copy(other.begin(), other.end(), data_); }
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
        if (this != &other) { std::move(other.begin(), other.end(), data_); }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified index.
    ///
    /// \param index The index of the element to access.
    /// \return Reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reference operator[](size_type index) { return data_[index]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified index (const version).
    ///
    /// \param index The index of the element to access.
    /// \return Const reference to the element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reference operator[](size_type index) const { return data_[index]; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified position with bounds
    /// checking.
    ///
    /// \param pos The position of the element to access.
    /// \return Reference to the element.
    /// \throws std::out_of_range if pos >= size().
    ///
    ///////////////////////////////////////////////////////////////////////////
    reference at(size_type pos)
    {
        if (pos >= N)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the element at the specified position with bounds
    /// checking (const version).
    ///
    /// \param pos The position of the element to access.
    /// \return Const reference to the element.
    /// \throws std::out_of_range if pos >= size().
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reference at(size_type pos) const
    {
        if (pos >= N)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the first element.
    ///
    /// \return Reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reference front() { return data_[0]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the first element (const version).
    ///
    /// \return Const reference to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reference front() const { return data_[0]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the last element.
    ///
    /// \return Reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reference back() { return data_[N - 1]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses the last element (const version).
    ///
    /// \return Const reference to the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reference back() const { return data_[N - 1]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the address of the first element.
    ///
    /// \return Pointer to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    pointer data() { return data_; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the address of the first element (const version).
    ///
    /// \return Const pointer to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_pointer data() const { return data_; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the beginning of the array.
    ///
    /// \return Iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator begin() { return data_; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the array.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator begin() const { return data_; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the array.
    ///
    /// \return Const iterator to the first element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator cbegin() const { return data_; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the end of the array.
    ///
    /// \return Iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator end() { return data_ + N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the array.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator end() const { return data_ + N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the array.
    ///
    /// \return Const iterator to one past the last element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator cend() const { return data_ + N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reverse iterator to the reverse beginning.
    ///
    /// \return Reverse iterator to the first element of the reversed array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reverse_iterator rbegin() { return reverse_iterator(end()); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse beginning.
    ///
    /// \return Const reverse iterator to the first element of the reversed
    /// array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse beginning.
    ///
    /// \return Const reverse iterator to the first element of the reversed
    /// array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reverse iterator to the reverse end.
    ///
    /// \return Reverse iterator to one past the last element of the reversed
    /// array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    reverse_iterator rend() { return reverse_iterator(begin()); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse end.
    ///
    /// \return Const reverse iterator to one past the last element of the
    /// reversed array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the reverse end.
    ///
    /// \return Const reverse iterator to one past the last element of the
    /// reversed array.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks whether the array is empty.
    ///
    /// \return True if the array is empty, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr bool empty() const noexcept { return N == 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the number of elements in the array.
    ///
    /// \return The number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr size_type size() const noexcept { return N; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the maximum possible number of elements.
    ///
    /// \return The maximum number of elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    constexpr size_type max_size() const noexcept { return N; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces all elements with the specified value.
    ///
    /// \param value The value to assign to all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void fill(const value_type& value) { std::fill(data_, data_ + N, value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swaps the contents with another array.
    ///
    /// \param other The array to swap with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void swap(TArray& other) noexcept(std::is_nothrow_swappable_v<value_type>)
    {
        std::swap_ranges(data_, data_ + N, other.data_);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assigns values to all elements (deprecated, use fill instead).
    ///
    /// \param value The value to assign to all elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    [[deprecated("Use fill() instead")]] void assign(const value_type& value)
    {
        fill(value);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Specialization for zero-sized arrays
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TArray<T, 0>
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    TArray() = default;

    reference operator[](size_type) { return *static_cast<pointer>(nullptr); }

    const_reference operator[](size_type) const
    {
        return *static_cast<const_pointer>(nullptr);
    }

    reference at(size_type) { throw std::out_of_range("TArray<T,0>::at"); }

    const_reference at(size_type) const
    {
        throw std::out_of_range("TArray<T,0>::at");
    }

    reference front() { return *static_cast<pointer>(nullptr); }

    const_reference front() const
    {
        return *static_cast<const_pointer>(nullptr);
    }

    reference back() { return *static_cast<pointer>(nullptr); }

    const_reference back() const
    {
        return *static_cast<const_pointer>(nullptr);
    }

    pointer data() { return nullptr; }

    const_pointer data() const { return nullptr; }

    iterator begin() { return iterator{}; }

    const_iterator begin() const { return const_iterator{}; }

    const_iterator cbegin() const { return const_iterator{}; }

    iterator end() { return iterator{}; }

    const_iterator end() const { return const_iterator{}; }

    const_iterator cend() const { return const_iterator{}; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

    constexpr bool empty() const noexcept { return true; }

    constexpr size_type size() const noexcept { return 0; }

    constexpr size_type max_size() const noexcept { return 0; }

    void fill(const value_type&) {}

    void swap(TArray&) noexcept {}

    [[deprecated("Use fill() instead")]] void assign(const value_type&) {}
};

///////////////////////////////////////////////////////////////////////////////
// Specialization for 2-element array with named access
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TArray<T, 2>
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    union
    {
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4201)
#endif
        struct
        {
            T x;   //!< The first component.
            T y;   //!< The second component.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data_[2];   //!< Array access to the components.
    };

public:
    TArray() = default;

    TArray(std::initializer_list<value_type> init)
    {
        std::copy(
            init.begin(),
            init.begin() + std::min(init.size(), size_t(2)),
            data_
        );
        if (init.size() < 2)
        {
            std::fill(data_ + init.size(), data_ + 2, value_type{});
        }
    }

    TArray(const TArray& other)
    {
        std::copy(other.begin(), other.end(), data_);
    }

    TArray(TArray&& other) noexcept
    {
        std::move(other.begin(), other.end(), data_);
    }

    TArray& operator=(const TArray& other)
    {
        if (this != &other) { std::copy(other.begin(), other.end(), data_); }
        return *this;
    }

    TArray& operator=(TArray&& other) noexcept
    {
        if (this != &other) { std::move(other.begin(), other.end(), data_); }
        return *this;
    }

    reference operator[](size_type index) { return data_[index]; }

    const_reference operator[](size_type index) const { return data_[index]; }

    reference at(size_type pos)
    {
        if (pos >= 2)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= 2)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    reference front() { return data_[0]; }

    const_reference front() const { return data_[0]; }

    reference back() { return data_[1]; }

    const_reference back() const { return data_[1]; }

    pointer data() { return data_; }

    const_pointer data() const { return data_; }

    iterator begin() { return data_; }

    const_iterator begin() const { return data_; }

    const_iterator cbegin() const { return data_; }

    iterator end() { return data_ + 2; }

    const_iterator end() const { return data_ + 2; }

    const_iterator cend() const { return data_ + 2; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

    constexpr bool empty() const noexcept { return false; }

    constexpr size_type size() const noexcept { return 2; }

    constexpr size_type max_size() const noexcept { return 2; }

    void fill(const value_type& value) { std::fill(data_, data_ + 2, value); }

    void swap(TArray& other) noexcept(std::is_nothrow_swappable_v<value_type>)
    {
        std::swap_ranges(data_, data_ + 2, other.data_);
    }

    [[deprecated("Use fill() instead")]] void assign(const value_type& value)
    {
        fill(value);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Specialization for 3-element array with named access
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TArray<T, 3>
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    union
    {
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4201)
#endif
        struct
        {
            T x;   //!< The first component.
            T y;   //!< The second component.
            T z;   //!< The third component.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data_[3];   //!< Array access to the components.
    };

public:
    TArray() = default;

    TArray(std::initializer_list<value_type> init)
    {
        std::copy(
            init.begin(),
            init.begin() + std::min(init.size(), size_t(3)),
            data_
        );
        if (init.size() < 3)
        {
            std::fill(data_ + init.size(), data_ + 3, value_type{});
        }
    }

    TArray(const TArray& other)
    {
        std::copy(other.begin(), other.end(), data_);
    }

    TArray(TArray&& other) noexcept
    {
        std::move(other.begin(), other.end(), data_);
    }

    TArray& operator=(const TArray& other)
    {
        if (this != &other) { std::copy(other.begin(), other.end(), data_); }
        return *this;
    }

    TArray& operator=(TArray&& other) noexcept
    {
        if (this != &other) { std::move(other.begin(), other.end(), data_); }
        return *this;
    }

    reference operator[](size_type index) { return data_[index]; }

    const_reference operator[](size_type index) const { return data_[index]; }

    reference at(size_type pos)
    {
        if (pos >= 3)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= 3)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    reference front() { return data_[0]; }

    const_reference front() const { return data_[0]; }

    reference back() { return data_[2]; }

    const_reference back() const { return data_[2]; }

    pointer data() { return data_; }

    const_pointer data() const { return data_; }

    iterator begin() { return data_; }

    const_iterator begin() const { return data_; }

    const_iterator cbegin() const { return data_; }

    iterator end() { return data_ + 3; }

    const_iterator end() const { return data_ + 3; }

    const_iterator cend() const { return data_ + 3; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

    constexpr bool empty() const noexcept { return false; }

    constexpr size_type size() const noexcept { return 3; }

    constexpr size_type max_size() const noexcept { return 3; }

    void fill(const value_type& value) { std::fill(data_, data_ + 3, value); }

    void swap(TArray& other) noexcept(std::is_nothrow_swappable_v<value_type>)
    {
        std::swap_ranges(data_, data_ + 3, other.data_);
    }

    [[deprecated("Use fill() instead")]] void assign(const value_type& value)
    {
        fill(value);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Specialization for 4-element array with named access
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TArray<T, 4>
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    union
    {
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4201)
#endif
        struct
        {
            T x;   //!< The first component.
            T y;   //!< The second component.
            T z;   //!< The third component.
            T w;   //!< The fourth component.
        };
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
        T data_[4];   //!< Array access to the components.
    };

public:
    TArray() = default;

    TArray(std::initializer_list<value_type> init)
    {
        std::copy(
            init.begin(),
            init.begin() + std::min(init.size(), size_t(4)),
            data_
        );
        if (init.size() < 4)
        {
            std::fill(data_ + init.size(), data_ + 4, value_type{});
        }
    }

    TArray(const TArray& other)
    {
        std::copy(other.begin(), other.end(), data_);
    }

    TArray(TArray&& other) noexcept
    {
        std::move(other.begin(), other.end(), data_);
    }

    TArray& operator=(const TArray& other)
    {
        if (this != &other) { std::copy(other.begin(), other.end(), data_); }
        return *this;
    }

    TArray& operator=(TArray&& other) noexcept
    {
        if (this != &other) { std::move(other.begin(), other.end(), data_); }
        return *this;
    }

    reference operator[](size_type index) { return data_[index]; }

    const_reference operator[](size_type index) const { return data_[index]; }

    reference at(size_type pos)
    {
        if (pos >= 4)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= 4)
        {
            throw std::out_of_range("TArray::at: index out of range");
        }
        return data_[pos];
    }

    reference front() { return data_[0]; }

    const_reference front() const { return data_[0]; }

    reference back() { return data_[3]; }

    const_reference back() const { return data_[3]; }

    pointer data() { return data_; }

    const_pointer data() const { return data_; }

    iterator begin() { return data_; }

    const_iterator begin() const { return data_; }

    const_iterator cbegin() const { return data_; }

    iterator end() { return data_ + 4; }

    const_iterator end() const { return data_ + 4; }

    const_iterator cend() const { return data_ + 4; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

    constexpr bool empty() const noexcept { return false; }

    constexpr size_type size() const noexcept { return 4; }

    constexpr size_type max_size() const noexcept { return 4; }

    void fill(const value_type& value) { std::fill(data_, data_ + 4, value); }

    void swap(TArray& other) noexcept(std::is_nothrow_swappable_v<value_type>)
    {
        std::swap_ranges(data_, data_ + 4, other.data_);
    }

    [[deprecated("Use fill() instead")]] void assign(const value_type& value)
    {
        fill(value);
    }
};

///////////////////////////////////////////////////////////////////////////////
// Non-member functions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if two arrays are equal.
///
/// \tparam T The element type.
/// \tparam N The array size.
/// \param lhs The first array.
/// \param rhs The second array.
/// \return True if all corresponding elements are equal.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator==(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Checks if two arrays are not equal.
///
/// \tparam T The element type.
/// \tparam N The array size.
/// \param lhs The first array.
/// \param rhs The second array.
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
/// \param lhs The first array.
/// \param rhs The second array.
/// \return True if lhs is lexicographically less than rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator<(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end()
    );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lexicographically compares two arrays.
///
/// \tparam T The element type.
/// \tparam N The array size.
/// \param lhs The first array.
/// \param rhs The second array.
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
/// \param lhs The first array.
/// \param rhs The second array.
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
/// \param lhs The first array.
/// \param rhs The second array.
/// \return True if lhs is lexicographically greater than or equal to rhs.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
bool operator>=(const TArray<T, N>& lhs, const TArray<T, N>& rhs)
{
    return !(lhs < rhs);
}

///////////////////////////gi////////////////////////////////////////////////////
/// \brief Swaps the contents of two arrays.
///
/// \tparam T The element type.
/// \tparam N The array size.
/// \param lhs The first array.
/// \param rhs The second array.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
void swap(TArray<T, N>& lhs, TArray<T, N>& rhs) noexcept(
    noexcept(lhs.swap(rhs))
)
{
    lhs.swap(rhs);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Gets a reference to the element at the specified index.
///
/// \tparam I The index.
/// \tparam T The element type.
/// \tparam N The array size.
/// \param arr The array.
/// \return Reference to the element at index I.
///
///////////////////////////////////////////////////////////////////////////////
template <std::size_t I, typename T, std::size_t N>
constexpr T& get(TArray<T, N>& arr) noexcept
{
    static_assert(I < N, "Index out of bounds");
    return arr[I];
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Gets a const reference to the element at the specified index.
///
/// \tparam I The index.
/// \tparam T The element type.
/// \tparam N The array size.
/// \param arr The array.
/// \return Const reference to the element at index I.
///
///////////////////////////////////////////////////////////////////////////////
template <std::size_t I, typename T, std::size_t N>
constexpr const T& get(const TArray<T, N>& arr) noexcept
{
    static_assert(I < N, "Index out of bounds");
    return arr[I];
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Gets an rvalue reference to the element at the specified index.
///
/// \tparam I The index.
/// \tparam T The element type.
/// \tparam N The array size.
/// \param arr The array.
/// \return Rvalue reference to the element at index I.
///
///////////////////////////////////////////////////////////////////////////////
template <std::size_t I, typename T, std::size_t N>
constexpr T&& get(TArray<T, N>&& arr) noexcept
{
    static_assert(I < N, "Index out of bounds");
    return std::move(arr[I]);
}

}   // namespace tkd
