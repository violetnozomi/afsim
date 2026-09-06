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
#include "UtVec3.hpp"

namespace
{
const UtCalendar cEPOCH;
const auto       cEARTH = ut::EarthEGM96();

// WCS
const UtVec3d cLOC_WCS(cEARTH.GetMeanRadius() / sqrt(2.0), 0.0, cEARTH.GetMeanRadius() / sqrt(2.0));
const UtVec3d cVEL_WCS(0.0, 0.0, 100.0);
const UtVec3d cACC_WCS(-7.0, 0.0, -7.0);

// ECI
const UtVec3d cLOC_ECI(1577626.926, 4220306.201, 4504415.602);
const UtVec3d cVEL_ECI(-307.7127078, 114.9452286, 100.0905818);
const UtVec3d cACC_ECI(-2.45975643, -6.580107692, -6.999123352);

// LLA/NED
constexpr double cLAT = 45.192315;
constexpr double cLON = 0.0;
constexpr double cALT = 3583.044838;

const UtVec3d cVEL_NED(70.47293773, 0, -70.94762186);
const UtVec3d cACC_NED(0.03322788886, 0, 9.899439171);

} // namespace

TEST(UtEntity, IncrementLocationWCS)
{
   // Initialize two entities to the same location (LLA)
   UtEntity e1;
   e1.SetLocationLLA(0.0, 0.0, 0.0);
   UtEntity e2{e1};

   // Increment the position of one of them by delta (WCS)
   UtVec3d delta{1000.0, 1000.0, 1000.0};
   e2.IncrementLocationWCS(delta.GetData());
   // Apply the inverse increment
   e2.IncrementLocationWCS((-delta).GetData());

   // Both entities should be at same location
   UtVec3d wcs1, wcs2;
   e1.GetLocationWCS(wcs1.GetData());
   e2.GetLocationWCS(wcs2.GetData());
   EXPECT_EQ(wcs1, wcs2);
}

TEST(UtEntity, IncrementLocationNED)
{
   // Initialize two entities to the same location (LLA)
   UtEntity e1;
   e1.SetLocationLLA(0.0, 0.0, 0.0);
   UtEntity e2{e1};

   UtVec3d deltaNED{1000.0, 1000.0, 1000.0};
   // Increment the position of one of them by delta (NED)
   e2.IncrementLocationNED(deltaNED.GetData());
   // Get relative location from one to the other
   UtVec3d relativeNED;
   e1.GetRelativeLocationNED(&e2, relativeNED.GetData());

   // Delta increment should equal the relative location
   EXPECT_EQ(deltaNED, relativeNED);
}

TEST(UtEntity, AccelerationConversions)
{
   UtVec3d loc(cEARTH.GetMeanRadius() / sqrt(2.0), 0.0, cEARTH.GetMeanRadius() / sqrt(2.0));
   UtVec3d vel(0.0, 0.0, 100.0);

   UtVec3d acc(-7.0, 0.0, -7.0);
   UtVec3d zeroAcc;

   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationWCS(loc.GetData());
   entity.SetVelocityWCS(vel.GetData());

   // WCS->ECI->WCS
   entity.SetAccelerationECI(zeroAcc.GetData());
   entity.SetAccelerationWCS(acc.GetData());
   UtVec3d accECI;
   entity.GetAccelerationECI(accECI.GetData());

   EXPECT_NE(accECI, zeroAcc); // ECI acceleration was updated from WCS

   // zero out the wcs acceleration and test update from inertial
   entity.SetAccelerationWCS(zeroAcc.GetData());
   entity.SetAccelerationECI(accECI.GetData());
   UtVec3d newAcc;
   entity.GetAccelerationWCS(newAcc.GetData());
   EXPECT_NEAR(acc[0], newAcc[0], 1.0e-8);
   EXPECT_NEAR(acc[1], newAcc[1], 1.0e-8);
   EXPECT_NEAR(acc[2], newAcc[2], 1.0e-8);

   // WCS->TOD->WCS
   entity.SetAccelerationTOD(zeroAcc.GetData());
   entity.SetAccelerationWCS(acc.GetData());
   UtVec3d accTOD;
   entity.GetAccelerationTOD(accTOD.GetData());

   EXPECT_NE(accTOD, zeroAcc); // TOD acceleration was updated from WCS

   // zero out the wcs acceleration and test update from tod
   entity.SetAccelerationWCS(zeroAcc.GetData());
   entity.SetAccelerationTOD(accTOD.GetData());
   entity.GetAccelerationWCS(newAcc.GetData());
   EXPECT_NEAR(acc[0], newAcc[0], 1.0e-8);
   EXPECT_NEAR(acc[1], newAcc[1], 1.0e-8);
   EXPECT_NEAR(acc[2], newAcc[2], 1.0e-8);
}

