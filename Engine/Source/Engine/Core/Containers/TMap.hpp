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
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A unordered_map wrapper
///
/// \param KeyType The type of keys used in the map.
/// \param ValueType The type of values stored in the map.
/// \param Hash Hashing functor type (defaults to std::hash<KeyType>).
/// \param KeyEqual Equality comparator type (defaults to
/// std::equal_to<KeyType>).
///
///////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename ValueType,
    typename Hash = std::hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>>
class TMap
{
public:
    static_assert(
        !std::is_reference<KeyType>::value, "KeyType must not be a reference"
    );
    static_assert(
        !std::is_reference<ValueType>::value,
        "ValueType must not be a reference"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    using KeyT = KeyType;            //!< Key type.
    using ValueT = ValueType;        //!< Value type.
    using PairType = std::pair<const KeyType, ValueType>;
    using SizeType = std::size_t;    //!< Size type.
    using HashType = Hash;           //!< Hash functor type.
    using KeyEqualType = KeyEqual;   //!< Key equality functor type.

    using underlying_type =
        std::unordered_map<KeyType, ValueType, Hash, KeyEqual>;
    using iterator = typename underlying_type::iterator;
    using const_iterator = typename underlying_type::const_iterator;

public:
    ///////////////////////////////////////////////////////////////////////////
    // Static Member
    ///////////////////////////////////////////////////////////////////////////
    // (No static members for now)

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor. Creates an empty map.
    ///
    /// \return A default-constructed, empty `TMap`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap()
        : m_map()
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor that reserves space for approximately `bucketCount`
    /// buckets.
    ///
    /// \param bucketCount Number of buckets to reserve in the underlying
    /// container.
    /// \return A `TMap` with reserved capacity for `bucketCount` buckets.
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit TMap(SizeType bucketCount)
        : m_map(bucketCount)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructs a map from an initializer list of key/value pairs.
    ///
    /// \param init Initializer list of pairs to insert into the map.
    /// \return A `TMap` containing the elements from `init`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap(std::initializer_list<PairType> init)
        : m_map(init.begin(), init.end())
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor.
    ///
    /// \param other The map to copy from.
    /// \return A copy of `other`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap(const TMap& other) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor.
    ///
    /// \param other The map to move from.
    /// \return A `TMap` that has taken ownership of `other`'s contents.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap(TMap&& other) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// \param other The map to copy from.
    /// \return Reference to this `TMap` after copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap& operator=(const TMap& other) = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// \param other The map to move from.
    /// \return Reference to this `TMap` after move.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap& operator=(TMap&& other) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///
    /// \return Destroys the container and its elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~TMap() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a key/value pair to the map. Returns true if inserted
    /// (key was not already present).
    ///
    /// \param key The key to insert.
    /// \param value The value to insert.
    /// \return True if the pair was inserted; false if key already existed.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Add(const KeyType& key, const ValueType& value)
    {
        auto res = m_map.insert(std::make_pair(key, value));
        return res.second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a key/value pair by moving the key or value. Returns true
    /// if inserted.
    ///
    /// \param key The key to insert (rvalue).
    /// \param value The value to insert (rvalue).
    /// \return True if the pair was inserted; false if key already existed.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Add(KeyType&& key, ValueType&& value)
    {
        auto res =
            m_map.insert(std::make_pair(std::move(key), std::move(value)));
        return res.second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emplace a value constructed in-place for the given key. Returns
    /// true if inserted.
    ///
    /// \param key The key for the element.
    /// \param args Arguments forwarded to `ValueType` constructor.
    /// \return True if the element was inserted; false if an element with
    /// the same key already existed.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    bool Emplace(const KeyType& key, Args&&... args)
    {
        auto res = m_map.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(key),
            std::forward_as_tuple(std::forward<Args>(args)...)
        );
        return res.second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove the element with `key`. Returns true if an element was
    /// removed.
    ///
    /// \param key The key to remove.
    /// \return True if an element was removed; false if key was not present.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Remove(const KeyType& key) { return m_map.erase(key) > 0; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all elements from the map.
    ///
    /// \return This function does not return a value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Clear() { m_map.clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check whether the map contains `key`.
    ///
    /// \param key The key to search for.
    /// \return True if the key is present, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Contains(const KeyType& key) const
    {
        return m_map.find(key) != m_map.end();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Number of elements currently stored.
    ///
    /// \return The number of elements in the map.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Num() const noexcept { return m_map.size(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Whether the map is empty.
    ///
    /// \return True if the map contains no elements; false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Empty() const noexcept { return m_map.empty(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reserve buckets for at least `count` elements.
    ///
    /// \param count The number of elements to reserve space for.
    /// \return This function does not return a value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reserve(SizeType count) { m_map.reserve(count); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of buckets in the underlying container.
    ///
    /// \return The number of buckets used by the underlying unordered_map.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType GetBucketCount() const noexcept { return m_map.bucket_count(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Iterator access.
    ///
    /// \return Iterators to iterate over the contained elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    iterator begin() noexcept { return m_map.begin(); }

    iterator end() noexcept { return m_map.end(); }

    const_iterator begin() const noexcept { return m_map.begin(); }

    const_iterator end() const noexcept { return m_map.end(); }

    const_iterator cbegin() const noexcept { return m_map.cbegin(); }

    const_iterator cend() const noexcept { return m_map.cend(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extract the underlying std::unordered_map by moving it out.
    ///
    /// \return The underlying `std::unordered_map` moved out from this
    /// `TMap`.
    ///
    ///////////////////////////////////////////////////////////////////////////
    underlying_type&& ExtractUnderlying() && { return std::move(m_map); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Find the value for `key`.
    ///
    /// \param key The key to find.
    /// \return Pointer to the value if found, `nullptr` otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ValueType* Find(const KeyType& key)
    {
        auto it = m_map.find(key);
        return it == m_map.end() ? nullptr : &it->second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Find the value for `key` (const version).
    ///
    /// \param key The key to find.
    /// \return Const pointer to the value if found, `nullptr` otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const ValueType* Find(const KeyType& key) const
    {
        auto it = m_map.find(key);
        return it == m_map.end() ? nullptr : &it->second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Find or add a value for `key`. If an entry exists, returns
    /// reference to it. Otherwise inserts `defaultValue` and returns
    /// reference to the newly inserted value.
    ///
    /// \param key The key to find or add.
    /// \param defaultValue The default value to insert if missing.
    /// \return Reference to the existing or newly inserted value.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ValueType& FindOrAdd(
        const KeyType& key, const ValueType& defaultValue = ValueType()
    )
    {
        auto it = m_map.find(key);
        if (it != m_map.end()) { return it->second; }
        auto res = m_map.emplace(key, defaultValue);
        return res.first->second;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Append elements from another map into this one. Existing keys
    /// are overwritten with values from `other`.
    ///
    /// \param other The other map whose elements will be added to this map.
    /// \return Number of elements actually added (new keys inserted).
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Append(const TMap& other)
    {
        SizeType added = 0;
        for (const auto& kv: other.m_map)
        {
            auto res = m_map.insert_or_assign(kv.first, kv.second);
            if (res.second) { ++added; }
        }
        return added;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compute intersection between this map and `other`, writing
    /// result into `out`. Only pairs with the same key and equal value are
    /// included.
    ///
    /// \param other The other map to intersect with.
    /// \param out Output map that will contain intersection results (cleared
    /// first).
    /// \return Number of elements in the intersection.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeType Intersection(const TMap& other, TMap& out) const
    {
        out.Clear();
        SizeType count = 0;
        for (const auto& kv: m_map)
        {
            auto it = other.m_map.find(kv.first);
            if (it != other.m_map.end() && it->second == kv.second)
            {
                out.m_map.insert(kv);
                ++count;
            }
        }
        return count;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Union-assignment: add all elements from `other` into this map
    /// (overwrites existing values).
    ///
    /// \param other Map whose elements will be inserted into this map.
    /// \return Reference to this map after insertion.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap& operator|=(const TMap& other)
    {
        Append(other);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Intersection-assignment: keep only elements present in both
    /// maps with equal values.
    ///
    /// \param other Map to intersect with.
    /// \return Reference to this map after intersection.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap& operator&=(const TMap& other)
    {
        for (auto it = m_map.begin(); it != m_map.end();)
        {
            auto otherIt = other.m_map.find(it->first);
            if (otherIt == other.m_map.end() || otherIt->second != it->second)
            {
                it = m_map.erase(it);
            }
            else { ++it; }
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Difference-assignment: remove elements present in `other`
    /// (keys present in other) from this map.
    ///
    /// \param other Map whose keys will be removed from this map.
    /// \return Reference to this map after removal.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TMap& operator-=(const TMap& other)
    {
        for (const auto& kv: other.m_map) { m_map.erase(kv.first); }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Return a vector containing keys from the map.
    ///
    /// \return A `std::vector<KeyType>` with copies of the keys.
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<KeyType> GenerateKeyArray() const
    {
        std::vector<KeyType> out;
        out.reserve(m_map.size());
        for (const auto& kv: m_map) { out.push_back(kv.first); }
        return out;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Return a vector containing values from the map.
    ///
    /// \return A `std::vector<ValueType>` with copies of the values.
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<ValueType> GenerateValueArray() const
    {
        std::vector<ValueType> out;
        out.reserve(m_map.size());
        for (const auto& kv: m_map) { out.push_back(kv.second); }
        return out;
    }

private:
    underlying_type m_map;   //!< The underlying container.
};

}   // namespace tkd

///////////////////////////////////////////////////////////////////////////////
// Free operators and comparisons for TMap
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

/////////////////////////////////////////////////////////////////////////////
/// \brief Return a new map containing the union of `lho` and `rho`.
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return A new `TMap` containing elements present in either `lho` or `rho`.
///
/////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename ValueType,
    typename Hash,
    typename KeyEqual>
TMap<KeyType, ValueType, Hash, KeyEqual> operator|(
    const TMap<KeyType, ValueType, Hash, KeyEqual>& lho,
    const TMap<KeyType, ValueType, Hash, KeyEqual>& rho
)
{
    TMap<KeyType, ValueType, Hash, KeyEqual> result = lho;
    result |= rho;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Return a new map containing the intersection of `lho` and `rho`.
/// Only pairs with same key and equal value are kept.
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return A new `TMap` containing elements present in both `lho` and `rho`.
///
/////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename ValueType,
    typename Hash,
    typename KeyEqual>
TMap<KeyType, ValueType, Hash, KeyEqual> operator&(
    const TMap<KeyType, ValueType, Hash, KeyEqual>& lho,
    const TMap<KeyType, ValueType, Hash, KeyEqual>& rho
)
{
    TMap<KeyType, ValueType, Hash, KeyEqual> result = lho;
    result &= rho;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Return a new map containing the difference of `lho` and `rho`
/// (entries in `lho` whose keys are not present in `rho`).
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return A new `TMap` containing elements present in `lho` but not in
/// `rho`.
///
/////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename ValueType,
    typename Hash,
    typename KeyEqual>
TMap<KeyType, ValueType, Hash, KeyEqual> operator-(
    const TMap<KeyType, ValueType, Hash, KeyEqual>& lho,
    const TMap<KeyType, ValueType, Hash, KeyEqual>& rho
)
{
    TMap<KeyType, ValueType, Hash, KeyEqual> result = lho;
    result -= rho;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Compare two maps for equality (same keys mapping to equal
/// values).
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return True if both maps have the same keys and corresponding values
/// are equal, false otherwise.
///
/////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename ValueType,
    typename Hash,
    typename KeyEqual>
bool operator==(
    const TMap<KeyType, ValueType, Hash, KeyEqual>& lho,
    const TMap<KeyType, ValueType, Hash, KeyEqual>& rho
)
{
    if (lho.Num() != rho.Num()) { return false; }
    for (const auto& kv: lho)
    {
        auto it = rho.Find(kv.first);
        if (!it) { return false; }
        if (*it != kv.second) { return false; }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Inequality comparison for maps (negation of `operator==`).
///
/// \param lho Left-hand operand.
/// \param rho Right-hand operand.
/// \return True if the maps are not equal; false otherwise.
///
/////////////////////////////////////////////////////////////////////////////
template <
    typename KeyType,
    typename ValueType,
    typename Hash,
    typename KeyEqual>
bool operator!=(
    const TMap<KeyType, ValueType, Hash, KeyEqual>& lho,
    const TMap<KeyType, ValueType, Hash, KeyEqual>& rho
)
{
    return !(lho == rho);
}

}   // namespace tkd
