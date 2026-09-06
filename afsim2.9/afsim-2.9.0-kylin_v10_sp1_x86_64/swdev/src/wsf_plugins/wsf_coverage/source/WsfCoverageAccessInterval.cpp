// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCoverageAccessInterval.hpp"

#include <algorithm>

namespace wsf
{
namespace coverage
{

bool AccessInterval::operator==(const AccessInterval& aOther) const
{
   return mStart == aOther.mStart && mEnd == aOther.mEnd;
}

bool AccessInterval::HasOverlapWith(const AccessInterval& aOther) const
{
   return mStart <= aOther.GetEnd() && mEnd >= aOther.GetStart();
}

AccessInterval AccessInterval::UnionWith(const AccessInterval& aOther) const
{
   AccessInterval retval{};
   if (HasOverlapWith(aOther))
   {
      retval.SetStart(std::min(mStart, aOther.GetStart()));
      retval.SetEnd(std::max(mEnd, aOther.GetEnd()));
   }
   return retval;
}

AccessInterval AccessInterval::IntersectionWith(const AccessInterval& aOther) const
{
   AccessInterval retval{};
   if (HasOverlapWith(aOther))
   {
      retval.SetStart(std::max(mStart, aOther.GetStart()));
      retval.SetEnd(std::min(mEnd, aOther.GetEnd()));
   }
   return retval;
}

bool AccessInterval::Contains(double aSimTime) const
{
   return aSimTime >= mStart && aSimTime <= mEnd;
}

} // namespace coverage
} // namespace wsf
