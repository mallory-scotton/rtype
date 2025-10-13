///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Core/Math.hpp>
#include <type_traits>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief FBinaryWriter class for writing binary data to a buffer
///
/// This class provides methods to write various data types to a binary
/// buffer, including primitive types, strings, and arrays. It ensures that
/// the data is written in a consistent and efficient manner.
///
///////////////////////////////////////////////////////////////////////////////
class FBinaryWriter
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    std::vector<UInt8>& m_buffer;   //<! Buffer to hold binary data
    SizeT m_offset = 0;             //<! Current write offset in the buffer

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    /// \param buffer Reference to the buffer to write to
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit FBinaryWriter(std::vector<UInt8>& buffer);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a trivially copyable value to the buffer
    ///
    /// \tparam T Type of the value to write
    ///
    /// \param value The value to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const T& value)
    {
        static_assert(
            std::is_trivially_copyable_v<T>, "Type must be trivially copyable"
        );
        SizeT needed = m_offset + sizeof(T);
        if (m_buffer.size() < needed) { m_buffer.resize(needed); }
        std::memcpy(m_buffer.data() + m_offset, &value, sizeof(T));
        m_offset += sizeof(T);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a string to the buffer
    ///
    /// \param str The string to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TVector2<T>& vec)
    {
        Write(vec.x);
        Write(vec.y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TVector3 to the buffer
    ///
    /// \param vec The TVector3 to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TVector3<T>& vec)
    {
        Write(vec.x);
        Write(vec.y);
        Write(vec.z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TVector4 to the buffer
    ///
    /// \param vec The TVector4 to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TVector4<T>& vec)
    {
        Write(vec.x);
        Write(vec.y);
        Write(vec.z);
        Write(vec.w);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TRotator2D to the buffer
    ///
    /// \param rot The TRotator2D to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TRotator2D<T>& rot)
    {
        Write(rot.GetAngle());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TRotator to the buffer
    ///
    /// \param rot The TRotator to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TRotator<T>& rot)
    {
        Write(rot.GetPitch());
        Write(rot.GetYaw());
        Write(rot.GetRoll());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TMatrix2x2 to the buffer
    ///
    /// \param mat The TMatrix2x2 to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TMatrix2x2<T>& mat)
    {
        for (SizeT i = 0; i < 2; ++i)
        {
            for (SizeT j = 0; j < 2; ++j) { Write(mat(i, j)); }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TMatrix3x3 to the buffer
    ///
    /// \param mat The TMatrix3x3 to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TMatrix3x3<T>& mat)
    {
        for (SizeT i = 0; i < 3; ++i)
        {
            for (SizeT j = 0; j < 3; ++j) { Write(mat(i, j)); }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TMatrix4x4 to the buffer
    ///
    /// \param mat The TMatrix4x4 to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TMatrix4x4<T>& mat)
    {
        for (SizeT i = 0; i < 4; ++i)
        {
            for (SizeT j = 0; j < 4; ++j) { Write(mat(i, j)); }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TRectangle to the buffer
    ///
    /// \param rect The TRectangle to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TRectangle<T>& rect)
    {
        Write(rect.left);
        Write(rect.top);
        Write(rect.width);
        Write(rect.height);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TTransform2D to the buffer
    ///
    /// \param tran The TTransform2D to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TTransform2D<T>& tran)
    {
        Write(tran.GetMatrix());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a TTransform to the buffer
    ///
    /// \param tran The TTransform to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const TTransform<T>& tran)
    {
        for (SizeT i = 0; i < 4; ++i)
        {
            for (SizeT j = 0; j < 4; ++j) { Write(tran(i, j)); }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a vector of trivially copyable values to the buffer
    ///
    /// \tparam T Type of the values in the vector
    ///
    /// \param array The vector of values to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Write(const std::vector<T>& array)
    {
        SizeT arraySize = array.size();
        Write(arraySize);
        if (arraySize > 0)
        {
            SizeT needed = m_offset + arraySize * sizeof(T);
            if (m_buffer.size() < needed) { m_buffer.resize(needed); }
            std::memcpy(
                m_buffer.data() + m_offset, array.data(), arraySize * sizeof(T)
            );
            m_offset += arraySize * sizeof(T);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a vector of strings to the buffer
    ///
    /// \param array The vector of strings to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Write(const std::vector<std::string>& array);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a vector of strings to the buffer
    ///
    /// \param array The vector of strings to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Write(const std::vector<FString>& array);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a string to the buffer
    ///
    /// \param str The string to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Write(const std::string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes an FString to the buffer
    ///
    /// \param str The FString to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Write(const FString& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a FVector3 to the buffer
    ///
    /// \param value The FVector3 to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Write(const FVector3& value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a FRotator to the buffer
    ///
    /// \param value The FRotator to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Write(const FRotator& value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a byte array to the buffer
    ///
    /// \param data Pointer to the byte array
    /// \param size Size of the byte array
    ///
    ///////////////////////////////////////////////////////////////////////////
    void WriteBytes(const UInt8* data, SizeT size);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the current write offset in the buffer
    ///
    /// \return The current write offset in the buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT GetOffset(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the current write offset in the buffer
    ///
    /// \param offset New write offset in the buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetOffset(SizeT offset);
};

}   // namespace tkd
