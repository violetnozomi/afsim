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

#include "mapUtil.hpp"

#include <string>

namespace artificer
{
std::map<std::string, std::string> ParseNameValuePairs(const std::string& nameValuePairs)
{
   std::map<std::string, std::string> nvMap;

   auto addPair = [&nvMap](const std::string& pairStr)
   {
      if (!pairStr.empty())
      {
         auto splitPos = pairStr.find_first_of('=');
         if (splitPos != std::string::npos)
         {
            nvMap[pairStr.substr(0, splitPos)] = pairStr.substr(splitPos + 1);
         }
         else
         {
            nvMap[pairStr] = "";
         }
      }
   };
   size_t startAt = 0;
   while (true)
   {
      const size_t pairSplit = nameValuePairs.find_first_of(';', startAt);
      if (pairSplit == std::string::npos)
      {
         addPair(nameValuePairs.substr(startAt));
         break;
      }
      addPair(nameValuePairs.substr(startAt, pairSplit - startAt));
      startAt = pairSplit + 1;
   }

   return nvMap;
}

} // namespace artificer
