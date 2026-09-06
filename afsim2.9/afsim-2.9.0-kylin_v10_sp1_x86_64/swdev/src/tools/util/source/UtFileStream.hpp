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

#ifndef UTFILESTREAM_HPP
#define UTFILESTREAM_HPP
#include "ut_export.h"

#include <fstream>
#include <iostream>
#include <memory>

// std::fstream doesn't allow retrieval of file path or open mode
// which means it's impossible to know what the stream represents
// unless we keep that information.  This class wraps the fstream
// to do just that
class UT_EXPORT UtFileStream
{
public:
   UtFileStream()  = default;
   ~UtFileStream() = default;
   void open(const char* aFileName, std::ios_base::openmode aOpenMode);
   void close();

   // Returns a pointer to the stream. Returns 'null' if the stream has not been open.
   std::fstream* stream() { return mStreamPtr.get(); }

   const std::string&      GetFilePath() const { return mFilePath; }
   std::ios_base::openmode GetOpenMode() const { return mOpenMode; }

private:
   std::unique_ptr<std::fstream> mStreamPtr{nullptr};
   std::string                   mFilePath{};
   std::ios_base::openmode       mOpenMode{};
};

#endif
