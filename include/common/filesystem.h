/*
 * dir.h
 *
 *  Created on: Jan 4, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#pragma once

#include <common/error.h>

#include <functional>

namespace Filesys
{
  class FileInfo
  {
  public:
    FileInfo(const std::wstring& path);
    std::wstring GetPath() const;
  private:
    std::wstring Path;
  };

  class Dir
  {
  public:
    Dir(const std::wstring& path);
    std::wstring GetPath() const;
    FileInfo GetFileInfo() const;
  private:
    FileInfo Info;
  };

  typedef std::function<bool (std::size_t, std::size_t)> ProgressCallback; // first argument - total count, second - processed count
  Common::Error RemoveDirRecursive(const Dir& dir, ProgressCallback progress = ProgressCallback());

  Common::Error CreateDir(const std::wstring& path);

  Common::Error Copy(const FileInfo& source, const FileInfo& destination);
} // namespace Platform