TEST(UtEntity, AccelerationECItoWCS)
{
   // Set ECI
   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationECI(cLOC_ECI.GetData());
   entity.SetVelocityECI(cVEL_ECI.GetData());
   entity.SetAccelerationECI(cACC_ECI.GetData());

   // Get WCS
   UtVec3d accWCS;
   entity.GetAccelerationWCS(accWCS.GetData());

   // Acceleration
   EXPECT_NEAR(accWCS[0], cACC_WCS[0], 1e-8);
   EXPECT_NEAR(accWCS[1], cACC_WCS[1], 1e-8);
   EXPECT_NEAR(accWCS[2], cACC_WCS[2], 1e-8);
}

TEST(UtEntity, AccelerationECItoNED)
{
   // Set ECI
   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationECI(cLOC_ECI.GetData());
   entity.SetVelocityECI(cVEL_ECI.GetData());
   entity.SetAccelerationECI(cACC_ECI.GetData());

   // Get NED
   UtVec3d accNED;
   entity.GetAccelerationNED(accNED.GetData());

   // Acceleration
   EXPECT_NEAR(accNED[0], cACC_NED[0], 1e-8);
   EXPECT_NEAR(accNED[1], cACC_NED[1], 1e-8);
   EXPECT_NEAR(accNED[2], cACC_NED[2], 1e-8);
}

TEST(UtEntity, AccelerationWCStoECI)
{
   // Set ECI
   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationWCS(cLOC_WCS.GetData());
   entity.SetVelocityWCS(cVEL_WCS.GetData());
   entity.SetAccelerationWCS(cACC_WCS.GetData());

   // Get NED
   UtVec3d accECI;
   entity.GetAccelerationECI(accECI.GetData());

   // Acceleration
   EXPECT_NEAR(accECI[0], cACC_ECI[0], 1e-8);
   EXPECT_NEAR(accECI[1], cACC_ECI[1], 1e-8);
   EXPECT_NEAR(accECI[2], cACC_ECI[2], 1e-8);
}

TEST(UtEntity, AccelerationWCStoNED)
{
   // Set ECI
   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationWCS(cLOC_WCS.GetData());
   entity.SetVelocityWCS(cVEL_WCS.GetData());
   entity.SetAccelerationWCS(cACC_WCS.GetData());

   // Get NED
   UtVec3d accNED;
   entity.GetAccelerationNED(accNED.GetData());

   // Acceleration
   EXPECT_NEAR(accNED[0], cACC_NED[0], 1e-8);
   EXPECT_NEAR(accNED[1], cACC_NED[1], 1e-8);
   EXPECT_NEAR(accNED[2], cACC_NED[2], 1e-8);
}


TEST(UtEntity, AccelerationNEDtoWCS)
{
   // Set ECI
   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationLLA(cLAT, cLON, cALT);
   entity.SetVelocityNED(cVEL_NED.GetData());
   entity.SetAccelerationNED(cACC_NED.GetData());

   // Get NED
   UtVec3d accWCS;
   entity.GetAccelerationWCS(accWCS.GetData());

   // Acceleration
   EXPECT_NEAR(accWCS[0], cACC_WCS[0], 1e-8);
   EXPECT_NEAR(accWCS[1], cACC_WCS[1], 1e-8);
   EXPECT_NEAR(accWCS[2], cACC_WCS[2], 1e-8);
}

TEST(UtEntity, AccelerationNEDtoECI)
{
   // Set ECI
   UtEntity entity;
   entity.SetECI_ReferenceEpoch(cEPOCH);
   entity.SetLocationLLA(cLAT, cLON, cALT);
   entity.SetVelocityNED(cVEL_NED.GetData());
   entity.SetAccelerationNED(cACC_NED.GetData());

   // Get NED
   UtVec3d accECI;
   entity.GetAccelerationECI(accECI.GetData());

   // Acceleration
   EXPECT_NEAR(accECI[0], cACC_ECI[0], 1e-8);
   EXPECT_NEAR(accECI[1], cACC_ECI[1], 1e-8);
   EXPECT_NEAR(accECI[2], cACC_ECI[2], 1e-8);
}

TEST(UtEntity, VelocityECIToWCS)
{
   UtEntity entity;

   // Set ECI, Get WCS
   UtVec3d velWCS;
   entity.ConvertVelocityECIToWCS(cLOC_ECI.GetData(), cVEL_ECI.GetData(), velWCS.GetData());

   // Compare Velocities
   EXPECT_NEAR(velWCS[0], cVEL_WCS[0], 1e-6);
   EXPECT_NEAR(velWCS[1], cVEL_WCS[1], 1e-6);
   EXPECT_NEAR(velWCS[2], cVEL_WCS[2], 1e-6);
}

TEST(UtEntity, VelocityWCSToECI)
{
   UtEntity entity;

   // Set WCS, Get ECI
   UtVec3d velECI;
   entity.ConvertVelocityWCSToECI(cLOC_WCS.GetData(), cVEL_WCS.GetData(), velECI.GetData());

   // Compare Velocities
   EXPECT_NEAR(velECI[0], cVEL_ECI[0], 1e-6);
   EXPECT_NEAR(velECI[1], cVEL_ECI[1], 1e-6);
   EXPECT_NEAR(velECI[2], cVEL_ECI[2], 1e-6);
}
