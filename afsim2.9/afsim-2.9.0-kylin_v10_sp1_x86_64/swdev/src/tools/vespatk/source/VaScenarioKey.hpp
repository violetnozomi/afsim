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

#ifndef VaScenarioKey_hpp
#define VaScenarioKey_hpp

#include "vespatk_export.h"

#include <iostream>

namespace vespa
{
class VESPATK_EXPORT VaScenarioKey
{
public:
   VaScenarioKey();

   bool IsValid();

   operator int() const { return mId; }

   bool operator<(const VaScenarioKey& aKey) const;
   bool operator==(const VaScenarioKey& aKey) const;
   bool operator!=(const VaScenarioKey& aKey) const;
   bool operator<(int aKey) const;
   bool operator==(int aKey) const;
   bool operator!=(int aKey) const;

   friend std::ostream& operator<<(std::ostream& aOut, const VaScenarioKey& aKey)
   {
      aOut << aKey.mId;
      return aOut;
   }

private:
   friend class VaScenario;

   void Initialize();

   int        mId;
   static int sUniqueId;
};
} // end namespace vespa.

#endif
