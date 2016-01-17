/*
 * dir.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include <common/filesystem.h>
#include <common/string_utils.h>
#include <common/trace.h>

#include <vector>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>

namespace Filesys
{
  namespace
  {
    typedef std::function<void(FTSENT*)> TraverseCallbackFunction;
    Common::Error TraverseDirectoryTree(const Dir& dir, TraverseCallbackFunction traverseCallback)
    {
      std::vector<char> buffer = Common::WideStringToCStr(dir.GetPath());
      int ret = 0;

      FTS* ftsp = NULL;
      FTSENT* curr;

      char *files[] = { &buffer.front(), NULL };

      // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
      //                in multithreaded programs
      // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
      //                of the specified directory
      // FTS_XDEV     - Don't cross filesystem boundaries
      ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
      if (!ftsp)
      {
        fprintf(stderr, "%s: fts_open failed: %s\n", &buffer.front(), strerror(errno));
        ret = -1;
        goto finish;
      }

      while ((curr = fts_read(ftsp)))
      {
        switch (curr->fts_info)
        {
        case FTS_NS:
        case FTS_DNR:
        case FTS_ERR:
          DEBUG(
            Common::MODULE_COMMON,
            std::wstring(L"fts_read error: ")
            + L" " + Common::StringToWideString(curr->fts_accpath)
            + L" " + Common::StringToWideString(std::string(strerror(curr->fts_errno)))
          );
          break;
        case FTS_DC:
        case FTS_DOT:
        case FTS_NSOK:
          // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
          // passed to fts_open()
          break;
        case FTS_D:
          // Do nothing. Need depth-first search, so directories are deleted
          // in FTS_DP
          break;
        case FTS_DP:
        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
          DEBUG(Common::MODULE_COMMON, L"traverse: " + Common::StringToWideString(curr->fts_accpath));
          traverseCallback(curr);
          break;
        }
      }

      finish:
      if (ftsp)
      {
        // TODO: use RAII
        fts_close(ftsp);
      }

      if (errno != 0)
      {
        return MAKE_ERROR(MAKE_MODULE_ERROR(Common::MODULE_OS, errno), Common::StringToWideString(strerror(errno)));
      }
      return Common::Success;
    }

    void EntryCounter(std::size_t& count, FTSENT* /*unused*/)
    {
      ++count;
    }

    void RemoveEntry(std::size_t totalCount, std::size_t& processed, ProgressCallback progress, FTSENT* curr)
    {
      if (remove(curr->fts_accpath) < 0)
      {
        DEBUG(Common::MODULE_COMMON, L"Failed to remove: " + Common::StringToWideString(curr->fts_accpath));
        return;
      }
      if (progress)
      {
        progress(totalCount, ++processed);
      }
      DEBUG(Common::MODULE_COMMON, L"Removed: " + Common::StringToWideString(curr->fts_accpath));
    }
  } // namespace

  Dir::Dir(const std::wstring& path)
    : Info(path)
  {
  }

  std::wstring Dir::GetPath() const
  {
    return Info.GetPath();
  }

  FileInfo Dir::GetFileInfo() const
  {
    return Info;
  }

  Common::Error RemoveDirRecursive(const Dir& dir, ProgressCallback progress)
  {
    DEBUG(Common::MODULE_COMMON, std::wstring(L"RemoveDirRecursive: ") + dir.GetPath());
    const bool withProgress = static_cast<bool>(progress);
    std::size_t entries = 0;

    RETURN_IF_FAILED(TraverseDirectoryTree(dir, std::bind(EntryCounter, std::ref(entries), std::placeholders::_1)));

    std::wstring arg = Common::ToString<unsigned, std::wstring>(entries);
    DEBUG(Common::MODULE_COMMON, L"Total sub entries:" + arg);

    std::size_t processed = 0;
    return TraverseDirectoryTree(dir, std::bind(RemoveEntry, entries, std::ref(processed), progress, std::placeholders::_1));
  }

  Common::Error CreateDir(const std::wstring& path)
  {
    DEBUG(Common::MODULE_COMMON, L"CreateDir: " + path);
    mkdir(Common::WideStringToString(path).c_str(), 0755);
    return Common::Success;
  }
} // namespace Filesys
