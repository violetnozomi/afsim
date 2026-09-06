// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UNITTESTFILEHELPER_H
#define UNITTESTFILEHELPER_H

#include <fstream>

#include <gtest/gtest.h>

class UnitTestFilePathHelper
{
public:
   // Not copyable
   UnitTestFilePathHelper(const UnitTestFilePathHelper&) = delete;
   UnitTestFilePathHelper& operator=(const UnitTestFilePathHelper&) = delete;

   static UnitTestFilePathHelper& Instance(const std::string& aRootDir = "")
   {
      static UnitTestFilePathHelper mInstance(aRootDir);
      return mInstance;
   }

   std::string Path(const std::string& aRelativePath) { return mRootDir + "/" + aRelativePath; }

private:
   UnitTestFilePathHelper(const std::string& aProjectRootDir)
      : mRootDir(aProjectRootDir)
   {
   }
   std::string mRootDir;
};

#endif
