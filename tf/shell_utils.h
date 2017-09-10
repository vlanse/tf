/*
 * shell_utils.h
 *
 *  Created on: Feb 13, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QString>

namespace TF
{
  namespace Shell
  {
    void OpenEditorForFile(const QString& file);
    void OpenTerminal(const QString& path);
    void RevealInFinder(const QString& path);
  } // namespace Shell
} // namespace TF
