// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTTESTDATASERVICE_HPP
#define UTTESTDATASERVICE_HPP

#include "UtPath.hpp"

namespace ut
{

class TestDataService
{
public:
   TestDataService(UtPath aResourcePath)
      : mResourcePath(aResourcePath)
   {
   }

   UtPath getResource(const std::string& aPath) const { return mResourcePath + aPath; }

private:
   UtPath mResourcePath;
};

} // namespace ut

#endif
