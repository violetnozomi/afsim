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

#include "UtDCM.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3dX.hpp"

// Convert a quaternion to a DCM and make sure they are equivalent
TEST(UtDCM, QuaternionEquivalence)
{
   // Angular tolerance for test values
   const double cDCM_FUZZ = 0.0001;

   UtDCM        threeD_TestRot(10 * UtMath::cRAD_PER_DEG, 20 * UtMath::cRAD_PER_DEG, -40 * UtMath::cRAD_PER_DEG);
   UtQuaternion quatD_TestRot(10 * UtMath::cRAD_PER_DEG, 20 * UtMath::cRAD_PER_DEG, -40 * UtMath::cRAD_PER_DEG);

   double quatDCM[3][3];
   quatD_TestRot.Get(quatDCM);

   bool bothDCMsMatch = true;
   for (size_t i = 0; i != 3; ++i)
   {
      for (size_t j = 0; j != 3; ++j)
      {
         if (fabs(quatDCM[i][j] - threeD_TestRot[i][j]) > cDCM_FUZZ)
         {
            bothDCMsMatch = false;
         }
      }
   }
   EXPECT_TRUE(bothDCMsMatch);
}

// Test to assure IsValid works:
TEST(UtDCM, IsValid)
{
   // A) Correctly pass a good matrix.
   UtDCM arbitraryOrient(20.0 * UtMath::cRAD_PER_DEG, 30.0 * UtMath::cRAD_PER_DEG, 10.0 * UtMath::cRAD_PER_DEG);
   EXPECT_TRUE(arbitraryOrient.IsValid());

   // B) Correctly fail a bad matrix.
   double junkMatrix[3][3];
   junkMatrix[0][0] = 0.345;
   junkMatrix[0][1] = 0.125;
   junkMatrix[0][2] = -0.345;
   junkMatrix[1][0] = -0.3;
   junkMatrix[1][1] = -0.125;
   junkMatrix[1][2] = 0.9;
   junkMatrix[2][0] = 0.0;
   junkMatrix[2][1] = 0.111;
   junkMatrix[2][2] = 0.0;
   arbitraryOrient.Set(junkMatrix);

   EXPECT_FALSE(arbitraryOrient.IsValid());
}

TEST(UtDCM, GetRowIdentity)
{
   // Tests to assure that GetRow function returns correct vectors:
   UtDCM unitVectorTest = UtDCM::cIDENTITY;

   UtVec3dX result;
   UtVec3dX rowVec0(1.0, 0.0, 0.0);
   UtVec3dX rowVec1(0.0, 1.0, 0.0);
   UtVec3dX rowVec2(0.0, 0.0, 1.0);

   // An identity matrix should return GetRow(0) = (1, 0, 0)
   unitVectorTest.GetRow(0, result.GetData());

   EXPECT_EQ(result[0], rowVec0[0]);
   EXPECT_EQ(result[1], rowVec0[1]);
   EXPECT_EQ(result[2], rowVec0[2]);

   result = unitVectorTest.GetRow(0);

   EXPECT_EQ(result[0], rowVec0[0]);
   EXPECT_EQ(result[1], rowVec0[1]);
   EXPECT_EQ(result[2], rowVec0[2]);

   // An identity matrix should return GetRow(1) = (0, 1, 0)
   unitVectorTest.GetRow(1, result.GetData());

   EXPECT_EQ(result[0], rowVec1[0]);
   EXPECT_EQ(result[1], rowVec1[1]);
   EXPECT_EQ(result[2], rowVec1[2]);

   result = unitVectorTest.GetRow(1);

   EXPECT_EQ(result[0], rowVec1[0]);
   EXPECT_EQ(result[1], rowVec1[1]);
   EXPECT_EQ(result[2], rowVec1[2]);

   // An identity matrix should return GetRow(2) = (0, 0, 1)
   unitVectorTest.GetRow(2, result.GetData());

   EXPECT_EQ(result[0], rowVec2[0]);
   EXPECT_EQ(result[1], rowVec2[1]);
   EXPECT_EQ(result[2], rowVec2[2]);

   result = unitVectorTest.GetRow(2);

   EXPECT_EQ(result[0], rowVec2[0]);
   EXPECT_EQ(result[1], rowVec2[1]);
   EXPECT_EQ(result[2], rowVec2[2]);
}

