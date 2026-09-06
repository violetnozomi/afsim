// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#include "UtFileStream.hpp"

#include "UtMemory.hpp"

void UtFileStream::close()
{
   mStreamPtr = nullptr;
   mFilePath.clear();
   mOpenMode = static_cast<std::ios_base::openmode>(0);
}

void UtFileStream::open(const char* aFileName, std::ios_base::openmode aOpenMode)
{
   close();
   mStreamPtr = ut::make_unique<std::fstream>(aFileName, aOpenMode);
   if (mStreamPtr->good())
   {
      mOpenMode = aOpenMode;
      mFilePath = aFileName;
   }
}
