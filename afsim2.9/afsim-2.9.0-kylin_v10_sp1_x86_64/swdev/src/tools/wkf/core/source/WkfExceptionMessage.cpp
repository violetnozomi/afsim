// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfExceptionMessage.hpp"

namespace wkf
{
namespace Exception
{
QString GetExceptionMessageBoxString(const QString& aExceptionString, const QString& aFunctionName, const QString& aPluginName)
{
   QString msg("Unhandled exception in function \"%1\".\n"
               "Exception: %2\n\n"
               "See '%3' for more information.\n"
               "Please provide this log with a bug report to the AFSIM Development Team.");

   if (!aPluginName.isEmpty())
   {
      msg.append("\n\nIf this problem persists, consider unloading the %4 plugin.");
   }
   return msg.arg(aFunctionName, aExceptionString, QString::fromStdString(ut::GetApplicationLogPath()), aPluginName);
}
} // namespace Exception
} // namespace wkf