TEST(UtDCM, GetRow)
{
   double psi   = UtMath::cPI / 6;  // yaw
   double theta = -UtMath::cPI / 4; // pitch
   double phi   = UtMath::cPI / 8;  // roll

   UtDCM dcm(psi, theta, phi);

   UtVec3dX rowVec0(std::cos(theta) * std::cos(psi), std::cos(theta) * std::sin(psi), -std::sin(theta));

   UtVec3dX rowVec1(-std::cos(phi) * std::sin(psi) + std::sin(phi) * std::sin(theta) * std::cos(psi),
                    std::cos(phi) * std::cos(psi) + std::sin(phi) * std::sin(theta) * std::sin(psi),
                    std::sin(phi) * std::cos(theta));

   UtVec3dX rowVec2(std::sin(phi) * std::sin(psi) + std::cos(phi) * std::sin(theta) * std::cos(psi),
                    -std::sin(phi) * std::cos(psi) + std::cos(phi) * std::sin(theta) * std::sin(psi),
                    std::cos(phi) * std::cos(theta));

   UtVec3dX result;

   // --------- Row 0 --------- //
   dcm.GetRow(0, result.GetData());

   EXPECT_EQ(result[0], rowVec0[0]);
   EXPECT_EQ(result[1], rowVec0[1]);
   EXPECT_EQ(result[2], rowVec0[2]);

   result = dcm.GetRow(0);

   EXPECT_EQ(result[0], rowVec0[0]);
   EXPECT_EQ(result[1], rowVec0[1]);
   EXPECT_EQ(result[2], rowVec0[2]);


   // --------- Row 1 --------- //
   dcm.GetRow(1, result.GetData());

   EXPECT_EQ(result[0], rowVec1[0]);
   EXPECT_EQ(result[1], rowVec1[1]);
   EXPECT_EQ(result[2], rowVec1[2]);

   result = dcm.GetRow(1);

   EXPECT_EQ(result[0], rowVec1[0]);
   EXPECT_EQ(result[1], rowVec1[1]);
   EXPECT_EQ(result[2], rowVec1[2]);

   // --------- Row 2 --------- //
   dcm.GetRow(2, result.GetData());

   EXPECT_EQ(result[0], rowVec2[0]);
   EXPECT_EQ(result[1], rowVec2[1]);
   EXPECT_EQ(result[2], rowVec2[2]);

   result = dcm.GetRow(2);

   EXPECT_EQ(result[0], rowVec2[0]);
   EXPECT_EQ(result[1], rowVec2[1]);
   EXPECT_EQ(result[2], rowVec2[2]);
}

TEST(UtDCM, GetColIdentity)
{
   // Tests to assure that GetRow function returns correct vectors:
   UtDCM unitVectorTest = UtDCM::cIDENTITY;

   UtVec3dX result;
   UtVec3dX colVec0(1.0, 0.0, 0.0);
   UtVec3dX colVec1(0.0, 1.0, 0.0);
   UtVec3dX colVec2(0.0, 0.0, 1.0);

   // An identity matrix should return GetRow(0) = (1, 0, 0)
   unitVectorTest.GetCol(0, result.GetData());

   EXPECT_EQ(result[0], colVec0[0]);
   EXPECT_EQ(result[1], colVec0[1]);
   EXPECT_EQ(result[2], colVec0[2]);

   result = unitVectorTest.GetCol(0);

   EXPECT_EQ(result[0], colVec0[0]);
   EXPECT_EQ(result[1], colVec0[1]);
   EXPECT_EQ(result[2], colVec0[2]);

   // An identity matrix should return GetRow(1) = (0, 1, 0)
   unitVectorTest.GetCol(1, result.GetData());

   EXPECT_EQ(result[0], colVec1[0]);
   EXPECT_EQ(result[1], colVec1[1]);
   EXPECT_EQ(result[2], colVec1[2]);

   result = unitVectorTest.GetCol(1);

   EXPECT_EQ(result[0], colVec1[0]);
   EXPECT_EQ(result[1], colVec1[1]);
   EXPECT_EQ(result[2], colVec1[2]);

   // An identity matrix should return GetRow(2) = (0, 0, 1)
   unitVectorTest.GetCol(2, result.GetData());

   EXPECT_EQ(result[0], colVec2[0]);
   EXPECT_EQ(result[1], colVec2[1]);
   EXPECT_EQ(result[2], colVec2[2]);

   result = unitVectorTest.GetCol(2);

   EXPECT_EQ(result[0], colVec2[0]);
   EXPECT_EQ(result[1], colVec2[1]);
   EXPECT_EQ(result[2], colVec2[2]);
}

