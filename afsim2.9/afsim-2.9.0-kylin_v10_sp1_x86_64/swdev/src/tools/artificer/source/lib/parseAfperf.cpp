// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "parseAfperf.hpp"

#include <cstring>
#include <istream>

namespace artificer
{
std::string ReadAfperfHeader(std::istream& aData)
{
   const int kHeaderSize = 16;
   char      rawHeader[kHeaderSize];
   aData.read(rawHeader, kHeaderSize);
   if (aData.gcount() < kHeaderSize)
   {
      throw std::runtime_error("Unable to read afperf header.");
   }
   return std::string(rawHeader, kHeaderSize);
}

} // namespace artificer
