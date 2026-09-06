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
#include <gtest/gtest.h>

#include "UtEntity.hpp"
#include "UtEntityPart.hpp"
#include "UtMath.hpp"

TEST(UtEntityPart, GetOrientationECI)
{
   UtEntity e;
   e.SetLocationLLA(40.0, 50.0, 0.0);
   UtCalendar j2000 = UtCalendar();
   j2000.SetDate(2000, 1, 1);
   j2000.SetTime(12, 0, 0.0);

   e.SetECI_ReferenceEpoch(j2000);

   UtEntityPart ep(&e);
   e.SetOrientationWCS(0.0, 0.0, 0.0);

   double pYaw   = 90.0 * UtMath::cRAD_PER_DEG;
   double pPitch = 45.0 * UtMath::cRAD_PER_DEG;
   double pRoll  = 30.0 * UtMath::cRAD_PER_DEG;
   ep.SetOrientation(pYaw, pPitch, pRoll);

   double eYawECI, ePitchECI, eRollECI;
   double epYawECI, epPitchECI, epRollECI;

   e.GetOrientationECI(eYawECI, ePitchECI, eRollECI);
   ep.GetOrientationECI(epYawECI, epPitchECI, epRollECI);
   double yawDiff   = epYawECI - eYawECI;
   double pitchDiff = epPitchECI - ePitchECI;
   double rollDiff  = epRollECI - eRollECI;

   EXPECT_NEAR(yawDiff, pYaw, 1.0e-4);
   EXPECT_NEAR(pitchDiff, pPitch, 1.0e-4);
   EXPECT_NEAR(rollDiff, pRoll, 1.0e-4);

   e.SetOrientationECI(0.0, 0.0, 0.0);
   ep.InvalidateTransform();
   ep.GetOrientationECI(epYawECI, epPitchECI, epRollECI);

   EXPECT_NEAR(epYawECI, pYaw, 1.0e-4);
   EXPECT_NEAR(epPitchECI, pPitch, 1.0e-4);
   EXPECT_NEAR(epRollECI, pRoll, 1.0e-4);
}
