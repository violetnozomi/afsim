// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef VaFileChooser_hpp
#define VaFileChooser_hpp

#include "vespatk_export.h"

#include <string>
#include <vector>

namespace vespa
{
class VESPATK_EXPORT VaFileChooser
{
public:
   enum ChooserType
   {
      SINGLE    = 0,
      MULTI     = 1,
      CREATE    = 2,
      DIRECTORY = 4
   };

   VaFileChooser() {}
   virtual ~VaFileChooser() {}

   virtual std::string chooseFile(int relative, const std::string& ns) = 0;
   virtual bool        chooseFiles(const std::string&        message,
                                   const std::string&        pat,
                                   const std::string&        fname,
                                   int                       relative,
                                   const std::string&        ns,
                                   std::vector<std::string>& files)    = 0;
   virtual std::string chooseDir(const std::string& message, const std::string& dname, int relative, const std::string& ns) = 0;
};
} // namespace vespa


#endif
