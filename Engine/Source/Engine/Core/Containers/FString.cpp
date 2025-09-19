///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Containers/FString.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
using sizeType = SizeT;

///////////////////////////////////////////////////////////////////////////////
const SizeT FString::NPOS = -1;

///////////////////////////////////////////////////////////////////////////////
FString::FString(void)
{
    m_data = nullptr;
    m_length = 0;
    m_capacity = 0;
    m_increment = 15;
    _setCapacity(1);   // Allocate at least 1 byte for null terminator
    _setLength(0);
}

///////////////////////////////////////////////////////////////////////////////
FString::FString(const FString& other)
{
    _setCapacity(0);
    _setLength(0);
    _append(other.m_data, other.m_length);
}

///////////////////////////////////////////////////////////////////////////////
FString::FString(const FString& other, sizeType pos, SizeT len)
{
    _setCapacity(0);
    _setLength(0);
    if (other.m_data && pos < other.m_length)
    {
        if (len == NPOS) { len = other.m_length - pos; }
        if (pos + len > other.m_length) { len = other.m_length - pos; }
        _append(other.m_data + pos, len);
    }
}

///////////////////////////////////////////////////////////////////////////////
FString::FString(FString&& other) { *this = std::move(other); }

///////////////////////////////////////////////////////////////////////////////
FString::FString(const char* other)
{
    _setCapacity(0);
    _setLength(0);
    _append(other);
}

///////////////////////////////////////////////////////////////////////////////
FString::FString(const char* other, SizeT len)
{
    _setCapacity(0);
    _setLength(0);
    if (other) { _append(other, len); }
}

///////////////////////////////////////////////////////////////////////////////
FString::FString(SizeT len, char filler)
{
    _setCapacity(0);
    _setLength(0);
    Append(len, filler);
}

