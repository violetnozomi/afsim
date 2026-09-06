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

#ifndef VaBoundingBox_hpp
#define VaBoundingBox_hpp

#include "vespatk_export.h"

#include "UtVec3.hpp"

namespace vespa
{
class VESPATK_EXPORT VaBoundingBox
{
public:
   VaBoundingBox();
   VaBoundingBox(const UtVec3d& aMinLLA, const UtVec3d& aMaxLLA);
   VaBoundingBox(const UtVec3d& aMinLLA, const UtVec3d& aMaxLLA, const UtVec3d& aMeanLLA);

   void Finalize();

   void   Grow(const VaBoundingBox& aBoundingBox);
   double LatSize() { return fabs(mMaxLLA[0] - mMinLLA[0]); }
   double LonSize() { return fabs(mMaxLLA[1] - mMinLLA[1]); }
   double AltSize() { return fabs(mMaxLLA[2] - mMinLLA[2]); }

   UtVec3d mMinLLA;
   UtVec3d mMaxLLA;
   UtVec3d mMeanLLA;
   UtVec3d mCenterLLA;
   bool    mMinLLASet;
   bool    mMaxLLASet;

private:
   double CalculateMeanAltitude() const;
};
} // end namespace vespa

#endif
