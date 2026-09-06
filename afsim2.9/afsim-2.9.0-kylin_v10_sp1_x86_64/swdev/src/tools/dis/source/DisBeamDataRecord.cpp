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

#include "DisBeamDataRecord.hpp"

#include <iostream>
#include <limits>

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtMath.hpp"

DisBeamDataRecord::DisBeamDataRecord()
   : mAzimuthCenter(0.0F)
   , mAzimuthSweep(0.0F)
   , mElevationCenter(0.0F)
   , mElevationSweep(0.0F)
   , mSweepSync(0.0F)
{
}

DisBeamDataRecord::~DisBeamDataRecord() {}

void DisBeamDataRecord::Get(GenI& aGenI)
{
   aGenI >> mAzimuthCenter;
   aGenI >> mAzimuthSweep;
   aGenI >> mElevationCenter;
   aGenI >> mElevationSweep;
   aGenI >> mSweepSync;
}

void DisBeamDataRecord::Put(GenO& aGenO) const
{
   aGenO << mAzimuthCenter;
   aGenO << mAzimuthSweep;
   aGenO << mElevationCenter;
   aGenO << mElevationSweep;
   aGenO << mSweepSync;
}

void DisBeamDataRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}

std::string DisBeamDataRecord::ToString() const
{
   std::string returnString;
   returnString += "Azimuth Center:   " + std::to_string(mAzimuthCenter) + "\n";
   returnString += "Azimuth Sweep:    " + std::to_string(mAzimuthSweep) + "\n";
   returnString += "Elevation Center: " + std::to_string(mElevationCenter) + "\n";
   returnString += "Elevation Sweep:  " + std::to_string(mElevationSweep) + "\n";
   returnString += "Sweep Sync:       " + std::to_string(mSweepSync) + "\n";
   return returnString;
}

bool DisBeamDataRecord::IsValid() const
{
   static const float PI   = static_cast<const float>(UtMath::cPI);
   static const float PI_2 = static_cast<const float>(UtMath::cPI_OVER_2);

   // Allow a floating point error epsilon to limits.
   static const float ZERO_LIMIT = std::numeric_limits<float>::epsilon();
   static const float PI_LIMIT   = PI + PI * std::numeric_limits<float>::epsilon();
   static const float PI_2_LIMIT = PI_2 + PI_2 * std::numeric_limits<float>::epsilon();

   // Perform data validation.
   if (!ValidateScalar(mAzimuthCenter) || (mAzimuthCenter < -PI_LIMIT || mAzimuthCenter > UtMath::cTWO_PI))
   {
      // Valid for either +/- 180 degrees or 0 - 360 degrees
      return false;
   }
   if (!ValidateScalar(mAzimuthSweep) || (mAzimuthSweep < -ZERO_LIMIT || mAzimuthSweep > PI_LIMIT))
   {
      // Sweep measurements are always valid and half angles can't be more than
      // 180 degrees
      return false;
   }
   if (!ValidateScalar(mElevationCenter) || (mElevationCenter < -PI_2_LIMIT || mElevationCenter > PI_2_LIMIT))
   {
      // Elevation is valid for +/- 90 degrees
      return false;
   }
   if (!ValidateScalar(mElevationSweep) || (mElevationSweep < -ZERO_LIMIT || mElevationSweep > PI_LIMIT))
   {
      // Sweep must be positive and less than 180 degrees
      return false;
   }
   if (!ValidateScalar(mSweepSync) || (mSweepSync < -ZERO_LIMIT || mSweepSync >= 100.F))
   {
      // Semi-open Range of [0. - 100.)
      return false;
   }
   return true;
}
