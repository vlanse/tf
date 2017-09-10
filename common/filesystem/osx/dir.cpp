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
  const char PATH_SEPARATOR = '/';

  namespace
  {
    typedef std::function<bool(FTSENT*)> TraverseCallbackFunction;
    Common::Error TraverseDirectoryTree(const Dir& dir, TraverseCallbackFunction traverseCallback, bool depthFirst = true)
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

        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:

        case FTS_D:
          if (depthFirst)
          {
            break;
          }
          if (!traverseCallback(curr))
          {
            fts_set(ftsp, curr, FTS_SKIP);
          }
          break;
        case FTS_DP:
          if (!depthFirst)
          {
            break;
          }
          if (!traverseCallback(curr))
          {
            fts_set(ftsp, curr, FTS_SKIP);
          }
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

    bool EntryCounter(std::size_t& count, FTSENT* /*unused*/)
    {
      ++count;
      return true;
    }

    bool RemoveEntry(std::size_t totalCount, std::size_t& processed, ProgressCallback progress, FTSENT* curr)
    {
      if (remove(curr->fts_accpath) < 0)
      {
        DEBUG(Common::MODULE_COMMON, L"Failed to remove: " + Common::StringToWideString(curr->fts_accpath));
        return true;
      }
      if (progress)
      {
        progress(totalCount, ++processed);
      }
      DEBUG(Common::MODULE_COMMON, L"Removed: " + Common::StringToWideString(curr->fts_accpath));
      return true;
    }

    bool ProcessEntry(WalkCallback callback, FTSENT* curr)
    {
      FileObjectType fileType = FILE_OTHER;
      if (curr->fts_info == FTS_F)
      {
        fileType = FILE_REGULAR;
      }
      else if (curr->fts_info == FTS_D || curr->fts_info == FTS_DC || curr->fts_info == FTS_DP)
      {
        fileType = FILE_DIRECTORY;
      }
      return callback(curr->fts_accpath, fileType);
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

  int CountFiles(const Dir& dir)
  {
    std::size_t entries = 0;
    TraverseDirectoryTree(dir, std::bind(EntryCounter, std::ref(entries), std::placeholders::_1));
    return entries;
  }

  Common::Error CreateDir(const std::wstring& path)
  {
    DEBUG(Common::MODULE_COMMON, L"CreateDir: " + path);
    mkdir(Common::WideStringToString(path).c_str(), 0755);
    return Common::Success;
  }

  Common::Error WalkDir(const Dir& dir, WalkCallback callback, bool depthFirst)
  {
    return TraverseDirectoryTree(dir, std::bind(ProcessEntry, callback, std::placeholders::_1), depthFirst);
  }
} // namespace Filesys
