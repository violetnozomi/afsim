// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtEulerAngles.hpp"

namespace ut
{
namespace EulerAngles
{
//! Compute the classical Euler transform.
//!@param aAlpha A rotation angle about the z axis.
//!@param aBeta  A rotation angle about the intermediate x axis.
//!@param aGamma A rotation angle about the intermediate z axis.
UT_EXPORT UtMat3d ComputeClassicalTransform(double aAlpha, double aBeta, double aGamma)
{
   // Compute the Euler rotation matrix.
   double sinA = sin(aAlpha);
   double cosA = cos(aAlpha);
   double sinB = sin(aBeta);
   double cosB = cos(aBeta);
   double sinG = sin(aGamma);
   double cosG = cos(aGamma);

   UtMat3d rotationMatrix;
   rotationMatrix[0] = cosG * cosA - sinG * cosB * sinA;
   rotationMatrix[1] = cosG * sinA + sinG * cosB * cosA;
   rotationMatrix[2] = sinG * sinB;
   rotationMatrix[3] = -sinG * cosA - cosG * cosB * sinA;
   rotationMatrix[4] = -sinG * sinA + cosG * cosB * cosA;
   rotationMatrix[5] = cosG * sinB;
   rotationMatrix[6] = sinB * sinA;
   rotationMatrix[7] = -sinB * cosA;
   rotationMatrix[8] = cosB;

   return rotationMatrix;
}
}; // namespace EulerAngles

} // namespace ut
