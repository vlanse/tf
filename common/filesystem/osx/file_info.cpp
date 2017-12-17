#include <common/filesystem.h>

namespace Filesys
{
  FileInfo::FileInfo(const std::wstring& path)
    : Path(path)
  {
  }

  std::wstring FileInfo::GetPath() const
  {
    return Path;
  }
}
