///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <cstring>
#include <Engine/Config.hpp>
#include <iostream>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A simplified string class for managing and manipulating C-style
/// character arrays.
///
/// This `String` class provides dynamic memory management for a character
/// array, handling allocation, reallocation, and capacity growth as the string
/// expands. It also includes basic types for managing string size and special
/// constants like `npos` for indicating "no position".
///
///////////////////////////////////////////////////////////////////////////////
class FString
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Member data.
    ///
    ///////////////////////////////////////////////////////////////////////////
    using sizeType = SizeT;    //<! Type alias for string size type.
    static const SizeT NPOS;   //<! The largest possible value.

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Private member data.
    ///
    ///////////////////////////////////////////////////////////////////////////
    char* m_data = nullptr;   //<! Pointer to the character array.
    SizeT m_length = 0;       //<! Length of the string.
    SizeT m_capacity = 0;     //<! Capacity of the string.
    SizeT m_increment = 15;   //<! Increment size for capacity growth.

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A structure used for iterating over the string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct StringIteratorType
    {
        ///////////////////////////////////////////////////////////////////////
        /// \brief Member data.
        ///
        /// This iterator type helps manage iteration over the characters of
        /// the string, keeping track of the current position and reference
        /// to the size.
        ///
        ///////////////////////////////////////////////////////////////////////
        char* first;         //<! Pointer to the first character in the string.
        const SizeT* size;   //<! Pointer to the size of the string.
        sizeType pos;        //<! Current position in the string.

        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor.
        ///
        /// Initializes the iterator with the given parameters, or with default
        /// values if none are provided.
        ///
        /// \param f Pointer to the first character.
        /// \param s Pointer to the size of the string.
        /// \param p Current position in the string.
        ///
        ///////////////////////////////////////////////////////////////////////
        StringIteratorType(
            char* f = nullptr, const SizeT* s = nullptr, sizeType p = 0
        )
            : first(f)
            , size(s)
            , pos(p)
        {}
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief ConstIterator class for iterating over the String object.
    ///
    /// This class provides a way to access and traverse the characters in a
    /// String object in a constant manner, ensuring that the underlying data
    /// remains unmodified during iteration.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class ConstIterator
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor for ConstIterator.
        ///
        /// Initializes an empty ConstIterator that does not point to any valid
        /// data.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructs a ConstIterator from a StringIteratorType.
        ///
        /// \param p The StringIteratorType that specifies the starting
        /// position of the iterator in the String object.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator(StringIteratorType p);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator to access the character at the current
        /// iterator position.
        ///
        /// Provides constant access to the character pointed by the iterator.
        ///
        /// \return A constant reference to the character.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator*(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subscript operator to access a character at an offset from
        /// the current iterator position.
        ///
        /// \param index Offset from the current iterator position.
        ///
        /// \return A constant reference to the character at the specified
        /// offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator[](int index) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-increment operator to move the iterator to the next
        /// position.
        ///
        /// Advances the iterator to the next character in the String.
        ///
        /// \return A reference to the incremented iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator& operator++(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-increment operator to move the iterator to the next
        /// position.
        ///
        /// Advances the iterator to the next character, but returns a copy of
        /// the iterator before the increment.
        ///
        /// \return A copy of the iterator before the increment.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator operator++(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-decrement operator to move the iterator to the previous
        /// position.
        ///
        /// Moves the iterator to the previous character in the String.
        ///
        /// \return A reference to the decremented iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator& operator--(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-decrement operator to move the iterator to the previous
        /// position.
        ///
        /// Moves the iterator to the previous character but returns a copy of
        /// the iterator before the decrement.
        ///
        /// \return A copy of the iterator before the decrement.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator operator--(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Equality operator to compare two iterators.
        ///
        /// Compares two ConstIterator objects for equality based on their
        /// current positions.
        ///
        /// \param rhs The iterator to compare with.
        ///
        /// \return True if both iterators are at the same position, otherwise
        /// false.
        ///
        ///////////////////////////////////////////////////////////////////////
        bool operator==(const ConstIterator& rhs) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Inequality operator to compare two iterators.
        ///
        /// Compares two ConstIterator objects for inequality.
        ///
        /// \param rhs The iterator to compare with.
        ///
        /// \return True if the iterators are at different positions, otherwise
        /// false.
        ///
        ///////////////////////////////////////////////////////////////////////
        bool operator!=(const ConstIterator& rhs) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Addition operator to move the iterator forward by a given
        /// number of positions.
        ///
        /// \param i The number of positions to move forward.
        ///
        /// \return A new iterator advanced by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator operator+(sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subtraction operator to move the iterator backward by a
        /// given number of positions.
        ///
        /// \param i The number of positions to move backward.
        ///
        /// \return A new iterator moved back by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstIterator operator-(sizeType i);

    protected:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Protected member data.
        ///
        ///////////////////////////////////////////////////////////////////////
        StringIteratorType current;   //<! Current iterator state.

        ///////////////////////////////////////////////////////////////////////
        /// \brief Retrieve the character at the current iterator position.
        ///
        /// \return A reference to the character at the iterator's current
        /// position.
        ///
        ///////////////////////////////////////////////////////////////////////
        char& retrieve(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Increment the current position of the iterator.
        ///
        /// Moves the iterator to the next character.
        ///
        ///////////////////////////////////////////////////////////////////////
        void _increment(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Decrement the current position of the iterator.
        ///
        /// Moves the iterator to the previous character.
        ///
        ///////////////////////////////////////////////////////////////////////
        void _decrement(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Access a character at a specific offset from the current
        /// iterator position.
        ///
        /// \param i The offset from the current position.
        ///
        /// \return A reference to the character at the offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        char& _randomAccess(sizeType i) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Add a given number of positions to the iterator.
        ///
        /// \param toReturn Reference to the iterator to modify.
        /// \param i Number of positions to add.
        ///
        ///////////////////////////////////////////////////////////////////////
        void _add(ConstIterator& toReturn, sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subtract a given number of positions from the iterator.
        ///
        /// \param toReturn Reference to the iterator to modify.
        /// \param i Number of positions to subtract.
        ///
        ///////////////////////////////////////////////////////////////////////
        void _subtract(ConstIterator& toReturn, sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief String class is a friend, allowing it access to private
        /// members of ConstIterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        friend class FString;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Iterator class for modifying the String object.
    ///
    /// This class allows iterating over a String object and modifying its
    /// characters. It inherits from the `ConstIterator` class, providing the
    /// same functionality with additional write access to the elements.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class Iterator : public ConstIterator
    {
        ///////////////////////////////////////////////////////////////////////
        /// \brief Alias for the base class (ConstIterator).
        ///
        ///////////////////////////////////////////////////////////////////////
        using _myBase = ConstIterator;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Grants access to private members of the Iterator to the
        /// String class.
        ///
        ///////////////////////////////////////////////////////////////////////
        friend class FString;

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor for Iterator.
        ///
        /// Constructs an Iterator object with no valid position.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructs an Iterator from a StringIteratorType.
        ///
        /// \param p The StringIteratorType representing the starting position
        /// of the iterator in the String.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator(StringIteratorType p);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator to access the character at the current
        /// iterator position with write access.
        ///
        /// Allows modifying the character pointed by the iterator.
        ///
        /// \return A reference to the character at the current iterator
        /// position.
        ///
        ///////////////////////////////////////////////////////////////////////
        char& operator*(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator to access the character at the current
        /// iterator position with read-only access.
        ///
        /// Provides constant access to the character at the iterator's
        /// position.
        ///
        /// \return A constant reference to the character.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator*(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subscript operator to access a character at an offset from
        /// the current iterator position with write access.
        ///
        /// Allows modifying the character at the specified offset.
        ///
        /// \param index Offset from the current iterator position.
        ///
        /// \return A reference to the character at the specified offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        char& operator[](int index);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subscript operator to access a character at an offset from
        /// the current iterator position with read-only access.
        ///
        /// Provides constant access to the character at the specified offset.
        ///
        /// \param index Offset from the current iterator position.
        ///
        /// \return A constant reference to the character at the specified
        /// offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator[](int index) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-increment operator to move the iterator to the next
        /// position.
        ///
        /// Advances the iterator to the next character in the String and
        /// allows write access.
        ///
        /// \return A reference to the incremented iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator& operator++(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-increment operator to move the iterator to the next
        /// position.
        ///
        /// Advances the iterator to the next character, but returns a copy of
        /// the iterator before the increment. Provides write access.
        ///
        /// \return A copy of the iterator before the increment.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator operator++(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-decrement operator to move the iterator to the previous
        /// position.
        ///
        /// Moves the iterator to the previous character in the String and
        /// allows write access.
        ///
        /// \return A reference to the decremented iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator& operator--(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-decrement operator to move the iterator to the previous
        /// position.
        ///
        /// Moves the iterator to the previous character but returns a copy of
        /// the iterator before the decrement. Provides write access.
        ///
        /// \return A copy of the iterator before the decrement.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator operator--(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Addition operator to move the iterator forward by a given
        /// number of positions.
        ///
        /// Advances the iterator by the specified number of positions,
        /// allowing write access to the characters.
        ///
        /// \param i The number of positions to move forward.
        ///
        /// \return A new iterator advanced by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator operator+(sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subtraction operator to move the iterator backward by a
        /// given number of positions.
        ///
        /// Moves the iterator backward by the specified number of positions,
        /// allowing write access to the characters.
        ///
        /// \param i The number of positions to move backward.
        ///
        /// \return A new iterator moved back by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        Iterator operator-(sizeType i);
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Typedefs for convenience.
    ///
    ///////////////////////////////////////////////////////////////////////////
    using ConstPointer = ConstIterator;
    using Pointer = Iterator;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief ConstReverseIterator class for reverse traversal of a String
    /// object.
    ///
    /// This class provides functionality to iterate over a String object in
    /// reverse order. It extends the `ConstIterator` class and offers
    /// read-only access to the characters during reverse traversal.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class ConstReverseIterator : public ConstIterator
    {
        ///////////////////////////////////////////////////////////////////////
        /// \brief Alias for the base class (ConstIterator).
        ///
        ///////////////////////////////////////////////////////////////////////
        using _myBase = ConstIterator;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Grants access to private members of the ConstReverseIterator
        /// to the String class.
        ///
        ///////////////////////////////////////////////////////////////////////
        friend class FString;

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor for ConstReverseIterator.
        ///
        /// Constructs a reverse iterator with no valid position.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructs a reverse iterator from a StringIteratorType.
        ///
        /// \param p The StringIteratorType representing the starting position
        /// of the reverse iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator(StringIteratorType p);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator to access the character at the current
        /// reverse iterator position with read-only access.
        ///
        /// Provides constant access to the character at the iterator's
        /// position during reverse traversal.
        ///
        /// \return A constant reference to the character.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator*(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subscript operator to access a character at an offset from
        /// the current reverse iterator position with read-only access.
        ///
        /// Provides constant access to the character at the specified offset
        /// during reverse traversal.
        ///
        /// \param index Offset from the current iterator position.
        ///
        /// \return A constant reference to the character at the specified
        /// offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator[](int index) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-increment operator to move the reverse iterator to the
        /// next position (previous character in the string).
        ///
        /// Advances the iterator to the next character in reverse order.
        ///
        /// \return A reference to the incremented reverse iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator& operator++(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-increment operator to move the reverse iterator to the
        /// next position (previous character in the string).
        ///
        /// Advances the reverse iterator to the next character in reverse
        /// order, but returns a copy of the iterator before the increment.
        ///
        /// \return A copy of the reverse iterator before the increment.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator operator++(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-decrement operator to move the reverse iterator to the
        /// previous position (next character in the string).
        ///
        /// Moves the reverse iterator to the previous character in reverse
        /// order.
        ///
        /// \return A reference to the decremented reverse iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator& operator--(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-decrement operator to move the reverse iterator to the
        /// previous position (next character in the string).
        ///
        /// Moves the reverse iterator to the previous character in reverse
        /// order, but returns a copy of the iterator before the decrement.
        ///
        /// \return A copy of the reverse iterator before the decrement.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator operator--(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Addition operator to move the reverse iterator forward by a
        /// given number of positions (backward in the string).
        ///
        /// Advances the reverse iterator by the specified number of positions,
        /// allowing read-only access to the characters.
        ///
        /// \param i The number of positions to move forward in reverse order.
        ///
        /// \return A new reverse iterator advanced by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator operator+(sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subtraction operator to move the reverse iterator backward
        /// by a given number of positions (forward in the string).
        ///
        /// Moves the reverse iterator backward by the specified number of
        /// positions, allowing read-only access to the characters.
        ///
        /// \param i The number of positions to move backward in reverse order.
        ///
        /// \return A new reverse iterator moved back by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        ConstReverseIterator operator-(sizeType i);

    protected:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Increments the current position of the reverse iterator.
        ///
        /// Moves the iterator one step forward (backward in the string).
        ///
        ///////////////////////////////////////////////////////////////////////
        void _increment(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Decrements the current position of the reverse iterator.
        ///
        /// Moves the iterator one step backward (forward in the string).
        ///
        ///////////////////////////////////////////////////////////////////////
        void _decrement(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Adds a specified number of positions to a reverse iterator.
        ///
        /// Modifies the provided iterator by advancing it by `i` positions.
        ///
        /// \param toReturn The iterator to be modified.
        /// \param i The number of positions to add.
        ///
        ///////////////////////////////////////////////////////////////////////
        void _add(ConstReverseIterator& toReturn, sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subtracts a specified number of positions from a reverse
        /// iterator.
        ///
        /// Modifies the provided iterator by moving it back by `i` positions.
        ///
        /// \param toReturn The iterator to be modified.
        /// \param i The number of positions to subtract.
        ///
        ///////////////////////////////////////////////////////////////////////
        void _subtract(ConstReverseIterator& toReturn, sizeType i);
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief ReverseIterator class for modifying a String object in reverse
    /// order.
    ///
    /// This class provides functionality to iterate over a String object in
    /// reverse order with write access to the characters. It extends the
    /// `ConstReverseIterator` class and offers read-write access to the
    /// characters during reverse traversal.
    ///
    ///////////////////////////////////////////////////////////////////////////
    class ReverseIterator : public ConstReverseIterator
    {
        ///////////////////////////////////////////////////////////////////////
        /// \brief Alias for the base class (ConstReverseIterator).
        ///
        ///////////////////////////////////////////////////////////////////////
        using _myBase = ConstReverseIterator;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Grants access to private members of the ReverseIterator to
        /// the String class.
        ///
        ///////////////////////////////////////////////////////////////////////
        friend class FString;

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor for ReverseIterator.
        ///
        /// Constructs a reverse iterator with no valid position.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructs a ReverseIterator from a StringIteratorType.
        ///
        /// \param p The StringIteratorType representing the starting position
        /// of the reverse iterator in the String.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator(StringIteratorType p);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator to access the character at the current
        /// reverse iterator position with write access.
        ///
        /// Allows modifying the character pointed by the iterator during
        /// reverse traversal.
        ///
        /// \return A reference to the character at the current iterator
        /// position.
        ///
        ///////////////////////////////////////////////////////////////////////
        char& operator*(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Dereference operator to access the character at the current
        /// reverse iterator position with read-only access.
        ///
        /// Provides constant access to the character at the iterator's
        /// position during reverse traversal.
        ///
        /// \return A constant reference to the character.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator*(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subscript operator to access a character at an offset from
        /// the current reverse iterator position with write access.
        ///
        /// Allows modifying the character at the specified offset during
        /// reverse traversal.
        ///
        /// \param index Offset from the current iterator position.
        ///
        /// \return A reference to the character at the specified offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        char& operator[](int index);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subscript operator to access a character at an offset from
        /// the current reverse iterator position with read-only access.
        ///
        /// Provides constant access to the character at the specified offset
        /// during reverse traversal.
        ///
        /// \param index Offset from the current iterator position.
        ///
        /// \return A constant reference to the character at the specified
        /// offset.
        ///
        ///////////////////////////////////////////////////////////////////////
        const char& operator[](int index) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-increment operator to move the reverse iterator to the
        /// next position (previous character in the string).
        ///
        /// Advances the iterator to the next character in reverse order and
        /// allows write access.
        ///
        /// \return A reference to the incremented reverse iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator& operator++(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-increment operator to move the reverse iterator to the
        /// next position (previous character in the string).
        ///
        /// Advances the reverse iterator to the next character in reverse
        /// order, but returns a copy of the iterator before the increment.
        /// Provides write access.
        ///
        /// \return A copy of the reverse iterator before the increment.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator operator++(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Pre-decrement operator to move the reverse iterator to the
        /// previous position (next character in the string).
        ///
        /// Moves the reverse iterator to the previous character in reverse
        /// order and allows write access.
        ///
        /// \return A reference to the decremented reverse iterator.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator& operator--(void);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Post-decrement operator to move the reverse iterator to the
        /// previous position (next character in the string).
        ///
        /// Moves the reverse iterator to the previous character in reverse
        /// order, but returns a copy of the iterator before the decrement.
        /// Provides write access.
        ///
        /// \return A copy of the reverse iterator before the decrement.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator operator--(int);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Addition operator to move the reverse iterator forward by a
        /// given number of positions (backward in the string).
        ///
        /// Advances the reverse iterator by the specified number of positions,
        /// allowing write access to the characters.
        ///
        /// \param i The number of positions to move forward in reverse order.
        ///
        /// \return A new reverse iterator advanced by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator operator+(sizeType i);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Subtraction operator to move the reverse iterator backward
        /// by a given number of positions (forward in the string).
        ///
        /// Moves the reverse iterator backward by the specified number of
        /// positions, allowing write access to the characters.
        ///
        /// \param i The number of positions to move backward in reverse order.
        ///
        /// \return A new reverse iterator moved back by `i` positions.
        ///
        ///////////////////////////////////////////////////////////////////////
        ReverseIterator operator-(sizeType i);
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Typedefs for reverse iterator convenience.
    ///
    ///////////////////////////////////////////////////////////////////////////
    using ConstReversePointer = ConstReverseIterator;
    using ReversePointer = ReverseIterator;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    ///
    /// Constructs an empty string with no characters.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor.
    ///
    /// Constructs a string as a copy of another string.
    ///
    /// \param other The string to copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(const FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Substring constructor.
    ///
    /// Constructs a string as a copy of a substring of another string.
    ///
    /// \param other The string to copy from.
    /// \param pos The position of the first character to copy.
    /// \param len The number of characters to copy (default: NPOS for all).
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(const FString& other, sizeType pos, SizeT len = NPOS);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor.
    ///
    /// Constructs a string by moving the contents of another string.
    ///
    /// \param other The string to move from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(FString&& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief C-string constructor.
    ///
    /// Constructs a string from a null-terminated C-string.
    ///
    /// \param other The C-string to copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(const char* other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief C-string with length constructor.
    ///
    /// Constructs a string from a C-string with a specified length.
    ///
    /// \param other The C-string to copy.
    /// \param len The number of characters to copy.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(const char* other, SizeT len);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Fill constructor.
    ///
    /// Constructs a string with a specified number of identical characters.
    ///
    /// \param len The number of characters to create.
    /// \param filler The character to fill the string with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(SizeT len, char filler);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Iterator range constructor.
    ///
    /// Constructs a string from a range of characters defined by iterators.
    ///
    /// \param first The beginning of the range.
    /// \param second The end of the range.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString(const ConstIterator first, const ConstIterator second);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the beginning of the string.
    ///
    /// \return An iterator pointing to the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Begin(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the string.
    ///
    /// \return A const iterator pointing to the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator Begin(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns an iterator to the end of the string.
    ///
    /// \return An iterator pointing past the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator End(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the string.
    ///
    /// \return A const iterator pointing past the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator End(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the beginning of the string.
    ///
    /// \return A const iterator pointing to the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator CBegin(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const iterator to the end of the string.
    ///
    /// \return A const iterator pointing past the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstIterator CEnd(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reverse iterator to the beginning of the string.
    ///
    /// \return A reverse iterator pointing to the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ReverseIterator RBegin(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the beginning of the string.
    ///
    /// \return A const reverse iterator pointing to the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator RBegin(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reverse iterator to the end of the string.
    ///
    /// \return A reverse iterator pointing before the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ReverseIterator REnd(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the end of the string.
    ///
    /// \return A const reverse iterator pointing before the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator REnd(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the beginning of the string.
    ///
    /// \return A const reverse iterator pointing to the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator CRBegin(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reverse iterator to the end of the string.
    ///
    /// \return A const reverse iterator pointing before the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ConstReverseIterator CREnd(void) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the internal iterator type for the beginning.
    ///
    /// \return The internal iterator type pointing to the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    StringIteratorType _iBegin(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the internal iterator type for the reverse beginning.
    ///
    /// \return The internal iterator type pointing to the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    StringIteratorType _rBegin(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the internal iterator type for the end.
    ///
    /// \return The internal iterator type pointing past the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    StringIteratorType _end(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Calculates the length between two iterators.
    ///
    /// \param first The starting iterator.
    /// \param second The ending iterator.
    ///
    /// \return The number of characters between the iterators.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT _getLength(const ConstIterator first, const ConstIterator second);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy assignment operator.
    ///
    /// Assigns the contents of another string to this string.
    ///
    /// \param other The string to copy.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator=(const FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator.
    ///
    /// Moves the contents of another string to this string.
    ///
    /// \param other The string to move from.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator=(FString&& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator from std::string.
    ///
    /// \param other The std::string to copy from.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator=(const std::string& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator from std::string.
    ///
    /// \param other The std::string to move from.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator=(std::string&& other);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///
    /// Destroys the string and deallocates any memory.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~FString(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Equality operator.
    ///
    /// Compares two strings for equality.
    ///
    /// \param lhs The left-hand side string.
    /// \param rhs The right-hand side string.
    ///
    /// \return True if the strings are equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    friend bool operator==(const FString& lhs, const FString& rhs);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inequality operator.
    ///
    /// Compares two strings for inequality.
    ///
    /// \param lhs The left-hand side string.
    /// \param rhs The right-hand side string.
    ///
    /// \return True if the strings are not equal, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    friend bool operator!=(const FString& lhs, const FString& rhs);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Less than operator.
    ///
    /// Compares if the left string is lexicographically less than the right.
    ///
    /// \param lhs The left-hand side string.
    /// \param rhs The right-hand side string.
    ///
    /// \return True if lhs < rhs, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    friend bool operator<(const FString& lhs, const FString& rhs);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Greater than operator.
    ///
    /// Compares if the left string is lexicographically greater than the
    /// right.
    ///
    /// \param lhs The left-hand side string.
    /// \param rhs The right-hand side string.
    ///
    /// \return True if lhs > rhs, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    friend bool operator>(const FString& lhs, const FString& rhs);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Less than or equal operator.
    ///
    /// Compares if the left string is lexicographically less than or equal to
    /// the right.
    ///
    /// \param lhs The left-hand side string.
    /// \param rhs The right-hand side string.
    ///
    /// \return True if lhs <= rhs, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    friend bool operator<=(const FString& lhs, const FString& rhs);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Greater than or equal operator.
    ///
    /// Compares if the left string is lexicographically greater than or equal
    /// to the right.
    ///
    /// \param lhs The left-hand side string.
    /// \param rhs The right-hand side string.
    ///
    /// \return True if lhs >= rhs, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    friend bool operator>=(const FString& lhs, const FString& rhs);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compares this string with another string.
    ///
    /// \param rhs The string to compare with.
    ///
    /// \return A negative value if this < rhs, 0 if equal, positive if this >
    /// rhs.
    ///
    ///////////////////////////////////////////////////////////////////////////
    int _compare(const FString& rhs) const;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Const subscript operator.
    ///
    /// Accesses a character at the specified index with read-only access.
    ///
    /// \param index The index of the character to access.
    ///
    /// \return A const reference to the character at the index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const char& operator[](sizeType index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Subscript operator.
    ///
    /// Accesses a character at the specified index with write access.
    ///
    /// \param index The index of the character to access.
    ///
    /// \return A reference to the character at the index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    char& operator[](sizeType index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses a character at the specified index with bounds checking
    /// (const).
    ///
    /// \param index The index of the character to access.
    ///
    /// \return A const reference to the character at the index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const char& At(sizeType index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accesses a character at the specified index with bounds
    /// checking.
    ///
    /// \param index The index of the character to access.
    ///
    /// \return A reference to the character at the index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    char& At(sizeType index);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reference to the first character.
    ///
    /// \return A const reference to the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const char& Front(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reference to the first character.
    ///
    /// \return A reference to the first character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    char& Front(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a const reference to the last character.
    ///
    /// \return A const reference to the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const char& Back(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a reference to the last character.
    ///
    /// \return A reference to the last character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    char& Back(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Append operator for FString.
    ///
    /// Appends another string to this string.
    ///
    /// \param other The string to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator+=(const FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Append operator for C-string.
    ///
    /// Appends a C-string to this string.
    ///
    /// \param other The C-string to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator+=(const char* other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Append operator for character.
    ///
    /// Appends a character to this string.
    ///
    /// \param ch The character to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& operator+=(char ch);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends another string.
    ///
    /// \param str The string to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Append(const FString& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends a substring of another string.
    ///
    /// \param str The string to append from.
    /// \param subPos The starting position in the source string.
    /// \param subLen The number of characters to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Append(const FString& str, sizeType subPos, SizeT subLen = NPOS);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends a C-string.
    ///
    /// \param str The C-string to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Append(const char* str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends a C-string with length.
    ///
    /// \param str The C-string to append.
    /// \param len The number of characters to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Append(const char* str, SizeT len);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends multiple identical characters.
    ///
    /// \param len The number of characters to append.
    /// \param filler The character to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Append(sizeType len, char filler);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends characters from an iterator range.
    ///
    /// \param first The beginning of the range.
    /// \param second The end of the range.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Append(const ConstIterator first, const ConstIterator second);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends a character to the end of the string.
    ///
    /// \param ch The character to append.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& PushBack(char ch);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts another string at the specified position.
    ///
    /// \param pos The position to insert at.
    /// \param other The string to insert.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Insert(sizeType pos, const FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts a substring at the specified position.
    ///
    /// \param pos The position to insert at.
    /// \param other The string to insert from.
    /// \param subPos The starting position in the source string.
    /// \param subLen The number of characters to insert.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Insert(
        sizeType pos, const FString& other, sizeType subPos, SizeT subLen
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts a C-string at the specified position.
    ///
    /// \param pos The position to insert at.
    /// \param other The C-string to insert.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Insert(sizeType pos, const char* other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts a C-string with length at the specified position.
    ///
    /// \param pos The position to insert at.
    /// \param other The C-string to insert.
    /// \param len The number of characters to insert.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Insert(sizeType pos, const char* other, SizeT len);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts multiple identical characters at the specified position.
    ///
    /// \param pos The position to insert at.
    /// \param len The number of characters to insert.
    /// \param filler The character to insert.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Insert(sizeType pos, SizeT len, char filler);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts multiple characters at the iterator position.
    ///
    /// \param ptr The position to insert at.
    /// \param len The number of characters to insert.
    /// \param ch The character to insert.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Insert(Iterator ptr, SizeT len, char ch);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts a character at the iterator position.
    ///
    /// \param ptr The position to insert at.
    /// \param ch The character to insert.
    ///
    /// \return An iterator pointing to the inserted character.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Insert(Iterator ptr, char ch);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inserts characters from an iterator range at the iterator
    /// position.
    ///
    /// \param ptr The position to insert at.
    /// \param first The beginning of the range to insert.
    /// \param last The end of the range to insert.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Insert(
        Iterator ptr, const ConstIterator first, const ConstIterator last
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Erases characters from the string.
    ///
    /// \param pos The starting position to erase from.
    /// \param len The number of characters to erase.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Erase(sizeType pos = 0, SizeT len = NPOS);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Erases a character at the iterator position.
    ///
    /// \param ptr The position of the character to erase.
    ///
    /// \return An iterator pointing to the character after the erased one.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Erase(ConstIterator ptr);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Erases characters in the iterator range.
    ///
    /// \param first The beginning of the range to erase.
    /// \param last The end of the range to erase.
    ///
    /// \return An iterator pointing to the character after the erased range.
    ///
    ///////////////////////////////////////////////////////////////////////////
    Iterator Erase(ConstIterator first, ConstIterator last);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters in the string with another string.
    ///
    /// \param pos The starting position to replace.
    /// \param len The number of characters to replace.
    /// \param other The string to replace with.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(sizeType pos, SizeT len, const FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters in the iterator range with another string.
    ///
    /// \param it1 The beginning of the range to replace.
    /// \param it2 The end of the range to replace.
    /// \param other The string to replace with.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString&
        Replace(ConstIterator it1, ConstIterator it2, const FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters with a substring of another string.
    ///
    /// \param pos The starting position to replace.
    /// \param len The number of characters to replace.
    /// \param other The string to replace with.
    /// \param subPos The starting position in the replacement string.
    /// \param subLen The number of characters to use from the replacement
    /// string.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(
        sizeType pos,
        SizeT len,
        const FString& other,
        sizeType subPos,
        SizeT subLen = NPOS
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters with a C-string.
    ///
    /// \param pos The starting position to replace.
    /// \param len The number of characters to replace.
    /// \param str The C-string to replace with.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(sizeType pos, SizeT len, const char* str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters in iterator range with a C-string.
    ///
    /// \param it1 The beginning of the range to replace.
    /// \param it2 The end of the range to replace.
    /// \param other The C-string to replace with.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(ConstIterator it1, ConstIterator it2, const char* other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters with a C-string of specified length.
    ///
    /// \param pos The starting position to replace.
    /// \param len The number of characters to replace.
    /// \param other The C-string to replace with.
    /// \param n The number of characters to use from the C-string.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(sizeType pos, SizeT len, const char* other, SizeT n);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters in iterator range with C-string of specified
    /// length.
    ///
    /// \param it1 The beginning of the range to replace.
    /// \param it2 The end of the range to replace.
    /// \param other The C-string to replace with.
    /// \param n The number of characters to use from the C-string.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(
        ConstIterator it1, ConstIterator it2, const char* other, SizeT n
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters with multiple identical characters.
    ///
    /// \param pos The starting position to replace.
    /// \param len The number of characters to replace.
    /// \param n The number of replacement characters.
    /// \param filler The character to replace with.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(sizeType pos, SizeT len, SizeT n, char filler);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters in iterator range with multiple identical
    /// characters.
    ///
    /// \param it1 The beginning of the range to replace.
    /// \param it2 The end of the range to replace.
    /// \param n The number of replacement characters.
    /// \param ch The character to replace with.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString&
        Replace(ConstIterator it1, ConstIterator it2, sizeType n, char ch);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Replaces characters in iterator range with characters from
    /// another range.
    ///
    /// \param it1 The beginning of the range to replace.
    /// \param it2 The end of the range to replace.
    /// \param first The beginning of the replacement range.
    /// \param second The end of the replacement range.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Replace(
        ConstIterator it1,
        ConstIterator it2,
        ConstIterator first,
        ConstIterator second
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swaps the contents of this string with another string.
    ///
    /// \param other The string to swap with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Swap(FString& other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes the last character from the string.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& PopBack(void);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to append a C-string.
    ///
    /// \param other The C-string to append.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _append(const char* other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to append a C-string with length.
    ///
    /// \param other The C-string to append.
    /// \param n The number of characters to append.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _append(const char* other, SizeT n);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to insert a C-string at a position.
    ///
    /// \param pos The position to insert at.
    /// \param other The C-string to insert.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _insertstr(sizeType pos, const char* other);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to insert a C-string with length at a position.
    ///
    /// \param pos The position to insert at.
    /// \param other The C-string to insert.
    /// \param n The number of characters to insert.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _insertstr(sizeType pos, const char* other, SizeT n);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to create a substring buffer.
    ///
    /// \param buffer The buffer to store the substring.
    /// \param other The source string.
    /// \param pos The starting position in the source.
    /// \param len The length of the substring.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _substr(
        char*& buffer, const char* other, sizeType pos = 0, SizeT len = NPOS
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to erase characters from the string.
    ///
    /// \param pos The starting position to erase from.
    /// \param Size The number of characters to erase.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _erase(sizeType pos, SizeT Size);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to replace characters in the string.
    ///
    /// \param pos The starting position to replace.
    /// \param len The number of characters to replace.
    /// \param other The replacement string.
    /// \param n The number of characters to use from the replacement.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _replace(sizeType pos, SizeT len, const char* other, SizeT n);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to get the length of a substring.
    ///
    /// \param str The string to get the length from.
    /// \param pos The starting position.
    /// \param len The requested length.
    ///
    /// \return The actual length of the substring.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT _getLength(const FString& str, sizeType pos, SizeT len = NPOS) const;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a C-string representation of the string.
    ///
    /// \return A const pointer to the null-terminated C-string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const char* CStr(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copies characters from the string to a C-string buffer.
    ///
    /// \param str The destination buffer.
    /// \param len The maximum number of characters to copy.
    /// \param pos The starting position in this string.
    ///
    /// \return The number of characters copied.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT Copy(char* str, SizeT len, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of a string.
    ///
    /// \param other The string to find.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType Find(const FString& other, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of a C-string.
    ///
    /// \param other The C-string to find.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType Find(const char* other, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of a C-string with length.
    ///
    /// \param other The C-string to find.
    /// \param pos The position to start searching from.
    /// \param n The number of characters to search for.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType Find(const char* other, sizeType pos, sizeType n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of a character.
    ///
    /// \param ch The character to find.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType Find(char ch, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of a string.
    ///
    /// \param other The string to find.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType RFind(const FString& other, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of a C-string.
    ///
    /// \param other The C-string to find.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType RFind(const char* other, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of a C-string with length.
    ///
    /// \param other The C-string to find.
    /// \param pos The position to start searching from (backward).
    /// \param n The number of characters to search for.
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType RFind(const char* other, sizeType pos, sizeType n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of a character.
    ///
    /// \param ch The character to find.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType RFind(char ch, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of any character from a string.
    ///
    /// \param other The string containing characters to search for.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstOf(const FString& other, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of any character from a C-string.
    ///
    /// \param other The C-string containing characters to search for.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstOf(const char* other, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of any character from a C-string with
    /// length.
    ///
    /// \param other The C-string containing characters to search for.
    /// \param pos The position to start searching from.
    /// \param n The number of characters in the C-string.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstOf(const char* other, sizeType pos, sizeType n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first occurrence of a character.
    ///
    /// \param ch The character to find.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstOf(char ch, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of any character from a string.
    ///
    /// \param other The string containing characters to search for.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastOf(const FString& other, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of any character from a C-string.
    ///
    /// \param other The C-string containing characters to search for.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastOf(const char* other, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of any character from a C-string with
    /// length.
    ///
    /// \param other The C-string containing characters to search for.
    /// \param pos The position to start searching from (backward).
    /// \param n The number of characters in the C-string.
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastOf(const char* other, sizeType pos, sizeType n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last occurrence of a character.
    ///
    /// \param ch The character to find.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last occurrence, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastOf(char ch, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first character not in the given string.
    ///
    /// \param other The string containing characters to exclude.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first non-matching character, or NPOS if
    /// not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstNotOf(const FString& other, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first character not in the given C-string.
    ///
    /// \param other The C-string containing characters to exclude.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first non-matching character, or NPOS if
    /// not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstNotOf(const char* other, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first character not in the given C-string with length.
    ///
    /// \param other The C-string containing characters to exclude.
    /// \param pos The position to start searching from.
    /// \param n The number of characters in the C-string.
    ///
    /// \return The position of the first non-matching character, or NPOS if
    /// not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstNotOf(const char* other, sizeType pos, sizeType n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the first character not equal to the given character.
    ///
    /// \param ch The character to exclude.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the first non-matching character, or NPOS if
    /// not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindFirstNotOf(char ch, sizeType pos = 0) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last character not in the given string.
    ///
    /// \param other The string containing characters to exclude.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last non-matching character, or NPOS if not
    /// found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastNotOf(const FString& other, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last character not in the given C-string.
    ///
    /// \param other The C-string containing characters to exclude.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last non-matching character, or NPOS if not
    /// found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastNotOf(const char* other, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last character not in the given C-string with length.
    ///
    /// \param other The C-string containing characters to exclude.
    /// \param pos The position to start searching from (backward).
    /// \param n The number of characters in the C-string.
    ///
    /// \return The position of the last non-matching character, or NPOS if not
    /// found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastNotOf(const char* other, sizeType pos, sizeType n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Finds the last character not equal to the given character.
    ///
    /// \param ch The character to exclude.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the last non-matching character, or NPOS if not
    /// found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType FindLastNotOf(char ch, sizeType pos = NPOS) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns a substring of this string.
    ///
    /// \param pos The starting position of the substring.
    /// \param len The length of the substring.
    ///
    /// \return A new string containing the substring.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString SubStr(sizeType pos = 0, SizeT len = NPOS) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to find a substring.
    ///
    /// \param other The substring to find.
    /// \param len The length of the substring.
    /// \param pos The position to start searching from.
    ///
    /// \return The position of the substring, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType _find(const char* other, SizeT len, sizeType pos) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to find a substring in reverse.
    ///
    /// \param other The substring to find.
    /// \param len The length of the substring.
    /// \param pos The position to start searching from (backward).
    ///
    /// \return The position of the substring, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType _rfind(const char* other, SizeT len, sizeType pos) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to compare for substring finding.
    ///
    /// \param other The substring to compare.
    /// \param len The length of the substring.
    /// \param pos The position to compare at.
    ///
    /// \return True if the substring matches, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool _findCompare(const char* other, SizeT len, sizeType pos) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to find the first occurrence of any character.
    ///
    /// \param other The characters to search for.
    /// \param len The number of characters.
    /// \param pos The position to start searching from.
    /// \param IsTrue Whether to find matching or non-matching characters.
    ///
    /// \return The position of the found character, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType _findFirstOf(
        const char* other, SizeT len, sizeType pos, bool IsTrue
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to find the last occurrence of any character.
    ///
    /// \param other The characters to search for.
    /// \param len The number of characters.
    /// \param pos The position to start searching from (backward).
    /// \param IsTrue Whether to find matching or non-matching characters.
    ///
    /// \return The position of the found character, or NPOS if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    sizeType _findLastOf(
        const char* other, SizeT len, sizeType pos, bool IsTrue
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Internal method to compare for character finding.
    ///
    /// \param other The characters to compare against.
    /// \param len The number of characters.
    /// \param pos The position to compare at.
    /// \param IsTrue Whether to check for matching or non-matching.
    ///
    /// \return True if the condition is met, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool _findOfCompare(
        const char* other, SizeT len, sizeType pos, bool IsTrue
    ) const;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the length of the string.
    ///
    /// \return The number of characters in the string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT Length(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the size of the string (same as Length).
    ///
    /// \return The number of characters in the string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT Size(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the maximum possible size of the string.
    ///
    /// \return The maximum number of characters the string can hold.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT MaxSize(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Resizes the string to the specified length.
    ///
    /// \param n The new length of the string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Resize(SizeT n);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Resizes the string to the specified length with a filler
    /// character.
    ///
    /// \param n The new length of the string.
    /// \param filler The character to fill any new positions.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Resize(sizeType n, char filler);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the current capacity of the string.
    ///
    /// \return The number of characters the string can hold without
    /// reallocation.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT Capacity(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reserves memory for at least the specified number of characters.
    ///
    /// \param n The minimum capacity to reserve.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reserve(SizeT n = 0);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clears the contents of the string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Clear(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reduces the capacity to fit the current size.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ShrinkToFit(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Converts all characters to lowercase.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& ToLowerCase(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Converts all characters to uppercase.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& ToUpperCase(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes leading and trailing whitespace.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString& Trim(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the string is empty.
    ///
    /// \return True if the string has no characters, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsEmpty(void) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the length of the string.
    ///
    /// \param len The new length.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _setLength(const SizeT len);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the capacity of the string.
    ///
    /// \param Cap The new capacity.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _setCapacity(const SizeT Cap);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Increases the capacity of the string.
    ///
    /// \param Cap The amount to increase the capacity by.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _increaseCapacity(const SizeT Cap);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Decreases the capacity of the string.
    ///
    /// \param Cap The amount to decrease the capacity by.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _decreaseCapacity(const SizeT Cap);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Allocates a C-string buffer.
    ///
    /// \param buffer The buffer to allocate.
    /// \param n The size of the buffer.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _allocCString(char*& buffer, const SizeT n) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Allocates a C-string buffer with a filler character.
    ///
    /// \param buffer The buffer to allocate.
    /// \param n The size of the buffer.
    /// \param filler The character to fill the buffer with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _allocCString(char*& buffer, const SizeT n, char filler) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Allocates a C-string buffer from an iterator range.
    ///
    /// \param buffer The buffer to allocate.
    /// \param n The size of the buffer.
    /// \param it1 The beginning of the range.
    /// \param it2 The end of the range.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _allocCString(
        char*& buffer,
        const SizeT n,
        const ConstIterator it1,
        const ConstIterator it2
    ) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Fills a string buffer with a character.
    ///
    /// \param other The buffer to fill.
    /// \param len The length of the buffer.
    /// \param pos The starting position.
    /// \param ch The character to fill with.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _fillStr(char* other, const SizeT len, sizeType pos, char ch) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clears a portion of a string buffer.
    ///
    /// \param pos The position to start clearing from.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void _clearStr(const sizeType pos);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Applies a transformation function to each character.
    ///
    /// \param Func The function to apply to each character.
    ///
    /// \return A reference to this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename F>
    FString& _transform(F Func);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Swaps the contents of two strings.
    ///
    /// \param A The first string.
    /// \param B The second string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Swap(FString& A, FString& B);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Conversion operator to std::string.
    ///
    /// \return An std::string equivalent of this string.
    ///
    ///////////////////////////////////////////////////////////////////////////
    operator std::string(void) const;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation operator for two FString objects.
///
/// \param lhs The left-hand side string.
/// \param rhs The right-hand side string.
///
/// \return A new string containing the concatenation.
///
///////////////////////////////////////////////////////////////////////////////
FString operator+(const FString& lhs, const FString& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation operator for FString and C-string.
///
/// \param lhs The left-hand side string.
/// \param rhs The right-hand side C-string.
///
/// \return A new string containing the concatenation.
///
///////////////////////////////////////////////////////////////////////////////
FString operator+(const FString& lhs, const char* rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation operator for C-string and FString.
///
/// \param lhs The left-hand side C-string.
/// \param rhs The right-hand side string.
///
/// \return A new string containing the concatenation.
///
///////////////////////////////////////////////////////////////////////////////
FString operator+(const char* lhs, const FString& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation operator for FString and character.
///
/// \param lhs The left-hand side string.
/// \param rhs The right-hand side character.
///
/// \return A new string containing the concatenation.
///
///////////////////////////////////////////////////////////////////////////////
FString operator+(const FString& lhs, char rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation operator for character and FString.
///
/// \param lhs The left-hand side character.
/// \param rhs The right-hand side string.
///
/// \return A new string containing the concatenation.
///
///////////////////////////////////////////////////////////////////////////////
FString operator+(char lhs, const FString& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Output stream operator for FString.
///
/// \param os The output stream.
/// \param str The string to output.
///
/// \return A reference to the output stream.
///
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const FString& str);

///////////////////////////////////////////////////////////////////////////////
/// \brief Input stream operator for FString.
///
/// \param is The input stream.
/// \param str The string to read into.
///
/// \return A reference to the input stream.
///
///////////////////////////////////////////////////////////////////////////////
std::ostream& operator>>(std::ostream& is, FString& str);

///////////////////////////////////////////////////////////////////////////////
/// \brief Gets a line from an input stream into an FString.
///
/// \param is The input stream.
/// \param str The string to read into.
/// \param delim The delimiter character.
///
/// \return A reference to the input stream.
///
///////////////////////////////////////////////////////////////////////////////
std::istream& getline(std::istream is, FString& str, char delim = '\n');

}   // namespace tkd
