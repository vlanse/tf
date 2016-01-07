/*
 * file_info.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
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
