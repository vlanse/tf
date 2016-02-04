/*
 * svn_supp.h
 *
 *  Created on: Jan 26, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <common/error.h>
#include <common/filesystem.h>

#include <vector>

namespace TF
{
  namespace Svn
  {
    Common::Error IsWorkingCopy(const Filesys::Dir& dir);

    struct ChangeItem
    {
      typedef std::vector<ChangeItem> List;
      enum ItemKind
      {
        Added,
        Modified,
        Deleted,
        Unversioned
      };
      std::string FileName;
      ItemKind Kind;
    };

    ChangeItem::List GetChanges(const Filesys::Dir& dir);
  } // namespace Svn
} // namespace TF
