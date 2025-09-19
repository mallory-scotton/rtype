///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Archives/FSaveArchive.hpp>
#include <Engine/Config.hpp>
#include <Engine/Core/Utils/FileSystem.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FSaveArchive::FSaveArchive(const FilePath& filePath, Int32 version)
    : FArchive(
          EArchiveMode::Saving,
          [filePath, this](TKD_MAYBE_UNUSED FArchive& archive)
          {
              if (FileSystem::WriteBytesToFile(filePath, m_data))
              {
                  m_data.clear();
                  m_position = 0;
              }
          },
          true
      )
{
    m_version = version;
}

}   // namespace tkd
