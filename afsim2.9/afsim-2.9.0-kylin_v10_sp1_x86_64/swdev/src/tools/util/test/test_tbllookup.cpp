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
#include <math.h>

#include <gtest/gtest.h>

#include "TblLookup.hpp"

// Uneven
TEST(TblLookup, Uneven)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupU<double> luU;
   luU.Lookup(x, -4.5);
   EXPECT_EQ(luU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luU.GetRatio()) < cTOL);
   luU.Lookup(x, -1.0);
   EXPECT_EQ(luU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luU.GetRatio()) < cTOL);
   luU.Lookup(x, 1.0);
   EXPECT_EQ(luU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luU.GetRatio()) < cTOL);
   luU.Lookup(x, 2.0);
   EXPECT_EQ(luU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luU.GetRatio() - 1.0) < cTOL);
   luU.Lookup(x, 3.0);
   EXPECT_EQ(luU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luU.GetRatio() - 1.0) < cTOL);
   luU.Lookup(x, 4.5);
   EXPECT_EQ(luU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luU.GetRatio() - 1.0) < cTOL);
}

TEST(TblLookup, UnevenBinary)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupUB<double> luUB;
   luUB.Lookup(x, -4.5);
   EXPECT_EQ(luUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUB.GetRatio()) < cTOL);
   luUB.Lookup(x, -1.0);
   EXPECT_EQ(luUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUB.GetRatio()) < cTOL);
   luUB.Lookup(x, 1.0);
   EXPECT_EQ(luUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUB.GetRatio()) < cTOL);
   luUB.Lookup(x, 2.0);
   EXPECT_EQ(luUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUB.GetRatio() - 1.0) < cTOL);
   luUB.Lookup(x, 3.0);
   EXPECT_EQ(luUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUB.GetRatio() - 1.0) < cTOL);
   luUB.Lookup(x, 4.5);
   EXPECT_EQ(luUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUB.GetRatio() - 1.0) < cTOL);
}

TEST(TblLookup, LinearUneven)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupLU<double> luLU;
   luLU.Lookup(x, -4.5);
   EXPECT_EQ(luLU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLU.GetRatio()) < cTOL);
   luLU.Lookup(x, -1.0);
   EXPECT_EQ(luLU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLU.GetRatio()) < cTOL);
   luLU.Lookup(x, 1.0);
   EXPECT_EQ(luLU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLU.GetRatio() - 0.5) < cTOL);
   luLU.Lookup(x, 2.0);
   EXPECT_EQ(luLU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLU.GetRatio() - 1.0) < cTOL);
   luLU.Lookup(x, 3.0);
   EXPECT_EQ(luLU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLU.GetRatio() - 1.0) < cTOL);
   luLU.Lookup(x, 4.5);
   EXPECT_EQ(luLU.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLU.GetRatio() - 1.0) < cTOL);
}

TEST(TblLookup, LinearUnevenBinary)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupLUB<double> luLUB;
   luLUB.Lookup(x, -4.5);
   EXPECT_EQ(luLUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUB.GetRatio()) < cTOL);
   luLUB.Lookup(x, -1.0);
   EXPECT_EQ(luLUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUB.GetRatio()) < cTOL);
   luLUB.Lookup(x, 1.0);
   EXPECT_EQ(luLUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUB.GetRatio() - 0.5) < cTOL);
   luLUB.Lookup(x, 2.0);
   EXPECT_EQ(luLUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUB.GetRatio() - 1.0) < cTOL);
   luLUB.Lookup(x, 3.0);
   EXPECT_EQ(luLUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUB.GetRatio() - 1.0) < cTOL);
   luLUB.Lookup(x, 4.5);
   EXPECT_EQ(luLUB.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUB.GetRatio() - 1.0) < cTOL);
}

