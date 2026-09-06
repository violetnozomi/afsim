// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RelativeGeometryUpdater.hpp"

#include "RvEventPipeUtil.hpp"
#include "RvResultPlatform.hpp"
#include "UtRelativeGeometry.hpp"

namespace
{
bool BuildEntity(rv::ResultPlatform& aPlatform, double aSimTime, UtEntity& aEntity)
{
   const auto* es = aPlatform.FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (es != nullptr)
   {
      rv::BuildUtEntity(aSimTime, *es, aEntity);
      return true;
   }
   return false;
}
} // namespace

bool RelativeGeometry::LinearRangeUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                    rv::ResultPlatform* aToPlatform,
                                                    double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateLinearRange(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::RadialUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                               rv::ResultPlatform* aToPlatform,
                                               double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateRadial(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::InTrackUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                rv::ResultPlatform* aToPlatform,
                                                double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateInTrack(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::CrossTrackUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                   rv::ResultPlatform* aToPlatform,
                                                   double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateCrossTrack(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::BearingUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                rv::ResultPlatform* aToPlatform,
                                                double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateBearing(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::ElevationUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                  rv::ResultPlatform* aToPlatform,
                                                  double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateElevation(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::DownRangeUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                  rv::ResultPlatform* aToPlatform,
                                                  double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateDownRange(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::CrossRangeUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                   rv::ResultPlatform* aToPlatform,
                                                   double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateCrossRange(e1, e2);
      return true;
   }
   return false;
}

bool RelativeGeometry::RangeRateUpdater::ReadData(rv::ResultPlatform* aFromPlatform,
                                                  rv::ResultPlatform* aToPlatform,
                                                  double              aSimTime)
{
   UtEntity e1;
   UtEntity e2;

   if (BuildEntity(*aFromPlatform, aSimTime, e1) && BuildEntity(*aToPlatform, aSimTime, e2))
   {
      mValue = UtRelativeGeometry::CalculateRangeRate(e1, e2);
      return true;
   }
   return false;
}