TEST(UtDCM, GetCol)
{
   double psi   = UtMath::cPI / 6;  // yaw
   double theta = -UtMath::cPI / 4; // pitch
   double phi   = UtMath::cPI / 8;  // roll

   UtDCM dcm(psi, theta, phi);

   UtVec3dX colVec0(std::cos(theta) * std::cos(psi),
                    -std::cos(phi) * std::sin(psi) + std::sin(phi) * std::sin(theta) * std::cos(psi),
                    std::sin(phi) * std::sin(psi) + std::cos(phi) * std::sin(theta) * std::cos(psi));

   UtVec3dX colVec1(std::cos(theta) * std::sin(psi),
                    std::cos(phi) * std::cos(psi) + std::sin(phi) * std::sin(theta) * std::sin(psi),
                    -std::sin(phi) * std::cos(psi) + std::cos(phi) * std::sin(theta) * std::sin(psi));

   UtVec3dX colVec2(-std::sin(theta), std::sin(phi) * std::cos(theta), std::cos(phi) * std::cos(theta));

   UtVec3dX result;

   // --------- Col 0 --------- //
   dcm.GetCol(0, result.GetData());

   EXPECT_EQ(result[0], colVec0[0]);
   EXPECT_EQ(result[1], colVec0[1]);
   EXPECT_EQ(result[2], colVec0[2]);

   result = dcm.GetCol(0);

   EXPECT_EQ(result[0], colVec0[0]);
   EXPECT_EQ(result[1], colVec0[1]);
   EXPECT_EQ(result[2], colVec0[2]);


   // --------- Col 1 --------- //
   dcm.GetCol(1, result.GetData());

   EXPECT_EQ(result[0], colVec1[0]);
   EXPECT_EQ(result[1], colVec1[1]);
   EXPECT_EQ(result[2], colVec1[2]);

   result = dcm.GetCol(1);

   EXPECT_EQ(result[0], colVec1[0]);
   EXPECT_EQ(result[1], colVec1[1]);
   EXPECT_EQ(result[2], colVec1[2]);

   // --------- Col 2 --------- //
   dcm.GetCol(2, result.GetData());

   EXPECT_EQ(result[0], colVec2[0]);
   EXPECT_EQ(result[1], colVec2[1]);
   EXPECT_EQ(result[2], colVec2[2]);

   result = dcm.GetCol(2);

   EXPECT_EQ(result[0], colVec2[0]);
   EXPECT_EQ(result[1], colVec2[1]);
   EXPECT_EQ(result[2], colVec2[2]);
}

TEST(UtDCM, Get)
{
   double psi   = UtMath::cPI / 6;  // yaw
   double theta = -UtMath::cPI / 4; // pitch
   double phi   = UtMath::cPI / 8;  // roll

   UtDCM dcm(psi, theta, phi);

   double psi_result   = 0.0;
   double theta_result = 0.0;
   double phi_result   = 0.0;

   dcm.Get(psi_result, theta_result, phi_result);

   EXPECT_NEAR(psi, psi_result, 1E-12);
   EXPECT_NEAR(theta, theta_result, 1E-12);
   EXPECT_NEAR(phi, phi_result, 1E-12);
}