TEST(TblLookup, UnevenExtrapolate)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupUX<double> luUX;
   luUX.Lookup(x, -4.5);
   EXPECT_EQ(luUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUX.GetRatio() + 2.0) < cTOL);
   luUX.Lookup(x, -1.0);
   EXPECT_EQ(luUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUX.GetRatio()) < cTOL);
   luUX.Lookup(x, 1.0);
   EXPECT_EQ(luUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUX.GetRatio()) < cTOL);
   luUX.Lookup(x, 2.0);
   EXPECT_EQ(luUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUX.GetRatio() - 1.0) < cTOL);
   luUX.Lookup(x, 3.0);
   EXPECT_EQ(luUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUX.GetRatio() - 1.0) < cTOL);
   luUX.Lookup(x, 4.5);
   EXPECT_EQ(luUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUX.GetRatio() - 2.0) < cTOL);
}

TEST(TblLookup, UnevenBinaryExtrapolate)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupUBX<double> luUBX;
   luUBX.Lookup(x, -4.5);
   EXPECT_EQ(luUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUBX.GetRatio() + 2.0) < cTOL);
   luUBX.Lookup(x, -1.0);
   EXPECT_EQ(luUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUBX.GetRatio()) < cTOL);
   luUBX.Lookup(x, 1.0);
   EXPECT_EQ(luUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUBX.GetRatio()) < cTOL);
   luUBX.Lookup(x, 2.0);
   EXPECT_EQ(luUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUBX.GetRatio() - 1.0) < cTOL);
   luUBX.Lookup(x, 3.0);
   EXPECT_EQ(luUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUBX.GetRatio() - 1.0) < cTOL);
   luUBX.Lookup(x, 4.5);
   EXPECT_EQ(luUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luUBX.GetRatio() - 2.0) < cTOL);
}

TEST(TblLookup, LinearUnevenExtrapolate)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupLUX<double> luLUX;
   luLUX.Lookup(x, -4.5);
   EXPECT_EQ(luLUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUX.GetRatio() + 2.25) < cTOL);
   luLUX.Lookup(x, -1.0);
   EXPECT_EQ(luLUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUX.GetRatio() + 0.5) < cTOL);
   luLUX.Lookup(x, 1.0);
   EXPECT_EQ(luLUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUX.GetRatio() - 0.5) < cTOL);
   luLUX.Lookup(x, 2.0);
   EXPECT_EQ(luLUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUX.GetRatio() - 1.0) < cTOL);
   luLUX.Lookup(x, 3.0);
   EXPECT_EQ(luLUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUX.GetRatio() - 1.5) < cTOL);
   luLUX.Lookup(x, 4.5);
   EXPECT_EQ(luLUX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUX.GetRatio() - 2.25) < cTOL);
}

TEST(TblLookup, LinearUnevenBinaryExtrapolate)
{
   TblIndVarU<double> x;
   x.Resize(2);
   x.SetValues({0.0, 2.0});
   static const double cTOL = 1.0e-13;

   TblLookupLUBX<double> luLUBX;
   luLUBX.Lookup(x, -4.5);
   EXPECT_EQ(luLUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUBX.GetRatio() + 2.25) < cTOL);
   luLUBX.Lookup(x, -1.0);
   EXPECT_EQ(luLUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUBX.GetRatio() + 0.5) < cTOL);
   luLUBX.Lookup(x, 1.0);
   EXPECT_EQ(luLUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUBX.GetRatio() - 0.5) < cTOL);
   luLUBX.Lookup(x, 2.0);
   EXPECT_EQ(luLUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUBX.GetRatio() - 1.0) < cTOL);
   luLUBX.Lookup(x, 3.0);
   EXPECT_EQ(luLUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUBX.GetRatio() - 1.5) < cTOL);
   luLUBX.Lookup(x, 4.5);
   EXPECT_EQ(luLUBX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLUBX.GetRatio() - 2.25) < cTOL);
}

