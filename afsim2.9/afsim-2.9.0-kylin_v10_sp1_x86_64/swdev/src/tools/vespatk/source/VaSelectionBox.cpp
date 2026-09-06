// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaSelectionBox.hpp"

#include <math.h>

#include "UtEllipsoidalEarth.hpp"

namespace vespa
{
// The input coordinates must define a geodetically convex region, with no three points co-linear
VaSelectionBox::VaSelectionBox(const double aLat1,
                               const double aLon1,
                               const double aLat2,
                               const double aLon2,
                               const double aLat3,
                               const double aLon3,
                               const double aLat4,
                               const double aLon4)
{
   mLLA1 = UtVec3d(aLat1, aLon1, 0.0);
   mLLA2 = UtVec3d(aLat2, aLon2, 0.0);
   mLLA3 = UtVec3d(aLat3, aLon3, 0.0);
   mLLA4 = UtVec3d(aLat4, aLon4, 0.0);

   double ecef1[3];
   double ecef2[3];
   double ecef3[3];
   double ecef4[3];
   double invLength;

   UtEllipsoidalEarth::ConvertLLAToECEF(aLat1, aLon1, 0.0, ecef1);
   UtEllipsoidalEarth::ConvertLLAToECEF(aLat2, aLon2, 0.0, ecef2);
   UtEllipsoidalEarth::ConvertLLAToECEF(aLat3, aLon3, 0.0, ecef3);
   UtEllipsoidalEarth::ConvertLLAToECEF(aLat4, aLon4, 0.0, ecef4);

   UtVec3d centerECEF;
   centerECEF.Add(ecef1);
   centerECEF.Add(ecef2);
   centerECEF.Add(ecef3);
   centerECEF.Add(ecef4);
   centerECEF /= 4.0;
   UtEllipsoidalEarth::ConvertECEFToLLA(centerECEF.GetData(), mCenterLLA[0], mCenterLLA[1], mCenterLLA[2]);

   mPlane1[0] = ecef1[1] * ecef2[2] - ecef2[1] * ecef1[2];
   mPlane1[1] = ecef1[2] * ecef2[0] - ecef2[2] * ecef1[0];
   mPlane1[2] = ecef1[0] * ecef2[1] - ecef2[0] * ecef1[1];
   invLength  = 1 / sqrt(mPlane1[0] * mPlane1[0] + mPlane1[1] * mPlane1[1] + mPlane1[2] * mPlane1[2]);
   mPlane1[0] *= invLength;
   mPlane1[1] *= invLength;
   mPlane1[2] *= invLength;
   if (mPlane1[0] * ecef3[0] + mPlane1[1] * ecef3[1] + mPlane1[2] * ecef3[2] < 0) // lets define positive as inside
   {
      mPlane1[0] *= -1;
      mPlane1[1] *= -1;
      mPlane1[2] *= -1;
   }

   mPlane2[0] = ecef2[1] * ecef3[2] - ecef3[1] * ecef2[2];
   mPlane2[1] = ecef2[2] * ecef3[0] - ecef3[2] * ecef2[0];
   mPlane2[2] = ecef2[0] * ecef3[1] - ecef3[0] * ecef2[1];
   invLength  = 1 / sqrt(mPlane2[0] * mPlane2[0] + mPlane2[1] * mPlane2[1] + mPlane2[2] * mPlane2[2]);
   mPlane2[0] *= invLength;
   mPlane2[1] *= invLength;
   mPlane2[2] *= invLength;
   if (mPlane2[0] * ecef4[0] + mPlane2[1] * ecef4[1] + mPlane2[2] * ecef4[2] < 0) // lets define positive as inside
   {
      mPlane2[0] *= -1;
      mPlane2[1] *= -1;
      mPlane2[2] *= -1;
   }

   mPlane3[0] = ecef3[1] * ecef4[2] - ecef4[1] * ecef3[2];
   mPlane3[1] = ecef3[2] * ecef4[0] - ecef4[2] * ecef3[0];
   mPlane3[2] = ecef3[0] * ecef4[1] - ecef4[0] * ecef3[1];
   invLength  = 1 / sqrt(mPlane3[0] * mPlane3[0] + mPlane3[1] * mPlane3[1] + mPlane3[2] * mPlane3[2]);
   mPlane3[0] *= invLength;
   mPlane3[1] *= invLength;
   mPlane3[2] *= invLength;
   if (mPlane3[0] * ecef1[0] + mPlane3[1] * ecef1[1] + mPlane3[2] * ecef1[2] < 0) // lets define positive as inside
   {
      mPlane3[0] *= -1;
      mPlane3[1] *= -1;
      mPlane3[2] *= -1;
   }

   mPlane4[0] = ecef4[1] * ecef1[2] - ecef1[1] * ecef4[2];
   mPlane4[1] = ecef4[2] * ecef1[0] - ecef1[2] * ecef4[0];
   mPlane4[2] = ecef4[0] * ecef1[1] - ecef1[0] * ecef4[1];
   invLength  = 1 / sqrt(mPlane4[0] * mPlane4[0] + mPlane4[1] * mPlane4[1] + mPlane4[2] * mPlane4[2]);
   mPlane4[0] *= invLength;
   mPlane4[1] *= invLength;
   mPlane4[2] *= invLength;
   if (mPlane4[0] * ecef2[0] + mPlane4[1] * ecef2[1] + mPlane4[2] * ecef2[2] < 0) // lets define positive as inside
   {
      mPlane4[0] *= -1;
      mPlane4[1] *= -1;
      mPlane4[2] *= -1;
   }
}

bool VaSelectionBox::Contains(const double aLat, const double aLon, const double aAlt) const
{
   double ecef[3];

   UtEllipsoidalEarth::ConvertLLAToECEF(aLat, aLon, aAlt, ecef);

   bool inside = true;

   if (mPlane1[0] * ecef[0] + mPlane1[1] * ecef[1] + mPlane1[2] * ecef[2] < 0)
   {
      inside = false;
   }
   if (mPlane2[0] * ecef[0] + mPlane2[1] * ecef[1] + mPlane2[2] * ecef[2] < 0)
   {
      inside = false;
   }
   if (mPlane3[0] * ecef[0] + mPlane3[1] * ecef[1] + mPlane3[2] * ecef[2] < 0)
   {
      inside = false;
   }
   if (mPlane4[0] * ecef[0] + mPlane4[1] * ecef[1] + mPlane4[2] * ecef[2] < 0)
   {
      inside = false;
   }
   return inside;
}

VaSelectionBox::~VaSelectionBox() {}
} // namespace vespa
