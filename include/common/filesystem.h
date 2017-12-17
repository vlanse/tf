#pragma once

#include <common/error.h>

#include <functional>

namespace Filesys
{
  extern const char PATH_SEPARATOR;

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

  int CountFiles(const Dir& dir);

  enum FileObjectType
  {
    FILE_REGULAR,
    FILE_DIRECTORY,
    FILE_OTHER = 999,
  };

  typedef std::function<bool (const std::string&, FileObjectType)> WalkCallback;
  Common::Error WalkDir(const Dir& dir, WalkCallback callback, bool depthFirst = true);
} // namespace Platform