// Even
TEST(TblLookup, Even)
{
   TblIndVarE<double> x;
   x.SetValues(0.0, 2.0, 2);
   static const double cTOL = 1.0e-13;

   TblLookupE<double> luE;
   luE.Lookup(x, -4.5);
   EXPECT_EQ(luE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luE.GetRatio()) < cTOL);
   luE.Lookup(x, -1.0);
   EXPECT_EQ(luE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luE.GetRatio()) < cTOL);
   luE.Lookup(x, 1.0);
   EXPECT_EQ(luE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luE.GetRatio()) < cTOL);
   luE.Lookup(x, 2.0);
   EXPECT_EQ(luE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luE.GetRatio() - 1.0) < cTOL);
   luE.Lookup(x, 3.0);
   EXPECT_EQ(luE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luE.GetRatio() - 1.0) < cTOL);
   luE.Lookup(x, 4.5);
   EXPECT_EQ(luE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luE.GetRatio() - 1.0) < cTOL);
}

TEST(TblLookup, LinearEven)
{
   TblIndVarE<double> x;
   x.SetValues(0.0, 2.0, 2);
   static const double cTOL = 1.0e-13;

   TblLookupLE<double> luLE;
   luLE.Lookup(x, -4.5);
   EXPECT_EQ(luLE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLE.GetRatio()) < cTOL);
   luLE.Lookup(x, -1.0);
   EXPECT_EQ(luLE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLE.GetRatio()) < cTOL);
   luLE.Lookup(x, 1.0);
   EXPECT_EQ(luLE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLE.GetRatio() - 0.5) < cTOL);
   luLE.Lookup(x, 2.0);
   EXPECT_EQ(luLE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLE.GetRatio() - 1.0) < cTOL);
   luLE.Lookup(x, 3.0);
   EXPECT_EQ(luLE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLE.GetRatio() - 1.0) < cTOL);
   luLE.Lookup(x, 4.5);
   EXPECT_EQ(luLE.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLE.GetRatio() - 1.0) < cTOL);
}

TEST(TblLookup, EvenExtrapolate)
{
   TblIndVarE<double> x;
   x.SetValues(0.0, 2.0, 2);
   static const double cTOL = 1.0e-13;

   TblLookupEX<double> luEX;
   luEX.Lookup(x, -4.5);
   EXPECT_EQ(luEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luEX.GetRatio() + 2.0) < cTOL);
   luEX.Lookup(x, -1.0);
   EXPECT_EQ(luEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luEX.GetRatio()) < cTOL);
   luEX.Lookup(x, 1.0);
   EXPECT_EQ(luEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luEX.GetRatio()) < cTOL);
   luEX.Lookup(x, 2.0);
   EXPECT_EQ(luEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luEX.GetRatio() - 1.0) < cTOL);
   luEX.Lookup(x, 3.0);
   EXPECT_EQ(luEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luEX.GetRatio() - 1.0) < cTOL);
   luEX.Lookup(x, 4.5);
   EXPECT_EQ(luEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luEX.GetRatio() - 2.0) < cTOL);
}

TEST(TblLookup, LinearEvenExtrapolate)
{
   TblIndVarE<double> x;
   x.SetValues(0.0, 2.0, 2);
   static const double cTOL = 1.0e-13;

   TblLookupLEX<double> luLEX;
   luLEX.Lookup(x, -4.5);
   EXPECT_EQ(luLEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLEX.GetRatio() + 2.25) < cTOL);
   luLEX.Lookup(x, -1.0);
   EXPECT_EQ(luLEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLEX.GetRatio() + 0.5) < cTOL);
   luLEX.Lookup(x, 1.0);
   EXPECT_EQ(luLEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLEX.GetRatio() - 0.5) < cTOL);
   luLEX.Lookup(x, 2.0);
   EXPECT_EQ(luLEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLEX.GetRatio() - 1.0) < cTOL);
   luLEX.Lookup(x, 3.0);
   EXPECT_EQ(luLEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLEX.GetRatio() - 1.5) < cTOL);
   luLEX.Lookup(x, 4.5);
   EXPECT_EQ(luLEX.GetIndex(), 0);
   EXPECT_TRUE(fabs(luLEX.GetRatio() - 2.25) < cTOL);
}
