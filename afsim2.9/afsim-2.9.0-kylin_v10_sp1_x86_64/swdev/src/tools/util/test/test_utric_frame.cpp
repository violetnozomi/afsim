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

#include "UtRIC_Frame.hpp"

TEST(UtRIC_Frame, CalculateRIC_Values)
{
   UtVec3d pos1(1, 1, 1);
   UtVec3d pos2(2, 2, 2);
   UtVec3d velocity(5, 5, 5);

   // Test same position RIC is (0,0,0)
   UtVec3d ric = ut::RIC_Frame::CalculateToRIC(pos1, velocity, pos1);
   EXPECT_EQ(ric, UtVec3d(0, 0, 0));

   // Test that parallel position diff and velocity results in only r being nonzero
   ric = ut::RIC_Frame::CalculateToRIC(pos1, velocity, pos2);
   EXPECT_NE(ric[0], 0);
   EXPECT_EQ(ric[1], 0);
   EXPECT_EQ(ric[2], 0);

   // Test that r, i, c values are calculated correctly for a nonzero example
   pos2     = UtVec3d(1, 2, 3);
   velocity = UtVec3d(5, 0, 0);
   UtVec3d cvec;
   UtVec3d ivec;
   UtVec3d rvec = pos1;
   cvec.CrossProduct(pos1, velocity);
   ivec.CrossProduct(cvec, pos1);
   cvec.Normalize();
   ivec.Normalize();
   rvec.Normalize();
   ric = ut::RIC_Frame::CalculateToRIC(pos1, velocity, pos2);
   EXPECT_EQ(ric[2], cvec.DotProduct(pos2 - pos1));
   EXPECT_EQ(ric[1], ivec.DotProduct(pos2 - pos1));
   EXPECT_EQ(ric[0], rvec.DotProduct(pos2 - pos1));
}

TEST(UtRIC_Frame, CalculateFromRIC)
{
   constexpr static double cTOLERANCE{1.0e-12};

   UtVec3d rVec{1.0, 0.0, 0.0};
   UtVec3d iVec{0.0, 1.0, 0.0};
   UtVec3d cVec{0.0, 0.0, 1.0};

   for (int i = 0; i < 3; ++i)
   {
      UtVec3d objPos{0.0, 0.0, 0.0};
      UtVec3d objVel{0.0, 0.0, 0.0};
      objPos[i] = 1.0;
      int j     = (i + 1) % 3;
      objVel[j] = 1.0;

      // With these two positions and velocities
      //   R is the ith direction
      //   I is the (i+1)th direction
      //   C is the (i+2)th direction
      // where we wrap those around as needed.
      int     rIdx = i;
      int     iIdx = (i + 1) % 3;
      int     cIdx = (i + 2) % 3;
      UtVec3d expR = objPos;
      expR[rIdx] += 1.0;
      UtVec3d expI = objPos;
      expI[iIdx] += 1.0;
      UtVec3d expC = objPos;
      expC[cIdx] += 1.0;

      UtVec3d offR = ut::RIC_Frame::CalculateFromRIC(objPos, objVel, rVec);
      UtVec3d offI = ut::RIC_Frame::CalculateFromRIC(objPos, objVel, iVec);
      UtVec3d offC = ut::RIC_Frame::CalculateFromRIC(objPos, objVel, cVec);

      for (int t = 0; t < 3; ++t)
      {
         EXPECT_NEAR(expR[t], offR[t], cTOLERANCE);
         EXPECT_NEAR(expI[t], offI[t], cTOLERANCE);
         EXPECT_NEAR(expC[t], offC[t], cTOLERANCE);
      }
   }
}

TEST(UtRIC_Frame, CalculateToAndFromRIC)
{
   constexpr static double cTOLERANCE{1.0e-12};

   UtVec3d objPos{1.0, 2.0, 3.0};
   UtVec3d objVel{3.0, 2.0, 1.0};

   UtVec3d pos{2.0, 3.0, 4.0};

   UtVec3d ric = ut::RIC_Frame::CalculateToRIC(objPos, objVel, pos);
   UtVec3d xyz = ut::RIC_Frame::CalculateFromRIC(objPos, objVel, ric);

   EXPECT_NEAR(xyz[0], pos[0], cTOLERANCE);
   EXPECT_NEAR(xyz[1], pos[1], cTOLERANCE);
   EXPECT_NEAR(xyz[2], pos[2], cTOLERANCE);
}

TEST(UtRIC_Frame, VelocityFromRIC)
{
   constexpr static double cTOLERANCE{1.0e-12};

   UtVec3d objPos{1.0, 0.0, 0.0};
   UtVec3d objVel{0.0, 1.0, 0.0};
   UtVec3d omega{0.0, 0.0, 1.0}; // = objPos x objVel / ||objPos||^2

   UtVec3d rPos{1.0, 0.0, 0.0};
   UtVec3d iPos{0.0, 1.0, 0.0};
   UtVec3d cPos{0.0, 0.0, 1.0};

   UtVec3d velo{1.0, 2.0, 3.0};

   // With no RIC position offset, should just be a sum
   UtVec3d noPositionOffset = ut::RIC_Frame::VelocityFromRIC(objPos, objVel, UtVec3d{0.0, 0.0, 0.0}, velo);
   EXPECT_NEAR(noPositionOffset[0], objVel[0] + velo[0], cTOLERANCE);
   EXPECT_NEAR(noPositionOffset[1], objVel[1] + velo[1], cTOLERANCE);
   EXPECT_NEAR(noPositionOffset[2], objVel[2] + velo[2], cTOLERANCE);

   // With position offset, picks up omega term
   UtVec3d noVeloOffset = ut::RIC_Frame::VelocityFromRIC(objPos, objVel, rPos, velo);
   EXPECT_NEAR(noVeloOffset[0], objVel[0] + velo[0] + 0.0, cTOLERANCE);
   EXPECT_NEAR(noVeloOffset[1], objVel[1] + velo[1] + 1.0, cTOLERANCE);
   EXPECT_NEAR(noVeloOffset[2], objVel[2] + velo[2] + 0.0, cTOLERANCE);

   noVeloOffset = ut::RIC_Frame::VelocityFromRIC(objPos, objVel, iPos, velo);
   EXPECT_NEAR(noVeloOffset[0], objVel[0] + velo[0] - 1.0, cTOLERANCE);
   EXPECT_NEAR(noVeloOffset[1], objVel[1] + velo[1] + 0.0, cTOLERANCE);
   EXPECT_NEAR(noVeloOffset[2], objVel[2] + velo[2] + 0.0, cTOLERANCE);

   noVeloOffset = ut::RIC_Frame::VelocityFromRIC(objPos, objVel, cPos, velo);
   EXPECT_NEAR(noVeloOffset[0], objVel[0] + velo[0] + 0.0, cTOLERANCE);
   EXPECT_NEAR(noVeloOffset[1], objVel[1] + velo[1] + 0.0, cTOLERANCE);
   EXPECT_NEAR(noVeloOffset[2], objVel[2] + velo[2] + 0.0, cTOLERANCE);
}
