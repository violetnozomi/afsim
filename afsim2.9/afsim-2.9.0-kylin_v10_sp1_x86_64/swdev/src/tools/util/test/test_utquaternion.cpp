// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtDCM.hpp"
#include "UtMat3.hpp"
#include "UtQuaternion.hpp"

namespace
{
// BodyRates are: RollRate, PitchRate, YawRate, in that order.
// magnitude = ONE degree per second
constexpr double   cONE_DEGREE_PER_SECOND = UtMath::cRAD_PER_DEG;
UtVec3d            bodyRates{cONE_DEGREE_PER_SECOND, 0.0, 0.0};
const UtQuaternion original{0.0, 0.0, 0.0};

void CompareVectors(const UtVec3d& aVec1, const UtVec3d& aVec2)
{
   for (auto i : {0, 1, 2})
   {
      EXPECT_NEAR(aVec1[i], aVec2[i], 0.00001);
   }
}
} // namespace

// Most key functionality of UtQuaternion is embedded in the Update and
// Propagate functions, so these tests exercise each...
TEST(UtQuaternion, LargeScalePropagate)
{
   UtQuaternion attitude{original.Propagate(bodyRates.GetData(), 90.0)};

   double phi, theta, psi;
   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 0.0, 0.001);
   EXPECT_NEAR(theta, 0.0, 0.001);
   EXPECT_NEAR(phi, UtMath::cPI_OVER_2, 0.001);

   attitude = original.Propagate(bodyRates.GetData(), 180.0);
   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 0.0, 0.001);
   EXPECT_NEAR(theta, 0.0, 0.001);
   EXPECT_NEAR(phi, UtMath::cPI, 0.001);

   bodyRates.Set(0.0, 0.0, cONE_DEGREE_PER_SECOND);

   attitude = original.Propagate(bodyRates.GetData(), 3.0);
   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 3.0 * UtMath::cRAD_PER_DEG, 0.001);
   EXPECT_NEAR(theta, 0.0, 0.001);
   EXPECT_NEAR(phi, 0.0, 0.001);

   // Now for a 3-D test.  Do a 90 degree pitch up, with a 90-degree roll.
   bodyRates.Set(cONE_DEGREE_PER_SECOND, cONE_DEGREE_PER_SECOND, 0.0);

   attitude = original.Propagate(bodyRates.GetData(), 90.0);
   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 76.1227 * UtMath::cRAD_PER_DEG, 0.001);
   EXPECT_NEAR(theta, 34.725 * UtMath::cRAD_PER_DEG, 0.001);
   EXPECT_NEAR(phi, 137.465 * UtMath::cRAD_PER_DEG, 0.001);
}

TEST(UtQuaternion, SmallScalePropagate)
{
   constexpr size_t normModulus = 100;

   bodyRates.Set(cONE_DEGREE_PER_SECOND, 0.0, 0.0);

   UtQuaternion attitude{original};
   double       totalSeconds = 90.0;
   double       dt           = 0.25;
   auto         timeSteps    = static_cast<size_t>(totalSeconds / dt);
   for (size_t i = 0; i != timeSteps; ++i)
   {
      attitude = attitude.Rotate(bodyRates.GetData(), dt);
      if (i % normModulus == 0)
      {
         attitude.Normalize();
      }
   }

   double psi, theta, phi;
   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 0.0, 0.01);
   EXPECT_NEAR(theta, 0.0, 0.01);
   EXPECT_NEAR(phi, UtMath::cPI_OVER_2, 0.01);

   totalSeconds = 180.0;
   timeSteps    = static_cast<size_t>(totalSeconds / dt);
   attitude     = original;
   for (size_t i = 0; i != timeSteps; ++i)
   {
      attitude = attitude.Rotate(bodyRates.GetData(), dt);
      if (i % normModulus == 0)
      {
         attitude.Normalize();
      }
   }
   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 0.0, 0.001);
   EXPECT_NEAR(theta, 0.0, 0.001);
   EXPECT_NEAR(phi, UtMath::cPI, 0.001);

   bodyRates.Set(0.0, 0.0, cONE_DEGREE_PER_SECOND);

   totalSeconds = 3.0;
   timeSteps    = static_cast<size_t>(totalSeconds / dt);
   attitude     = original;
   for (size_t i = 0; i != timeSteps; ++i)
   {
      attitude = attitude.Rotate(bodyRates.GetData(), dt);
      if (i % normModulus == 0)
      {
         attitude.Normalize();
      }
   }

   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 3.0 * UtMath::cRAD_PER_DEG, 0.001);
   EXPECT_NEAR(theta, 0.0, 0.001);
   EXPECT_NEAR(phi, 0.0, 0.001);

   // Now for a 3-D test.  Do a 90 degree pitch up, with a 90-degree roll.
   bodyRates.Set(cONE_DEGREE_PER_SECOND, cONE_DEGREE_PER_SECOND, 0.0);

   totalSeconds = 90.0;
   dt           = 1.0 / 400.; // 400 Hz.
   timeSteps    = static_cast<size_t>(totalSeconds / dt);
   attitude     = original;
   for (size_t i = 0; i != timeSteps; ++i)
   {
      attitude = attitude.Rotate(bodyRates.GetData(), dt);
      if (i % normModulus == 0)
      {
         attitude.Normalize();
      }
   }

   attitude.Get(psi, theta, phi);

   EXPECT_NEAR(psi, 76.20 * UtMath::cRAD_PER_DEG, 0.01);
   EXPECT_NEAR(theta, 34.24 * UtMath::cRAD_PER_DEG, 0.01);
   EXPECT_NEAR(phi, 137.11 * UtMath::cRAD_PER_DEG, 0.01);
}

