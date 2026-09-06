// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtCoords.hpp"

bool ut::coords::LLA::operator==(const LLA& aOther) const
{
   return mLat == aOther.mLat && mLon == aOther.mLon && mAlt == aOther.mAlt;
}
