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

#include "UtRelativeGeometry.hpp"

#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

double UtRelativeGeometry::CalculateLinearRange(const UtEntity& aFrom, const UtEntity& aTo)
{
   double fxyz[3];
   double txyz[3];
   aFrom.GetLocationWCS(fxyz);
   aTo.GetLocationWCS(txyz);
   return sqrt((fxyz[0] - txyz[0]) * (fxyz[0] - txyz[0]) + (fxyz[1] - txyz[1]) * (fxyz[1] - txyz[1]) +
               (fxyz[2] - txyz[2]) * (fxyz[2] - txyz[2]));
}

double UtRelativeGeometry::CalculateRadial(const UtEntity& aFrom, const UtEntity& aTo)
{
   double fxyz[3];
   double txyz[3];
   aFrom.GetLocationWCS(fxyz);
   aTo.GetLocationWCS(txyz);
   UtVec3d pfxyz(fxyz);
   UtVec3d ptxyz(txyz);
   UtVec3d dxyz = ptxyz - pfxyz;
   UtVec3d rVec = pfxyz; // up vector
   rVec.Normalize();     // normalized up
   return rVec.DotProduct(dxyz);
}

double UtRelativeGeometry::CalculateInTrack(const UtEntity& aFrom, const UtEntity& aTo)
{
   double ecixyzf[3];
   double ecivxyzf[3];
   aFrom.GetLocationECI(ecixyzf);
   aFrom.GetVelocityECI(ecivxyzf);

   double ecixyzt[3];
   aTo.GetLocationECI(ecixyzt);

   UtVec3d pxyz1(ecixyzf);
   UtVec3d pxyz2(ecixyzt);
   UtVec3d dxyz = pxyz2 - pxyz1;
   UtVec3d vxyz(ecivxyzf); // velocity vector
   UtVec3d cvec;
   cvec.CrossProduct(pxyz1, vxyz);
   UtVec3d ivec;
   ivec.CrossProduct(cvec, pxyz1);
   ivec.Normalize();

   return ivec.DotProduct(dxyz);
}

double UtRelativeGeometry::CalculateCrossTrack(const UtEntity& aFrom, const UtEntity& aTo)
{
   double ecixyzf[3];
   double ecivxyzf[3];
   aFrom.GetLocationECI(ecixyzf);
   aFrom.GetVelocityECI(ecivxyzf);

   double ecixyzt[3];
   aTo.GetLocationECI(ecixyzt);

   UtVec3d pxyz1(ecixyzf);
   UtVec3d pxyz2(ecixyzt);
   UtVec3d dxyz = pxyz2 - pxyz1;
   UtVec3d vxyz(ecivxyzf); // velocity vector
   UtVec3d cvec;
   cvec.CrossProduct(pxyz1, vxyz);
   cvec.Normalize();

   return cvec.DotProduct(dxyz);
}

void UtRelativeGeometry::CalculateRIC(const UtEntity& aFrom, const UtEntity& aTo, UtVec3d& aRIC)
{
   double ecixyzf[3];
   double ecivxyzf[3];
   aFrom.GetLocationECI(ecixyzf);
   aFrom.GetVelocityECI(ecivxyzf);

   double ecixyzt[3];
   aTo.GetLocationECI(ecixyzt);

   UtVec3d pxyz1(ecixyzf);
   UtVec3d pxyz2(ecixyzt);
   UtVec3d dxyz = pxyz2 - pxyz1;
   UtVec3d vxyz(ecivxyzf); // velocity vector
   UtVec3d cvec;
   cvec.CrossProduct(pxyz1, vxyz);
   cvec.Normalize();
   UtVec3d ivec;
   ivec.CrossProduct(cvec, pxyz1);
   ivec.Normalize();
   UtVec3d rvec = pxyz1; // up vector
   rvec.Normalize();     // normalized up
   aRIC[0] = rvec.DotProduct(dxyz);
   aRIC[1] = ivec.DotProduct(dxyz);
   aRIC[2] = cvec.DotProduct(dxyz);
}

double UtRelativeGeometry::CalculateBearing(const UtEntity& aFrom, const UtEntity& aTo)
{
   double txyz[3];
   aTo.GetLocationWCS(txyz);
   return aFrom.RelativeBearing(txyz);
}

double UtRelativeGeometry::CalculateElevation(const UtEntity& aFrom, const UtEntity& aTo)
{
   double txyz[3];
   aTo.GetLocationWCS(txyz);
   double ned[3];
   double az;
   double el;
   aFrom.ConvertWCSToNED(txyz, ned);
   UtEntity::ComputeAzimuthAndElevation(ned, az, el);
   return el;
}

double UtRelativeGeometry::CalculateDownRange(const UtEntity& aFrom, const UtEntity& aTo)
{
   UtEntity tempFrom = aFrom;
   double   hpr[3];
   double   ecs[3];
   double   txyz[3];
   aTo.GetLocationWCS(txyz);
   tempFrom.GetOrientationNED(hpr[0], hpr[1], hpr[2]);
   tempFrom.SetOrientationNED(hpr[0], 0.0, 0.0);
   tempFrom.ConvertWCSToECS(txyz, ecs);
   return ecs[0]; // this is a linear measurement, not a ground range measurement
}

double UtRelativeGeometry::CalculateCrossRange(const UtEntity& aFrom, const UtEntity& aTo)
{
   UtEntity tempFrom = aFrom;
   double   hpr[3];
   double   ecs[3];
   double   txyz[3];
   aTo.GetLocationWCS(txyz);
   tempFrom.GetOrientationNED(hpr[0], hpr[1], hpr[2]);
   tempFrom.SetOrientationNED(hpr[0], 0.0, 0.0);
   tempFrom.ConvertWCSToECS(txyz, ecs);
   return ecs[1]; // this is a linear measurement, not a ground range measurement
}

double UtRelativeGeometry::CalculateRangeRate(const UtEntity& aFrom, const UtEntity& aTo)
{
   auto v = aFrom.GetVelocityWCS() - aTo.GetVelocityWCS();
   auto d = aFrom.GetLocationWCS() - aTo.GetLocationWCS();
   d.Normalize();
   return v.DotProduct(d);
}
