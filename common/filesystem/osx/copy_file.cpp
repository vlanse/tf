#include <common/filesystem.h>
#include <common/string_utils.h>

#include <copyfile.h>

namespace Filesys
{
  Common::Error Copy(const FileInfo& source, const FileInfo& destination)
  {
    const std::vector<char>& src = Common::WideStringToCStr(source.GetPath());
    const std::vector<char>& dst = Common::WideStringToCStr(destination.GetPath());

    copyfile(&src.front(), &dst.front(), NULL, COPYFILE_ALL | COPYFILE_RECURSIVE | COPYFILE_NOFOLLOW);

    return Common::Success;
  }
} // namespace Filesys