///////////////////////////////////////////////////////////////////////////////
FString::FString(const ConstIterator first, const ConstIterator second)
{
    _setCapacity(0);
    _setLength(0);
    Append(first, second);
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::operator=(const FString& other)
{
    if (this != &other)
    {
        _setLength(0);
        _append(other.m_data, other.m_length);
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::operator=(FString&& other)
{
    if (this != &other)
    {
        SafeDeleteArray(m_data);
        m_data = other.m_data;
        m_length = other.m_length;
        m_capacity = other.m_capacity;
        other.m_data = nullptr;
        other.m_length = 0;
        other.m_capacity = 0;
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::~FString(void)
{
    SafeDeleteArray(m_data);
    m_length = 0;
    m_capacity = 0;
    m_increment = 0;
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FString& lhs, const FString& rhs)
{
    return lhs._compare(rhs) == 0;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FString& lhs, const FString& rhs)
{
    return lhs._compare(rhs) != 0;
}

///////////////////////////////////////////////////////////////////////////////
bool operator<(const FString& lhs, const FString& rhs)
{
    return lhs._compare(rhs) < 0;
}

///////////////////////////////////////////////////////////////////////////////
bool operator>(const FString& lhs, const FString& rhs)
{
    return lhs._compare(rhs) > 0;
}

///////////////////////////////////////////////////////////////////////////////
bool operator<=(const FString& lhs, const FString& rhs)
{
    return lhs._compare(rhs) <= 0;
}

///////////////////////////////////////////////////////////////////////////////
bool operator>=(const FString& lhs, const FString& rhs)
{
    return lhs._compare(rhs) >= 0;
}

///////////////////////////////////////////////////////////////////////////////
int FString::_compare(const FString& rhs) const
{
    if (!m_data && !rhs.m_data) { return 0; }
    if (!m_data) { return -1; }
    if (!rhs.m_data) { return 1; }
    return ::strcmp(m_data, rhs.m_data);
}

///////////////////////////////////////////////////////////////////////////////
const char& FString::operator[](sizeType index) const
{
    return *(m_data + index);
}

///////////////////////////////////////////////////////////////////////////////
char& FString::operator[](sizeType index) { return *(m_data + index); }

///////////////////////////////////////////////////////////////////////////////
const char& FString::At(sizeType index) const
{
    if (m_length <= index) { throw; }
    return operator[](index);
}

///////////////////////////////////////////////////////////////////////////////
char& FString::At(sizeType index)
{
    if (m_length <= index) { throw; }
    return operator[](index);
}

///////////////////////////////////////////////////////////////////////////////
const char& FString::Front(void) const { return operator[](0); }

///////////////////////////////////////////////////////////////////////////////
char& FString::Front(void) { return operator[](0); }

///////////////////////////////////////////////////////////////////////////////
const char& FString::Back(void) const { return operator[](m_length - 1); }

///////////////////////////////////////////////////////////////////////////////
char& FString::Back(void) { return operator[](m_length - 1); }

///////////////////////////////////////////////////////////////////////////////
FString& FString::operator+=(const FString& other)
{
    _append(other.m_data, other.m_length);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::operator+=(const char* other)
{
    _append(other);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::operator+=(char ch)
{
    _append(&ch, 1);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Append(const FString& str)
{
    _append(str.m_data, str.m_length);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Append(const FString& str, sizeType subPos, SizeT subLen)
{
    char* buffer = nullptr;

    subLen = _getLength(str, subPos, subLen);
    _substr(buffer, str.m_data, subPos, subLen);
    _append(buffer, subLen);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Append(const char* str)
{
    _append(str);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Append(const char* str, SizeT len)
{
    char* buffer = nullptr;

    _substr(buffer, str, 0, len);
    _append(buffer, len);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Append(sizeType len, char filler)
{
    char* buffer = nullptr;

    _allocCString(buffer, len, filler);
    _append(buffer, len);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Append(const ConstIterator first, const ConstIterator second)
{
    const SizeT len = _getLength(first, second);

    if (len != 0)
    {
        char* buffer = nullptr;
        _allocCString(buffer, len, first, second);
        _append(buffer, len);
        SafeDelete(buffer);
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::PushBack(char ch)
{
    _append(&ch, 1);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Insert(sizeType pos, const FString& other)
{
    _insertstr(pos, other.m_data, other.m_length);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Insert(
    sizeType pos, const FString& other, sizeType subPos, SizeT subLen
)
{
    char* buffer = nullptr;

    subLen = _getLength(other, subPos, subLen);
    _substr(buffer, other.m_data, subPos, subLen);
    _insertstr(pos, buffer, subLen);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Insert(sizeType pos, const char* other)
{
    _insertstr(pos, other);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Insert(sizeType pos, const char* other, SizeT len)
{
    char* buffer = nullptr;

    _substr(buffer, other, 0, len);
    _insertstr(pos, buffer, len);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Insert(sizeType pos, SizeT len, char filler)
{
    char* buffer = nullptr;

    _allocCString(buffer, len, filler);
    _insertstr(pos, buffer, len);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
void FString::Insert(Iterator ptr, SizeT len, char ch)
{
    char* buffer = nullptr;

    _allocCString(buffer, len, ch);
    _insertstr(ptr.current.pos, buffer, len);
    SafeDelete(buffer);
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Insert(Iterator ptr, char ch)
{
    _insertstr(ptr.current.pos, &ch, 1);
    return ptr;
}

///////////////////////////////////////////////////////////////////////////////
void FString::Insert(
    Iterator ptr, const ConstIterator first, const ConstIterator second
)
{
    const SizeT len = _getLength(first, second);

    if (len == 0) { return; }
    char* buffer = nullptr;
    _allocCString(buffer, len, first, second);
    _insertstr(ptr.current.pos, buffer, len);
    SafeDelete(buffer);
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Erase(sizeType pos, SizeT len)
{
    _erase(pos, len);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Erase(ConstIterator ptr)
{
    _erase(ptr.current.pos, 1);
    return ptr.current;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Erase(ConstIterator first, ConstIterator Last)
{
    _erase(first.current.pos, _getLength(first, Last));
    return first.current;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(sizeType pos, SizeT len, const FString& other)
{
    _replace(pos, len, other.m_data, other.m_length);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(
    ConstIterator it1, ConstIterator it2, const FString& other
)
{
    Replace(
        it1.current.pos, _getLength(it1, it2), other.m_data, other.m_length
    );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(
    sizeType pos,
    SizeT len,
    const FString& other,
    sizeType subPos,
    SizeT subLen
)
{
    char* buffer = nullptr;

    subLen = _getLength(other, subPos, subLen);
    _substr(buffer, other.m_data, subPos, subLen);
    _replace(pos, len, buffer, ::strlen(buffer));
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(sizeType pos, SizeT len, const char* str)
{
    _replace(pos, len, str, ::strlen(str));
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString&
    FString::Replace(ConstIterator it1, ConstIterator it2, const char* other)
{
    _replace(it1.current.pos, _getLength(it1, it2), other, ::strlen(other));
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(sizeType pos, SizeT len, const char* other, SizeT n)
{
    _replace(pos, len, other, n);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(
    ConstIterator it1, ConstIterator it2, const char* other, SizeT n
)
{
    _replace(it1.current.pos, _getLength(it1, it2), other, n);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(sizeType pos, SizeT len, SizeT n, char filler)
{
    char* buffer = nullptr;

    _allocCString(buffer, n, filler);
    _replace(pos, len, buffer, n);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString&
    FString::Replace(ConstIterator it1, ConstIterator it2, sizeType n, char ch)
{
    char* buffer = nullptr;

    _allocCString(buffer, n, ch);
    _replace(it1.current.pos, _getLength(it1, it2), buffer, n);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::Replace(
    ConstIterator it1,
    ConstIterator it2,
    ConstIterator first,
    ConstIterator second
)
{
    const SizeT len = _getLength(first, second);
    char* buffer = nullptr;

    _allocCString(buffer, len, first, second);
    _replace(it1.current.pos, _getLength(it1, it2), buffer, len);
    SafeDelete(buffer);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
void FString::Swap(FString& other)
{
    FString temp = std::move(*this);
    *this = std::move(other);
    other = std::move(temp);
}

///////////////////////////////////////////////////////////////////////////////
FString& FString::PopBack(void)
{
    _erase((End() - 1).current.pos, 1);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
void FString::_append(const char* other)
{
    if (!other) { return; }
    return _append(other, ::strlen(other));
}

///////////////////////////////////////////////////////////////////////////////
void FString::_append(const char* other, SizeT len)
{
    if (!other || len == 0) { return; }
    _increaseCapacity(m_length + len);
    sizeType i = 0;
    for (; i < len; ++i) { operator[](m_length + i) = other[i]; }
    operator[](m_length + i) = '\0';
    _setLength(m_length + len);
}

///////////////////////////////////////////////////////////////////////////////
void FString::_insertstr(sizeType pos, const char* other)
{
    if (other != nullptr) { _insertstr(pos, other, ::strlen(other)); }
}

///////////////////////////////////////////////////////////////////////////////
void FString::_insertstr(sizeType pos, const char* other, SizeT len)
{
    if (pos == m_length) { return _append(other, len); }
    if (pos > m_length) { throw; }
    if (len == 0) { return; }
    _increaseCapacity(m_length + len);
    if (m_data)
    {
        char* buffer = nullptr;
        _substr(buffer, m_data, pos, m_length);
        _clearStr(pos);
        _append(other, len);
        _append(buffer, ::strlen(buffer));
        SafeDelete(buffer);
    }
    else { _append(other, len); }
}

///////////////////////////////////////////////////////////////////////////////
void FString::_substr(
    char*& buffer, const char* other, sizeType pos, SizeT len
) const
{
    if (other == nullptr) { throw; }
    _allocCString(buffer, len);
    for (sizeType i = 0; i < len; ++i) { buffer[i] = other[pos + i]; }
    buffer[len] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
void FString::_erase(sizeType pos, SizeT len)
{
    len = _getLength(*this, pos, len);
    for (SizeT i = pos + len; i < m_length; ++i)
    {
        operator[](i - len) = operator[](i);
    }
    _clearStr(m_length - len);
}

///////////////////////////////////////////////////////////////////////////////
void FString::_replace(sizeType pos, SizeT len, const char* other, SizeT n)
{
    char* buffer = nullptr;
    char* rBuffer = nullptr;

    len = _getLength(*this, pos, len);
    SizeT remainingLen = m_length - (pos + len);
    _substr(buffer, m_data, pos + len, remainingLen);
    _clearStr(pos);
    _substr(rBuffer, other, 0, n);
    _append(rBuffer, n);
    _append(buffer, remainingLen);
    SafeDelete(buffer);
    SafeDelete(rBuffer);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::_getLength(const FString& str, sizeType pos, SizeT len) const
{
    if (len == NPOS) { len = m_length - pos; }
    if (pos + len > str.m_length) { throw; }
    return len;
}

///////////////////////////////////////////////////////////////////////////////
const char* FString::CStr(void) const
{
    if (!m_data)
    {
        static const char empty[] = "";
        return empty;
    }
    return m_data;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Copy(char* str, SizeT len, sizeType pos) const
{
    if (!str) { throw; }
    len = _getLength(*this, pos, len);
    for (sizeType i = 0; i < len; ++i) { *(str + i) = operator[](pos + i); }
    return len;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Find(const FString& other, sizeType pos) const
{
    return _find(other.m_data, other.m_length, pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Find(const char* other, sizeType pos) const
{
    return _find(other, ::strlen(other), pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Find(const char* other, sizeType pos, sizeType n) const
{
    return _find(other, n, pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Find(char ch, sizeType pos) const { return _find(&ch, 1, pos); }

///////////////////////////////////////////////////////////////////////////////
SizeT FString::RFind(const FString& other, sizeType pos) const
{
    return _rfind(other.m_data, other.m_length, pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::RFind(const char* other, sizeType pos) const
{
    return _rfind(other, ::strlen(other), pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::RFind(const char* other, sizeType pos, SizeT n) const
{
    return _rfind(other, n, pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::RFind(char ch, sizeType pos) const
{
    return _rfind(&ch, 1, pos);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstOf(const FString& other, sizeType pos) const
{
    return _findFirstOf(other.m_data, other.m_length, pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstOf(const char* other, sizeType pos) const
{
    return _findFirstOf(other, ::strlen(other), pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstOf(const char* other, sizeType pos, SizeT n) const
{
    return _findFirstOf(other, n, pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstOf(char ch, sizeType pos) const
{
    return _findFirstOf(&ch, 1, pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastOf(const FString& other, sizeType pos) const
{
    return _findLastOf(other.m_data, other.m_length, pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastOf(const char* other, sizeType pos) const
{
    return _findLastOf(other, ::strlen(other), pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastOf(const char* other, sizeType pos, SizeT n) const
{
    return _findLastOf(other, n, pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastOf(char ch, sizeType pos) const
{
    return _findLastOf(&ch, 1, pos, true);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstNotOf(const FString& other, sizeType pos) const
{
    return _findFirstOf(other.m_data, other.m_length, pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstNotOf(const char* other, sizeType pos) const
{
    return _findFirstOf(other, ::strlen(other), pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstNotOf(const char* other, sizeType pos, SizeT n) const
{
    return _findFirstOf(other, n, pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindFirstNotOf(char ch, sizeType pos) const
{
    return _findFirstOf(&ch, 1, pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastNotOf(const FString& other, sizeType pos) const
{
    return _findLastOf(other.m_data, other.m_length, pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastNotOf(const char* other, sizeType pos) const
{
    return _findLastOf(other, ::strlen(other), pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastNotOf(const char* other, sizeType pos, SizeT n) const
{
    return _findLastOf(other, n, pos, false);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::FindLastNotOf(char ch, sizeType pos) const
{
    return _findLastOf(&ch, 1, pos, false);
}

///////////////////////////////////////////////////////////////////////////////
FString FString::SubStr(sizeType pos, SizeT len) const
{
    char* buffer = nullptr;

    len = _getLength(*this, pos, len);
    _substr(buffer, m_data, pos, len);
    FString toReturn(buffer);
    SafeDelete(buffer);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::_find(const char* other, SizeT len, sizeType pos) const
{
    SizeT toReturn = NPOS;

    if (pos == NPOS) { pos = m_length - 1; }
    for (; pos <= m_length; ++pos)
    {
        if (operator[](pos) == *other && _findCompare(other, len, pos))
        {
            toReturn = pos;
            break;
        }
    }
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::_rfind(const char* other, SizeT len, sizeType pos) const
{
    SizeT toReturn = NPOS;

    if (pos == NPOS || pos + len > m_length + 1) { pos = m_length - len + 1; }
    for (int i = (int)pos; 0 <= i; --i)
    {
        if (operator[](i) == *other && _findCompare(other, len, i))
        {
            toReturn = i;
            break;
        }
    }
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
bool FString::_findCompare(const char* other, SizeT len, sizeType pos) const
{
    for (sizeType i = 1; i < len; ++i)
    {
        if (operator[](pos + i) != *(other + i)) { return false; }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::_findFirstOf(
    const char* other, SizeT len, sizeType pos, bool IsTrue
) const
{
    SizeT toReturn = NPOS;

    if (pos == NPOS) { pos = m_length - 1; }
    for (; pos < m_length; ++pos)
    {
        if (_findOfCompare(other, len, pos, IsTrue))
        {
            toReturn = pos;
            break;
        }
    }
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::_findLastOf(
    const char* other, SizeT len, sizeType pos, bool IsTrue
) const
{
    SizeT toReturn = NPOS;

    if (pos == NPOS) { pos = m_length - 1; }
    for (int i = (int)pos; 0 <= i; --i)
    {
        if (_findOfCompare(other, len, i, IsTrue))
        {
            toReturn = i;
            break;
        }
    }
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
bool FString::_findOfCompare(
    const char* other, SizeT len, sizeType pos, bool IsTrue
) const
{
    for (sizeType i = 0; i < len; ++i)
    {
        if (operator[](pos) == *(other + i)) { return IsTrue; }
    }
    return !IsTrue;
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Length(void) const { return m_length; }

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Size(void) const { return Length(); }

///////////////////////////////////////////////////////////////////////////////
SizeT FString::MaxSize(void) const { return NPOS - 1; }

///////////////////////////////////////////////////////////////////////////////
void FString::Resize(SizeT n)
{
    if (m_length == n) { return; }
    if (m_length < n)
    {
        _setLength(n);
        return;
    }
    _clearStr(n);
}

///////////////////////////////////////////////////////////////////////////////
void FString::Resize(SizeT n, char filler)
{
    if (m_length == n) { return; }
    if (m_length < n)
    {
        sizeType pos = m_length;
        _setLength(n);
        _fillStr(m_data, m_length, pos, filler);
        return;
    }
    _clearStr(n);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::Capacity(void) const { return m_capacity; }

///////////////////////////////////////////////////////////////////////////////
void FString::Reserve(SizeT n)
{
    if (m_capacity == n) { return; }
    if (m_capacity < n)
    {
        _setCapacity(n);
        return;
    }
    if (m_capacity / 2 > m_length) { _decreaseCapacity(m_capacity / 2); }
}

///////////////////////////////////////////////////////////////////////////////
void FString::Clear(void) { _clearStr(0); }

///////////////////////////////////////////////////////////////////////////////
void FString::ShrinkToFit(void) { _decreaseCapacity(m_length); }

///////////////////////////////////////////////////////////////////////////////
FString& FString::ToLowerCase(void) { return _transform(::towlower); }

///////////////////////////////////////////////////////////////////////////////
FString& FString::ToUpperCase(void) { return _transform(::toupper); }

///////////////////////////////////////////////////////////////////////////////
FString& FString::Trim(void)
{
    SizeT start = 0;
    SizeT end = m_length - 1;

    for (; start < m_length && ::isspace(m_data[start]); start++);
    for (; end && ::isspace(m_data[end]); end--);

    if (start > 0 && end < m_length - 1)
    {
        SizeT newSize = end - start + 1;
        char* newStr = new char[newSize + 1];
        for (SizeT i = 0; i < newSize; i++) { newStr[i] = m_data[start + i]; }
        newStr[newSize] = '\0';
        delete[] m_data;
        m_data = newStr;
        m_length = newSize;
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
bool FString::IsEmpty(void) const { return m_length ? false : true; }

///////////////////////////////////////////////////////////////////////////////
void FString::_setLength(const SizeT len)
{
    if (m_length > len) { _clearStr(len); }
    else if (m_capacity < len) { _increaseCapacity(len); }
    m_length = len;
}

///////////////////////////////////////////////////////////////////////////////
void FString::_setCapacity(const SizeT capacity)
{
    if ((m_capacity == capacity && m_data) || capacity < m_length) { return; }
    m_capacity = capacity;

    char* buffer = m_data;
    m_data = nullptr;
    _allocCString(m_data, m_capacity);
    if (buffer)
    {
        for (sizeType i = 0; i < m_length; ++i) { operator[](i) = buffer[i]; }
        operator[](m_length) = '\0';
    }
    SafeDelete(buffer);
}

///////////////////////////////////////////////////////////////////////////////
void FString::_increaseCapacity(const SizeT capacity)
{
    if (m_capacity > capacity && m_data) { return; }
    SizeT n = m_capacity;
    while (n <= capacity) { n += m_increment; }
    m_increment++;
    _setCapacity(n);
}

///////////////////////////////////////////////////////////////////////////////
void FString::_decreaseCapacity(const SizeT capacity)
{
    if (m_capacity < capacity) { return; }
    if (m_increment > 15) { m_increment--; }
    _setCapacity(capacity);
}

///////////////////////////////////////////////////////////////////////////////
void FString::_allocCString(char*& buffer, const SizeT n) const
{
    if (buffer) { throw; }
    buffer = new char[n + 1]();
}

///////////////////////////////////////////////////////////////////////////////
void FString::_allocCString(char*& buffer, const SizeT n, char filler) const
{
    _allocCString(buffer, n);
    _fillStr(buffer, n, 0, filler);
}

///////////////////////////////////////////////////////////////////////////////
void FString::_allocCString(
    char*& buffer,
    const SizeT n,
    const ConstIterator it1,
    const ConstIterator it2
) const
{
    (void)it2;
    _allocCString(buffer, n);
    ConstIterator begin = it1;
    for (sizeType i = 0; i < n; i++) { buffer[i] = *begin++; }
}

///////////////////////////////////////////////////////////////////////////////
void FString::_fillStr(
    char* other, const SizeT len, const sizeType pos, char ch
) const
{
    sizeType begin = pos;

    while (begin != len) { other[begin++] = ch; }
    other[begin] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
void FString::_clearStr(const sizeType pos)
{
    _fillStr(m_data, m_length, pos, '\0');
    m_length = pos;
}

///////////////////////////////////////////////////////////////////////////////
template <typename F>
FString& FString::_transform(F Func)
{
    for (SizeT i = 0; i < m_length; i++) { m_data[i] = Func(m_data[i]); }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Begin(void) { return _iBegin(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::Begin(void) const { return _iBegin(); }

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::End(void) { return _end(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::End(void) const { return _end(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::CBegin(void) const { return _iBegin(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::CEnd(void) const { return _end(); }

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator FString::RBegin(void) { return _rBegin(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator FString::RBegin(void) const { return _rBegin(); }

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator FString::REnd(void) { return _end(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator FString::REnd(void) const { return _end(); }

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator FString::CRBegin(void) const
{
    return _rBegin();
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator FString::CREnd(void) const { return _end(); }

///////////////////////////////////////////////////////////////////////////////
FString::StringIteratorType FString::_iBegin(void) const
{
    return StringIteratorType(m_data, &m_length, 0);
}

///////////////////////////////////////////////////////////////////////////////
FString::StringIteratorType FString::_end(void) const
{
    return StringIteratorType(m_data, &m_length, m_length);
}

///////////////////////////////////////////////////////////////////////////////
FString::StringIteratorType FString::_rBegin(void) const
{
    return StringIteratorType(m_data, &m_length, m_length - 1);
}

///////////////////////////////////////////////////////////////////////////////
SizeT FString::_getLength(
    const ConstIterator first, const ConstIterator second
)
{
    if (first.current.first != second.current.first) { throw; }
    if (first.current.pos > second.current.pos) { throw; }
    return second.current.pos - first.current.pos;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator::ConstIterator() {}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator::ConstIterator(StringIteratorType p)
    : current(p)
{}

///////////////////////////////////////////////////////////////////////////////
const char& FString::ConstIterator::operator[](int index) const
{
    return _randomAccess(index);
}

///////////////////////////////////////////////////////////////////////////////
const char& FString::ConstIterator::operator*(void) const
{
    return retrieve();
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator& FString::ConstIterator::operator++(void)
{
    _increment();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::ConstIterator::operator++(int)
{
    ConstIterator old = *this;
    _increment();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator& FString::ConstIterator::operator--(void)
{
    _decrement();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::ConstIterator::operator--(int)
{
    ConstIterator old = *this;
    _decrement();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
bool FString::ConstIterator::operator==(const ConstIterator& rhs) const
{
    return current.first == rhs.current.first &&
           current.pos == rhs.current.pos;
}

///////////////////////////////////////////////////////////////////////////////
bool FString::ConstIterator::operator!=(const ConstIterator& rhs) const
{
    return !(*this == rhs);
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::ConstIterator::operator+(sizeType i)
{
    ConstIterator toReturn;
    _add(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstIterator FString::ConstIterator::operator-(sizeType i)
{
    ConstIterator toReturn;
    _subtract(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
char& FString::ConstIterator::retrieve(void) const
{
    return *(current.first + current.pos);
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstIterator::_increment(void)
{
    if (current.pos > *current.size) { throw; }
    current.pos++;
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstIterator::_decrement(void)
{
    if (current.pos == 0) { throw; }
    current.pos--;
}

///////////////////////////////////////////////////////////////////////////////
char& FString::ConstIterator::_randomAccess(sizeType i) const
{
    if (i >= *current.size) { throw; }
    return *(current.first + i);
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstIterator::_add(ConstIterator& toReturn, sizeType i)
{
    if (current.pos + i > *current.size) { throw; }
    toReturn = *this;
    toReturn.current.pos += i;
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstIterator::_subtract(ConstIterator& toReturn, sizeType i)
{
    if (current.pos < i) { throw; }
    toReturn = *this;
    toReturn.current.pos -= i;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator::Iterator(void)
    : _myBase()
{}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator::Iterator(StringIteratorType p)
    : _myBase(p)
{}

///////////////////////////////////////////////////////////////////////////////
char& FString::Iterator::operator[](int index)
{
    return _myBase::_randomAccess(index);
}

///////////////////////////////////////////////////////////////////////////////
const char& FString::Iterator::operator[](int index) const
{
    return _myBase::operator[](index);
}

///////////////////////////////////////////////////////////////////////////////
char& FString::Iterator::operator*(void) { return _myBase::retrieve(); }

///////////////////////////////////////////////////////////////////////////////
const char& FString::Iterator::operator*(void) const
{
    return _myBase::operator*();
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator& FString::Iterator::operator++(void)
{
    _myBase::_increment();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Iterator::operator++(int)
{
    Iterator old = *this;
    _myBase::_increment();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator& FString::Iterator::operator--(void)
{
    _myBase::_decrement();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Iterator::operator--(int)
{
    Iterator old = *this;
    _myBase::_decrement();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Iterator::operator+(sizeType i)
{
    Iterator toReturn;
    _add(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString::Iterator FString::Iterator::operator-(sizeType i)
{
    Iterator toReturn;
    _subtract(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator::ConstReverseIterator(void)
    : _myBase()
{}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator::ConstReverseIterator(StringIteratorType p)
    : _myBase(p)
{}

///////////////////////////////////////////////////////////////////////////////
const char& FString::ConstReverseIterator::operator[](int index) const
{
    return _myBase::operator[](index);
}

///////////////////////////////////////////////////////////////////////////////
const char& FString::ConstReverseIterator::operator*(void) const
{
    return _myBase::operator*();
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator& FString::ConstReverseIterator::operator++()
{
    _increment();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator FString::ConstReverseIterator::operator++(int)
{
    ConstReverseIterator old = *this;
    _increment();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator& FString::ConstReverseIterator::operator--()
{
    _decrement();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator FString::ConstReverseIterator::operator--(int)
{
    ConstReverseIterator old = *this;
    _decrement();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator
    FString::ConstReverseIterator::operator+(sizeType i)
{
    ConstReverseIterator toReturn;
    _add(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString::ConstReverseIterator
    FString::ConstReverseIterator::operator-(sizeType i)
{
    ConstReverseIterator toReturn;
    _subtract(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstReverseIterator::_increment(void)
{
    if (_myBase::current.pos == 0)
    {
        _myBase::current.pos = *_myBase::current.size;
        return;
    }
    if (current.pos == *current.size) { throw; }
    _myBase::_decrement();
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstReverseIterator::_decrement(void)
{
    if (current.pos == *_myBase::current.size)
    {
        _myBase::current.pos = 0;
        return;
    }
    if (current.pos + 1 == *_myBase::current.size) { throw; }
    _myBase::_increment();
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstReverseIterator::_add(
    ConstReverseIterator& toReturn, sizeType i
)
{
    if (current.pos == *_myBase::current.size && !i) { throw; }
    if (current.pos == i - 1)
    {
        _myBase::current.pos = *_myBase::current.size;
        return;
    }
    if (current.pos < i - 1) { throw; }
    toReturn = *this;
    toReturn.current.pos -= i;
}

///////////////////////////////////////////////////////////////////////////////
void FString::ConstReverseIterator::_subtract(
    ConstReverseIterator& toReturn, sizeType i
)
{
    if (*_myBase::current.size <= _myBase::current.pos + i) { throw; }
    toReturn = *this;
    toReturn.current.pos += i;
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator::ReverseIterator(void)
    : _myBase()
{}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator::ReverseIterator(StringIteratorType p)
    : _myBase(p)
{}

///////////////////////////////////////////////////////////////////////////////
char& FString::ReverseIterator::operator[](int index)
{
    return _myBase::_randomAccess(index);
}

///////////////////////////////////////////////////////////////////////////////
const char& FString::ReverseIterator::operator[](int index) const
{
    return _myBase::operator[](index);
}

///////////////////////////////////////////////////////////////////////////////
char& FString::ReverseIterator::operator*(void) { return _myBase::retrieve(); }

///////////////////////////////////////////////////////////////////////////////
const char& FString::ReverseIterator::operator*(void) const
{
    return _myBase::operator*();
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator& FString::ReverseIterator::operator++(void)
{
    _myBase::_increment();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator FString::ReverseIterator::operator++(int)
{
    ReverseIterator old = *this;
    _myBase::_increment();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator& FString::ReverseIterator::operator--(void)
{
    _myBase::_decrement();
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator FString::ReverseIterator::operator--(int)
{
    ReverseIterator old = *this;
    _myBase::_decrement();
    return old;
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator FString::ReverseIterator::operator+(sizeType i)
{
    ReverseIterator toReturn;
    _myBase::_add(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString::ReverseIterator FString::ReverseIterator::operator-(sizeType i)
{
    ReverseIterator toReturn;
    _myBase::_subtract(toReturn, i);
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
void FString::Swap(FString& a, FString& b) { a.Swap(b); }

///////////////////////////////////////////////////////////////////////////////
FString operator+(const FString& lhs, const FString& rhs)
{
    FString toReturn = lhs;
    toReturn += rhs;
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString operator+(const FString& lhs, const char* rhs)
{
    FString toReturn = lhs;
    toReturn += rhs;
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString operator+(const char* lhs, const FString& rhs)
{
    FString toReturn = lhs;
    toReturn += rhs;
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString operator+(const FString& lhs, char rhs)
{
    FString toReturn = lhs;
    toReturn += rhs;
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
FString operator+(char lhs, const FString& rhs)
{
    FString toReturn;
    toReturn += lhs;
    toReturn += rhs;
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const FString& str)
{
    for (size_t i = 0; i < str.Length(); i++) { os << str[i]; }
    return os;
}

///////////////////////////////////////////////////////////////////////////////
std::istream& operator>>(std::istream& is, FString& str)
{
    str.Clear();
    char ch;

    do {
        ch = is.get();
        if (ch == '\n' || ch == ' ') { break; }
        str.PushBack(ch);
    }
    while (true);
    return is;
}

///////////////////////////////////////////////////////////////////////////////
std::istream& getline(std::istream& is, FString& str, char delim)
{
    str.Clear();
    char ch;

    do {
        ch = is.get();
        if (ch == delim) { break; }
        str.PushBack(ch);
    }
    while (true);
    return is;
}

///////////////////////////////////////////////////////////////////////////////
FString::operator std::string(void) const
{
    if (!m_data) { return std::string(); }
    return std::string(this->m_data);
}

}   // namespace tkd
