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
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for reading binary data from a buffer
///
/// This class provides methods to read various data types from a binary
/// buffer, including primitive types, strings, and arrays. It ensures that
/// the data is read in a consistent and efficient manner.
///
///////////////////////////////////////////////////////////////////////////////
class FBinaryReader
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    const UInt8* m_data;   //<! Pointer to the data buffer
    SizeT m_size;          //<! Size of the data buffer
    SizeT m_offset;        //<! Current read offset

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    /// \param data Pointer to the data buffer
    /// \param size Size of the data buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    FBinaryReader(const UInt8* data, SizeT size);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor from a data vector
    ///
    /// \param data Vector containing the data buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    FBinaryReader(const std::vector<UInt8>& data);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a value of type T from the binary data
    ///
    /// \tparam T The type of the value to read
    ///
    /// \param value Reference to the variable to fill with the read value
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(T& value)
    {
        static_assert(
            std::is_trivially_copyable_v<T>, "Type must be trivially copyable"
        );
        if (m_offset + sizeof(T) > m_size) { return false; }
        std::memcpy(&value, m_data + m_offset, sizeof(T));
        m_offset += sizeof(T);
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 2D vector from the binary data
    ///
    /// \param vec Reference to the vector to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TVector2<T>& vec)
    {
        return Read(vec.x) && Read(vec.y);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 3D vector from the binary data
    ///
    /// \param vec Reference to the vector to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TVector3<T>& vec)
    {
        return Read(vec.x) && Read(vec.y) && Read(vec.z);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 4D vector from the binary data
    ///
    /// \param vec Reference to the vector to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TVector4<T>& vec)
    {
        return Read(vec.x) && Read(vec.y) && Read(vec.z) && Read(vec.w);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 2D rotator from the binary data
    ///
    /// \param rot Reference to the rotator to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TRotator2D<T>& rot)
    {
        return Read(rot.GetAngle());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a rotator from the binary data
    ///
    /// \param rot Reference to the rotator to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TRotator<T>& rot)
    {
        return Read(rot.GetPitch()) && Read(rot.GetYaw()) &&
               Read(rot.GetRoll());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 2x2 matrix from the binary data
    ///
    /// \param mat Reference to the matrix to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TMatrix2x2<T>& mat)
    {
        for (SizeT i = 0; i < 2; ++i)
        {
            for (SizeT j = 0; j < 2; ++j)
            {
                if (!Read(mat(i, j))) { return false; }
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 3x3 matrix from the binary data
    ///
    /// \param mat Reference to the matrix to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TMatrix3x3<T>& mat)
    {
        for (SizeT i = 0; i < 3; ++i)
        {
            for (SizeT j = 0; j < 3; ++j)
            {
                if (!Read(mat(i, j))) { return false; }
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 4x4 matrix from the binary data
    ///
    /// \param mat Reference to the matrix to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TMatrix4x4<T>& mat)
    {
        for (SizeT i = 0; i < 4; ++i)
        {
            for (SizeT j = 0; j < 4; ++j)
            {
                if (!Read(mat(i, j))) { return false; }
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a rectangle from the binary data
    ///
    /// \param rect Reference to the rectangle to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TRectangle<T>& rect)
    {
        return Read(rect.left) && Read(rect.top) && Read(rect.width) &&
               Read(rect.height);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a 2D transform from the binary data
    ///
    /// \param tran Reference to the transform to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TTransform2D<T>& tran)
    {
        TMatrix3x3<T> mat;
        if (Read(mat))
        {
            tran.SetMatrix(mat);
            return true;
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a transform from the binary data
    ///
    /// \param tran Reference to the transform to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(TTransform<T>& tran)
    {
        for (SizeT i = 0; i < 4; ++i)
        {
            for (SizeT j = 0; j < 4; ++j)
            {
                if (!Read(tran(i, j))) { return false; }
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a vector of strings from the binary data
    ///
    /// \tparam T Type of the vector elements
    ///
    /// \param vec Reference to the vector to fill with strings
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    bool Read(std::vector<T>& vec)
    {
        SizeT size;
        if (!Read(size)) { return false; }
        vec.resize(size);
        for (SizeT i = 0; i < size; ++i)
        {
            if (!Read(vec[i])) { return false; }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a string from the binary data
    ///
    /// \param str Reference to the string to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Read(std::string& value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read a string from the binary data
    ///
    /// \param str Reference to the string to fill with data
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Read(FString& value);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Read raw bytes into a buffer
    ///
    /// \param buffer Pointer to the buffer to fill
    /// \param size Size of the buffer
    ///
    /// \return True if the read was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ReadBytes(void* buffer, SizeT size);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the current read offset
    ///
    /// \return The current read offset
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT GetOffset(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of remaining bytes to read
    ///
    /// \return The number of remaining bytes
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT GetRemaining(void) const;
};

}   // namespace tkd