TEST(UtQuaternion, RotateReverse)
{
   double psi   = 3 * UtMath::cRAD_PER_DEG;
   double theta = 25 * UtMath::cRAD_PER_DEG;
   double phi   = 160 * UtMath::cRAD_PER_DEG;

   const UtQuaternion tripleTurn1(psi, theta, phi);
   const UtDCM        tripleTurn2(psi, theta, phi);

   UtDCM dcmEquiv;
   tripleTurn1.Get(dcmEquiv);

   EXPECT_EQ(dcmEquiv, tripleTurn2);

   UtVec3d  randomVect{2.5, 5.6, -8.0};
   UtVec3dX randomVectUV(2.5, 5.6, -8.0);

   UtVec3d resultVect1;
   tripleTurn1.Rotate(randomVect.GetData(), resultVect1.GetData());

   UtVec3dX resultVect2 = tripleTurn2 * randomVectUV;

   CompareVectors(resultVect1, resultVect2);

   tripleTurn1.ReverseRotate(randomVect.GetData(), resultVect1.GetData());
   resultVect2 = tripleTurn2.InverseTransform(randomVectUV);

   CompareVectors(resultVect1, resultVect2);
}

TEST(UtQuaternion, AngleVecConstructVecRotate)
{
   UtVec3d startAttitudeVec(1, 0, 0);

   // Rotate 90 degrees around X
   UtQuaternion qPlus90X(UtMath::cPI_OVER_2, startAttitudeVec.GetData());

   UtVec3d a(0, 1, 0);
   UtVec3d out1;
   qPlus90X.Rotate(a.GetData(), out1.GetData());

   // Set rotation to 90 deg around X (phi) using Euler angles:
   qPlus90X.Set(0, 0, UtMath::cPI_OVER_2);
   UtVec3d out2;
   qPlus90X.Rotate(a.GetData(), out2.GetData());

   CompareVectors(out1, out2);

   qPlus90X.ReverseRotate(a.GetData(), out1.GetData());

   // Output DCM, and use Transform to rotate vector:
   double DCM[3][3];
   qPlus90X.Get(DCM);

   UtMat3d::InverseTransform(out2.GetData(), DCM, a.GetData());

   CompareVectors(out1, out2);

   qPlus90X.ReverseRotate(a.GetData(), out1.GetData());

   CompareVectors(out1, out2);

   // ---- Test UtVec3dX rotate and reverse rotate: ----
   UtVec3dX aa(0.0, 1.0, 0.0);
   UtVec3dX outaa1{qPlus90X.Transform(aa)};
   UtVec3dX outab1{qPlus90X * aa};
   UtVec3dX outaa2{qPlus90X.InverseTransform(aa)};

   CompareVectors(outaa1, UtVec3d{0.0, 0.0, -1.0});
   CompareVectors(outab1, UtVec3d{0.0, 0.0, -1.0});
   CompareVectors(outaa2, UtVec3d{0.0, 0.0, 1.0});
}

TEST(UtQuaternion, SetFromUtDCM)
{
   double       psiX   = 0.0;
   double       thetaX = 90.0 * UtMath::cRAD_PER_DEG;
   double       phiX   = 0.0;
   UtDCM        dcmPitch90(psiX, thetaX, phiX);
   UtQuaternion quatPitch90(dcmPitch90);

   UtDCM        dcmOtherRotation(20.0 * UtMath::cRAD_PER_DEG, 30.0 * UtMath::cRAD_PER_DEG, 40.0 * UtMath::cRAD_PER_DEG);
   UtQuaternion quatOtherRotation(dcmOtherRotation);

   // Re-orient the quaternion to a new value:
   quatOtherRotation.Set(dcmPitch90);
   double psiZ, thetaZ, phiZ;
   quatOtherRotation.Get(psiZ, thetaZ, phiZ);

   EXPECT_NEAR(psiZ, psiX, 0.00001);
   EXPECT_NEAR(thetaZ, thetaX, 0.00001);
   EXPECT_NEAR(phiZ, phiX, 0.00001);
}
