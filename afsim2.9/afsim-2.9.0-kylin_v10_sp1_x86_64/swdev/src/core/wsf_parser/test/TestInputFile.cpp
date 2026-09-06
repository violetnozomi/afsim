// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "TestInputFile.hpp"

#include <fstream>

#include <gtest/gtest.h>

namespace wsf
{
namespace parser
{
namespace test
{

InputFile::InputFile(const UtPath& aPathPrefix, const std::string& aName, const std::string& aContents)
   : mPathPrefix(aPathPrefix)
   , mName(aName)
{
   WriteSource(aContents);
}

InputFile::~InputFile()
{
   RemoveSource();
}

UtPath InputFile::GetPathPrefix() const noexcept
{
   return mPathPrefix;
}

std::string InputFile::GetName() const noexcept
{
   return mName;
}

UtPath InputFile::GetPath() const noexcept
{
   return mPathPrefix + mName;
}

std::string InputFile::GetPathString() const noexcept
{
   return GetPath().GetNormalizedPath();
}

void InputFile::WriteSource(const std::string& aContents)
{
   std::ofstream ofs{GetPathString(), std::ios::out | std::ios::binary};
   ASSERT_TRUE(ofs.is_open());
   // operator<< is not guaranteed to work with binary data.
   // For this reason, use .write instead.
   ofs.write(aContents.data(), aContents.size());
   ofs.close();
   ASSERT_FALSE(ofs.is_open());
}

void InputFile::RemoveSource()
{
   UtPath path{GetPath()};
   if (path.Exists())
   {
      path.RemoveFile();
   }
}

} // namespace test
} // namespace parser
} // namespace wsf
