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

#ifndef UTVECTORUTIL_HPP
#define UTVECTORUTIL_HPP

#include <vector>

namespace UtVectorUtil
{
// If vector is too small, will resize vector and fill the new undefined
// locations with "aDefaultValue".  Once the correct size is ensured, "aValue"
// will be put in the location defined by "aIndex".
template<class T>
void Set(std::vector<T>& aVec, typename std::vector<T>::size_type aIndex, const T& aValue, const T& aDefaultValue)
{
   if (aIndex == aVec.size())
   {
      aVec.push_back(aValue);
   }
   else
   {
      if (aIndex > aVec.size())
      {
         aVec.resize(aIndex + 1, aDefaultValue);
      }
      aVec[aIndex] = aValue;
   }
}
} // namespace UtVectorUtil

#endif
