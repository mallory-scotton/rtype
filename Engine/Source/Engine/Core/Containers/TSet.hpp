///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <unordered_set>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A small, header-only set container inspired by Unreal Engine's TSet.
///
/// This class wraps `std::unordered_set` and exposes a subset of operations
/// commonly used in engine code. It's implemented as a template and therefore
/// header-only.
///
/// 	param KeyType The type of elements stored in the set.
/// 	param Hash Hashing functor type (defaults to std::hash<KeyType>).
/// 	param KeyEqual Equality comparator type (defaults to
/// std::equal_to<KeyType>).
///
///////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename Hash = std::hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>>
class TSet
{
public:
    static_assert(
        !std::is_reference<KeyType>::value, "KeyType must not be a reference"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    using ElementType = KeyType;     //!< Element type stored in the set.
    using SizeType = std::size_t;    //!< Size type.
    using HashType = Hash;           //!< Hash functor type.
    using KeyEqualType = KeyEqual;   //!< Key equality functor type.

    using underlying_type = std::unordered_set<KeyType, Hash, KeyEqual>;
    using iterator = typename underlying_type::iterator;
    using const_iterator = typename underlying_type::const_iterator;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    // (No static members for now)

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Creates an empty set.
    ///
    /// \return A default-constructed, empty `TSet`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet()
        : m_set()
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor that reserves space for approximately `bucketCount`
    /// buckets.
    ///
    /// \param bucketCount Number of buckets to reserve in the underlying
    /// container.
    /// \return A `TSet` with reserved capacity for `bucketCount` buckets.
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit TSet(SizeType bucketCount)
        : m_set(bucketCount)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a set from an initializer list of values.
    ///
    /// \param init Initializer list of values to insert into the set.
    /// \return A `TSet` containing the elements from `init`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet(std::initializer_list<KeyType> init)
        : m_set(init.begin(), init.end())
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor.
    ///
    /// \param other The set to copy from.
    /// \return A copy of `other`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet(const TSet& other) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor.
    ///
    /// \param other The set to move from.
    /// \return A `TSet` that has taken ownership of `other`'s contents.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet(TSet&& other) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The set to copy from.
    /// \return Reference to this `TSet` after copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet& operator=(const TSet& other) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// \param other The set to move from.
    /// \return Reference to this `TSet` after move.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet& operator=(TSet&& other) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///
    /// \return Destroys the container and its elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~TSet() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add an element to the set. Returns true if inserted (was not
    /// already present).
    ///
    /// \param value The value to insert.
    /// \return True if the value was inserted; false if it was already
    /// present.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Add(const KeyType& value)
    {
        auto res = m_set.insert(value);
        return res.second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add an element to the set by moving it in. Returns true if
    /// inserted.
    ///
    /// \param value The value to move into the set.
    /// \return True if the value was inserted; false if it was already
    /// present.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Add(KeyType&& value)
    {
        auto res = m_set.insert(std::move(value));
        return res.second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emplace a new element constructed in-place. Returns true if
    /// inserted.
    ///
    /// 	param Args Parameter pack used to construct the element in-place.
    /// \param args Arguments forwarded to the element's constructor.
    /// \return True if the element was inserted; false if an equivalent
    /// element was already present.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    bool Emplace(Args&&... args)
    {
        auto res = m_set.emplace(std::forward<Args>(args)...);
        return res.second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove an element equal to `value`. Returns true if an element
    /// was removed.
    ///
    /// \param value The value to remove.
    /// \return True if an element was removed; false if the value was not
    /// present.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Remove(const KeyType& value) { return m_set.erase(value) > 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all elements from the set.
    ///
    /// \return This function does not return a value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Clear() { m_set.clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check whether the set contains `value`.
    ///
    /// \param value The value to search for.
    /// \return True if the value is present, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Contains(const KeyType& value) const
    {
        return m_set.find(value) != m_set.end();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Number of elements currently stored.
    ///
    /// \return The number of elements in the set.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Num() const noexcept { return m_set.size(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Whether the set is empty.
    ///
    /// \return True if the set contains no elements; false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Empty() const noexcept { return m_set.empty(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reserve buckets for at least `count` elements.
    ///
    /// \param count The number of elements to reserve space for.
    /// \return This function does not return a value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reserve(SizeType count) { m_set.reserve(count); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of buckets in the underlying container.
    ///
    /// \return The number of buckets used by the underlying unordered_set.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType GetBucketCount() const noexcept { return m_set.bucket_count(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Iterator access.
    ///
    /// \return Iterators to iterate over the contained elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator begin() noexcept { return m_set.begin(); }

    iterator end() noexcept { return m_set.end(); }

    const_iterator begin() const noexcept { return m_set.begin(); }

    const_iterator end() const noexcept { return m_set.end(); }

    const_iterator cbegin() const noexcept { return m_set.cbegin(); }

    const_iterator cend() const noexcept { return m_set.cend(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extract the underlying std::unordered_set by moving it out.
    ///
    /// \return The underlying `std::unordered_set` moved out from this `TSet`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    underlying_type&& ExtractUnderlying() && { return std::move(m_set); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Find an element equal to `value`.
    ///
    /// \param value The value to find.
    /// \return Iterator to the found element or `end()` if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator Find(const KeyType& value) { return m_set.find(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Find an element equal to `value` (const version).
    ///
    /// \param value The value to find.
    /// \return Const iterator to the found element or `cend()` if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const_iterator Find(const KeyType& value) const
    {
        return m_set.find(value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Find or add an element. If an equivalent element exists, returns
    /// iterator to it. Otherwise inserts `value` and returns iterator to the
    /// new element.
    ///
    /// \param value The value to find or add.
    /// \return Iterator to the existing or newly inserted element.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator FindOrAdd(const KeyType& value)
    {
        auto res = m_set.insert(value);
        return res.first;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Append elements from another set into this one.
    ///
    /// \param other The other set whose elements will be added to this set.
    /// \return Number of elements actually added.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Append(const TSet& other)
    {
        SizeType added = 0;
        for (const auto& el: other.m_set)
        {
            auto res = m_set.insert(el);
            if (res.second) { ++added; }
        }
        return added;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compute intersection between this set and `other`, writing
    /// result into `out`.
    ///
    /// \param other The other set to intersect with.
    /// \param out Output set that will contain intersection results (cleared
    /// first).
    /// \return Number of elements in the intersection.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Intersection(const TSet& other, TSet& out) const
    {
        out.Clear();
        SizeType count = 0;
        for (const auto& el: m_set)
        {
            if (other.Contains(el))
            {
                out.m_set.insert(el);
                ++count;
            }
        }
        return count;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Union-assignment: add all elements from `other` into this set.
    ///
    /// \param other Set whose elements will be inserted into this set.
    /// \return Reference to this set after insertion.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet& operator|=(const TSet& other)
    {
        Append(other);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Intersection-assignment: keep only elements present in both
    /// sets.
    ///
    /// \param other Set to intersect with.
    /// \return Reference to this set after intersection.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet& operator&=(const TSet& other)
    {
        for (auto it = m_set.begin(); it != m_set.end();)
        {
            if (!other.Contains(*it)) { it = m_set.erase(it); }
            else { ++it; }
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Difference-assignment: remove elements present in `other` from
    /// this set.
    ///
    /// \param other Set whose elements will be removed from this set.
    /// \return Reference to this set after removal.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TSet& operator-=(const TSet& other)
    {
        for (const auto& el: other.m_set) { m_set.erase(el); }
        return *this;
    }

private:
    underlying_type m_set;   //!< The underlying container.
};

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
// Free operators and comparisons for TSet
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

/////////////////////////////////////////////////////////////////////////////
/// \brief Return a new set containing the union of `lho` and `rho`.
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return A new `TSet` containing elements present in either `lho` or `rho`.
///
/////////////////////////////////////////////////////////////////////////////
template <typename KeyType, typename Hash, typename KeyEqual>
TSet<KeyType, Hash, KeyEqual> operator|(
    const TSet<KeyType, Hash, KeyEqual>& lho,
    const TSet<KeyType, Hash, KeyEqual>& rho
)
{
    TSet<KeyType, Hash, KeyEqual> result = lho;
    result |= rho;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Return a new set containing the intersection of `lho` and `rho`.
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return A new `TSet` containing elements present in both `lho` and `rho`.
///
/////////////////////////////////////////////////////////////////////////////
template <typename KeyType, typename Hash, typename KeyEqual>
TSet<KeyType, Hash, KeyEqual> operator&(
    const TSet<KeyType, Hash, KeyEqual>& lho,
    const TSet<KeyType, Hash, KeyEqual>& rho
)
{
    TSet<KeyType, Hash, KeyEqual> result = lho;
    result &= rho;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Return a new set containing the difference of `lho` and `rho`
/// (elements in `lho` not in `rho`).
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return A new `TSet` containing elements present in `lho` but not in `rho`.
///
/////////////////////////////////////////////////////////////////////////////
template <typename KeyType, typename Hash, typename KeyEqual>
TSet<KeyType, Hash, KeyEqual> operator-(
    const TSet<KeyType, Hash, KeyEqual>& lho,
    const TSet<KeyType, Hash, KeyEqual>& rho
)
{
    TSet<KeyType, Hash, KeyEqual> result = lho;
    result -= rho;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Compare two sets for equality (contain the same elements).
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return True if both sets contain the same elements (order-independent),
/// false otherwise.
///
/////////////////////////////////////////////////////////////////////////////
template <typename KeyType, typename Hash, typename KeyEqual>
bool operator==(
    const TSet<KeyType, Hash, KeyEqual>& lho,
    const TSet<KeyType, Hash, KeyEqual>& rho
)
{
    if (lho.Num() != rho.Num()) { return false; }
    for (const auto& el: lho)
    {
        if (!rho.Contains(el)) { return false; }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Inequality comparison for sets (negation of `operator==`).
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return True if the sets are not equal; false otherwise.
///
/////////////////////////////////////////////////////////////////////////////
template <typename KeyType, typename Hash, typename KeyEqual>
bool operator!=(
    const TSet<KeyType, Hash, KeyEqual>& lho,
    const TSet<KeyType, Hash, KeyEqual>& rho
)
{
    return !(lho == rho);
}

}   // namespace tkd
