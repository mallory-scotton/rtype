///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <Engine/Config.hpp>
#include <functional>
#include <stdexcept>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A dynamic array (vector) container class.
///
/// \tparam T The type of elements stored in the vector.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TVector
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    using Reference = std::vector<T>::reference;
    using ConstReference = std::vector<T>::const_reference;
    using Iterator = typename std::vector<T>::iterator;
    using RIterator = typename std::vector<T>::reverse_iterator;
    using ConstIterator = typename std::vector<T>::const_iterator;
    using RConstIterator = typename std::vector<T>::const_reverse_iterator;

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    std::vector<T> m_data;   //<! The underlying data storage.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector(void)
        : m_data()
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with size.
    ///
    /// \param size The initial size of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector(SizeT size)
        : m_data(size)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with size and default value.
    ///
    /// \param size The initial size of the vector.
    /// \param value The default value to initialize elements with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector(SizeT size, const T& value)
        : m_data(size, value)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor with std::vector.
    ///
    /// \param other The other vector to copy from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector(const std::vector<T>& other)
        : m_data(other)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor with std::vector.
    ///
    /// \param other The other vector to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TVector(std::vector<T>&& other) noexcept
        : m_data(std::move(other))
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access an element of the vector.
    ///
    /// \param index The index of the element to access.
    ///
    /// \return A reference to the element at the specified index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& operator[](SizeT index) { return m_data[index]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access an element of the vector.
    ///
    /// \param index The index of the element to access.
    ///
    /// \return A constant reference to the element at the specified index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator[](SizeT index) const { return m_data[index]; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Convert to std::vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    operator std::vector<T>(void) const { return m_data; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clear the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Clear(void) { m_data.clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the size of the vector.
    ///
    /// \return The size of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT Size(void) const { return m_data.size(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Resize the vector.
    ///
    /// \param size The new size of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Resize(SizeT size) { m_data.resize(size); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access an element of the vector with bounds checking.
    ///
    /// \param index The index of the element to access.
    ///
    /// \return A reference to the element at the specified index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& At(SizeT index) { return m_data.at(index); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Access an element of the vector with bounds checking.
    ///
    /// \param index The index of the element to access.
    ///
    /// \return A constant reference to the element at the specified index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& At(SizeT index) const { return m_data.at(index); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the vector is empty.
    ///
    /// \return True if the vector is empty, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Empty(void) const { return m_data.empty(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add an element to the end of the vector.
    ///
    /// \param value The element to add.
    ///
    /// \return A reference to the added element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PushBack(const T& value) { m_data.push_back(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add an element to the end of the vector.
    ///
    /// \param value The element to add.
    ///
    /// \return A reference to the added element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PushBack(T&& value) { m_data.push_back(std::move(value)); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add an element to the front of the vector.
    ///
    /// \param value The element to add.
    ///
    /// \return A reference to the added element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PushFront(const T& value) { m_data.insert(m_data.begin(), value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add an element to the front of the vector.
    ///
    /// \param value The element to add.
    ///
    /// \return A reference to the added element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PushFront(T&& value)
    {
        m_data.insert(m_data.begin(), std::move(value));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove the last element of the vector.
    ///
    /// \return A reference to the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference PopBack(void)
    {
        Reference value = m_data.back();
        m_data.pop_back();
        return value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove the last element of the vector.
    ///
    /// \return A constant reference to the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference PopBack(void) const
    {
        ConstReference value = m_data.back();
        m_data.pop_back();
        return value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove the first element of the vector.
    ///
    /// \return A reference to the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference PopFront(void)
    {
        Reference value = m_data.front();
        m_data.erase(m_data.begin(), m_data.begin() + 1);
        return value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove the first element of the vector.
    ///
    /// \return A constant reference to the removed element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference PopFront(void) const
    {
        ConstReference value = m_data.front();
        m_data.erase(m_data.begin(), m_data.begin() + 1);
        return value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a pointer to the underlying data.
    ///
    /// \return A pointer to the underlying data.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T* Data(void) { return m_data.data(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a pointer to the underlying data.
    ///
    /// \return A pointer to the underlying data.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T* Data(void) const { return m_data.data(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an iterator to the beginning of the vector.
    ///
    /// \return An iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Begin(void) { return m_data.begin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an iterator to the beginning of the vector.
    ///
    /// \return An iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator begin(void) { return m_data.begin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant iterator to the beginning of the vector.
    ///
    /// \return A constant iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator Begin(void) const { return m_data.begin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant iterator to the beginning of the vector.
    ///
    /// \return A constant iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator begin(void) const { return m_data.begin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an iterator to the end of the vector.
    ///
    /// \return An iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator End(void) { return m_data.end(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an iterator to the end of the vector.
    ///
    /// \return An iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator end(void) { return m_data.end(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant iterator to the end of the vector.
    ///
    /// \return A constant iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator End(void) const { return m_data.end(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant iterator to the end of the vector.
    ///
    /// \return A constant iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator end(void) const { return m_data.end(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a reverse iterator to the beginning of the vector.
    ///
    /// \return A reverse iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RIterator RBegin(void) { return m_data.rbegin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a reverse iterator to the beginning of the vector.
    ///
    /// \return A reverse iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RIterator rbegin(void) { return m_data.rbegin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant reverse iterator to the beginning of the vector.
    ///
    /// \return A constant reverse iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RConstIterator RBegin(void) const { return m_data.rbegin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant reverse iterator to the beginning of the vector.
    ///
    /// \return A constant reverse iterator to the beginning of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RConstIterator rbegin(void) const { return m_data.rbegin(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a reverse iterator to the end of the vector.
    ///
    /// \return A reverse iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RIterator REnd(void) { return m_data.rend(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a reverse iterator to the end of the vector.
    ///
    /// \return A reverse iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RIterator rend(void) { return m_data.rend(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant reverse iterator to the end of the vector.
    ///
    /// \return A constant reverse iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RConstIterator REnd(void) const { return m_data.rend(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant reverse iterator to the end of the vector.
    ///
    /// \return A constant reverse iterator to the end of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    RConstIterator rend(void) const { return m_data.rend(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a reference to the first element of the vector.
    ///
    /// \return A reference to the first element of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference Front(void) { return m_data.front(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant reference to the first element of the vector.
    ///
    /// \return A constant reference to the first element of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference Front(void) const { return m_data.front(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a reference to the last element of the vector.
    ///
    /// \return A reference to the last element of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Reference Back(void) { return m_data.back(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a constant reference to the last element of the vector.
    ///
    /// \return A constant reference to the last element of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReference Back(void) const { return m_data.back(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Erase a range of elements from the vector.
    ///
    /// \param begin The beginning of the range to erase.
    /// \param end The end of the range to erase.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Erase(Iterator begin, Iterator end) { m_data.erase(begin, end); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Erase a range of elements from the vector.
    ///
    /// \param begin The beginning of the range to erase.
    /// \param end The end of the range to erase.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Erase(ConstIterator begin, ConstIterator end)
    {
        m_data.erase(begin, end);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swap the contents of this vector with another vector.
    ///
    /// \param other The other vector to swap with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Swap(TVector<T>& other) { m_data.swap(other.m_data); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the underlying std::vector.
    ///
    /// \return A reference to the underlying std::vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<T>& ToStdVector(void) { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the underlying std::vector.
    ///
    /// \return A constant reference to the underlying std::vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const std::vector<T>& ToStdVector(void) const { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reserve space in the vector.
    ///
    /// \param size The number of elements to reserve space for.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reserve(SizeT size) { m_data.reserve(size); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the capacity of the vector.
    ///
    /// \return The capacity of the vector.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT Capacity(void) const { return m_data.capacity(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reduce the capacity of the vector to fit its size.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShrinkToFit(void) { m_data.shrink_to_fit(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assign new contents to the vector.
    ///
    /// \param count The number of elements to assign.
    /// \param value The value to assign to each element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Assign(SizeT count, const T& value) { m_data.assign(count, value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assign new contents to the vector.
    ///
    /// \param first The beginning of the range to assign.
    /// \param last The end of the range to assign.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename InputIt>
    void Assign(InputIt first, InputIt last)
    {
        m_data.assign(first, last);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Insert an element at a specified position in the vector.
    ///
    /// \param pos The position to insert the element at.
    /// \param value The value to insert.
    ///
    /// \return An iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Insert(ConstIterator pos, const T& value)
    {
        return m_data.insert(pos, value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Insert an element at a specified position in the vector.
    ///
    /// \param pos The position to insert the element at.
    /// \param value The value to insert.
    ///
    /// \return An iterator to the inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Insert(ConstIterator pos, T&& value)
    {
        return m_data.insert(pos, std::move(value));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Insert multiple elements at a specified position in the vector.
    ///
    /// \param pos The position to insert the elements at.
    /// \param count The number of elements to insert.
    /// \param value The value to insert.
    ///
    /// \return An iterator to the first inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Insert(ConstIterator pos, SizeT count, const T& value)
    {
        return m_data.insert(pos, count, value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Insert multiple elements at a specified position in the vector.
    ///
    /// \param pos The position to insert the elements at.
    /// \param first The beginning of the range to insert.
    /// \param last The end of the range to insert.
    ///
    /// \return An iterator to the first inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename InputIt>
    Iterator Insert(ConstIterator pos, InputIt first, InputIt last)
    {
        return m_data.insert(pos, first, last);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emplace an element at a specified position in the vector.
    ///
    /// \param pos The position to emplace the element at.
    /// \param args The arguments to forward to the constructor of the element.
    ///
    /// \return An iterator to the emplaced element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    Iterator Emplace(ConstIterator pos, Args&&... args)
    {
        return m_data.emplace(pos, std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emplace an element at the end of the vector.
    ///
    /// \param args The arguments to forward to the constructor of the element.
    ///
    /// \return A reference to the emplaced element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    Reference EmplaceBack(Args&&... args)
    {
        m_data.emplace_back(std::forward<Args>(args)...);
        return m_data.back();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emplace an element at the front of the vector.
    ///
    /// \param args The arguments to forward to the constructor of the element.
    ///
    /// \return A reference to the emplaced element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    Reference EmplaceFront(Args&&... args)
    {
        m_data.emplace(m_data.begin(), std::forward<Args>(args)...);
        return m_data.front();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param func
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ForEach(const std::function<void(T&)>& func)
    {
        for (auto& element: m_data) { func(element); }
    }
};

}   // namespace tkd
