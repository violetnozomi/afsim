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
#include "WsfSixDOF_Utils.hpp"

TEST(WsfSixDOF_Utils, AlphaBetaNonZero)
{
   UtVec3dX vec(0.0, 0.0, 0.0);
   double   alpha_deg = 0;
   double   beta_deg  = 0;

   double alpha = 0;
   double beta  = 0;

   alpha = UtMath::cPI_OVER_4 / 2;
   beta  = UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = -UtMath::cPI_OVER_4 / 2;
   beta  = UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = UtMath::cPI_OVER_4 / 2;
   beta  = -UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = -UtMath::cPI_OVER_4 / 2;
   beta  = -UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);
}

TEST(WsfSixDOF_Utils, AlphaBetaZero)
{
   UtVec3dX vec(0.0, 0.0, 0.0);
   double   alpha_deg = 0;
   double   beta_deg  = 0;

   double alpha = 0;
   double beta  = 0;

   alpha = 0;
   beta  = UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = UtMath::cPI_OVER_4 / 2;
   beta  = 0;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = 0;
   beta  = 0;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);
}

TEST(WsfSixDOF_Utils, AlphaBeta90)
{
   UtVec3dX vec(0.0, 0.0, 0.0);
   double   alpha_deg = 0;
   double   beta_deg  = 0;

   double alpha = 0;
   double beta  = 0;

   alpha = UtMath::cPI_OVER_2;
   beta  = UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = -UtMath::cPI_OVER_2;
   beta  = UtMath::cPI_OVER_2 / 3;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, alpha * UtMath::cDEG_PER_RAD, 1E-12);

   alpha = UtMath::cPI_OVER_4 / 2;
   beta  = UtMath::cPI_OVER_2;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, 90, 1E-12);

   alpha = UtMath::cPI_OVER_4 / 2;
   beta  = -UtMath::cPI_OVER_2;
   vec   = UtVec3dX(std::cos(alpha) * std::cos(beta), std::sin(beta), std::sin(alpha) * std::cos(beta));
   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha_deg, beta_deg);
   EXPECT_NEAR(beta_deg, beta * UtMath::cDEG_PER_RAD, 1E-12);
   EXPECT_NEAR(alpha_deg, -90, 1E-12);
}

TEST(WsfSixDOF_Utils, VelocityZero)
{
   UtVec3dX vec(0.0, 0.0, 0.0);
   double   alpha = 0.0;
   double   beta  = 0.0;

   wsf::six_dof::utils::AlphaBetaFromBodyRelVel(vec, alpha, beta);

   EXPECT_NEAR(beta, 0, 1E-12);
   EXPECT_NEAR(alpha, 0, 1E-12);
}
