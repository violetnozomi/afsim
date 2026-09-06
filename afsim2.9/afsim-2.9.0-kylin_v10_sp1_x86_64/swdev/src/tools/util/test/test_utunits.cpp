// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtInput.hpp"
#include "UtUnitTypes.hpp"

TEST(UtUnitTypes, UtUnitAreaDB)
{
   // The value of the dbsm enum in the UtUnitAreaDB class should
   // always be negative to avoid conflicting with base units length
   EXPECT_EQ(UtUnitAreaDB::BaseUnit::cDBSM, -2);

   // Check reading of area inputs from UtInput
   UtInput input;
   double  metersSquared;
   input.PushInputString("20 dbsm");
   input.ReadValueOfType(metersSquared, UtInput::cAREA_DB);
   EXPECT_DOUBLE_EQ(100.0, metersSquared);

   input.PushInputString("100 m^2");
   input.ReadValueOfType(metersSquared, UtInput::cAREA_DB);
   EXPECT_DOUBLE_EQ(100.0, metersSquared);

   // Check conversions between units with dimensions and units without
   double dbsmFromM2 = UtUnitAreaDB::ConvertFromStandard(100000, UtUnitAreaDB::BaseUnit::cDBSM);
   double m2FromDbsm = UtUnitAreaDB::ConvertToStandard(50, UtUnitAreaDB::BaseUnit::cDBSM);
   EXPECT_DOUBLE_EQ(50, dbsmFromM2);
   EXPECT_DOUBLE_EQ(100000, m2FromDbsm);

   // Check that other length units are accepted
   double m2FromKm = UtUnitAreaDB::ConvertToStandard(50, UtUnitLength::BaseUnit::cKILOMETERS);
   double cmFromM2 = UtUnitAreaDB::ConvertFromStandard(50, UtUnitLength::BaseUnit::cCENTIMETERS);
   EXPECT_DOUBLE_EQ(5e+7, m2FromKm);
   EXPECT_DOUBLE_EQ(500000, cmFromM2);
}

TEST(UtUnitTypes, UtUnitAngularInertia)
{
   // Check reading angular inertia from UtInput
   UtInput input;
   double  kgMetersSquared;
   input.PushInputString("100 slug*ft^2");
   input.ReadValueOfType(kgMetersSquared, UtInput::cANGULAR_INERTIA);
   EXPECT_NEAR(135.5817961893, kgMetersSquared, 0.00001);

   input.PushInputString("100 lbm*in^2");
   input.ReadValueOfType(kgMetersSquared, UtInput::cANGULAR_INERTIA);
   EXPECT_NEAR(0.02926396534292, kgMetersSquared, 0.00001);

   input.PushInputString("100 lb*ft^2");
   input.ReadValueOfType(kgMetersSquared, UtInput::cANGULAR_INERTIA);
   EXPECT_NEAR(4.21401100938, kgMetersSquared, 0.00001);

   double slugFt2FromKgm2 = UtUnitAngularInertia::ConvertFromStandard(135.5817961893, UtUnitAngularInertia::cSLUG_FEET2);
   double kgm2Fromslugft2 = UtUnitAngularInertia::ConvertToStandard(100.0, UtUnitAngularInertia::cSLUG_FEET2);
   EXPECT_NEAR(100.0, slugFt2FromKgm2, 0.00001);
   EXPECT_NEAR(135.5817961893, kgm2Fromslugft2, 0.00001);

   double lbmIn2FromKgm2 = UtUnitAngularInertia::ConvertFromStandard(0.02926396534292, UtUnitAngularInertia::cLBM_INCHES2);
   double kgm2FromlbmIn2 = UtUnitAngularInertia::ConvertToStandard(100.0, UtUnitAngularInertia::cLBM_INCHES2);
   EXPECT_NEAR(100.0, lbmIn2FromKgm2, 0.00001);
   EXPECT_NEAR(0.02926396534292, kgm2FromlbmIn2, 0.00001);
}
