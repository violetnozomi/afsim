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
#ifndef VaSelectionBox_hpp
#define VaSelectionBox_hpp

#include "vespatk_export.h"

#include "UtVec3.hpp"

namespace vespa
{
class VESPATK_EXPORT VaSelectionBox
{
public:
   VaSelectionBox(const double aLat1,
                  const double aLon1,
                  const double aLat2,
                  const double aLon2,
                  const double aLat3,
                  const double aLon3,
                  const double aLat4,
                  const double aLon4);

   virtual ~VaSelectionBox();

   bool Contains(const double aLat, const double aLon, const double aAlt) const;

   const UtVec3d& GetLLA1() const { return mLLA1; }
   const UtVec3d& GetLLA2() const { return mLLA2; }
   const UtVec3d& GetLLA3() const { return mLLA3; }
   const UtVec3d& GetLLA4() const { return mLLA3; }
   const UtVec3d& GetCenterLLA() const { return mCenterLLA; }

protected:
private:
   UtVec3d mPlane1;
   UtVec3d mPlane2;
   UtVec3d mPlane3;
   UtVec3d mPlane4;
   UtVec3d mLLA1;
   UtVec3d mLLA2;
   UtVec3d mLLA3;
   UtVec3d mLLA4;
   UtVec3d mCenterLLA;
};
} // end namespace vespa
#endif
