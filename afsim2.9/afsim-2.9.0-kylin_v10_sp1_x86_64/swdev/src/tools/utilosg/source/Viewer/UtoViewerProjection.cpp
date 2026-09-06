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

/////////////////////////////////////////////////////////////////////////////
// ViewerProjectionDefault.cpp : implementation file

#include "UtoViewerProjection.hpp"

#include <osg/Matrix>
#include <osg/Quat>

#include "UtEntity.hpp"
#include "UtMath.hpp"

/////////////////////////////////////////////////////////////////////////////
UtoViewerProjection::UtoViewerProjection(UtoMapProjection& aMapProjection)
   : UtoMapProjection(aMapProjection)
{
}

UtoViewerProjection::~UtoViewerProjection() {}

void UtoViewerProjection::GetECS_Frame(UtEntity& aEntity, osg::Matrix& aMatrix)
{
#define AT(x, y) xfm[y][x]
   if (projection == UtoMapProjection::Geocentric)
   {
      double wcs[3];
      aEntity.GetLocationWCS(wcs);
      double xfm[3][3];
      aEntity.GetWCSToECSTransform(xfm);

      aMatrix.set(AT(0, 0),
                  AT(1, 0),
                  AT(2, 0),
                  0,
                  AT(0, 1),
                  AT(1, 1),
                  AT(2, 1),
                  0,
                  AT(0, 2),
                  AT(1, 2),
                  AT(2, 2),
                  0,
                  wcs[0],
                  wcs[1],
                  wcs[2],
                  1.0);
   }
   else
   {
      double scene[3];
      Convert(aEntity, scene);
      double xfm[3][3];
      aEntity.GetNEDToECSTransform(xfm);
      aMatrix.set(AT(0, 0), AT(1, 0), AT(2, 0), 0, AT(0, 1), AT(1, 1), AT(2, 1), 0, AT(0, 2), AT(1, 2), AT(2, 2), 0, 0, 0, 0, 1.0);
      aMatrix.postMultRotate(osg::Quat(UtMath::cPI, osg::Vec3d(0, 1, 0)));
      aMatrix.postMultRotate(osg::Quat(-UtMath::cPI_OVER_2, osg::Vec3d(0, 0, 1)));
      aMatrix.postMultTranslate(osg::Vec3f(scene[0], scene[1], scene[2] + 10));
   }
#undef AT
}

void UtoViewerProjection::GetNED_Frame(UtEntity& aEntity, osg::Matrix& aMatrix)
{
#define AT(x, y) xfm[y][x]
   if (projection == UtoMapProjection::Geocentric)
   {
      double wcs[3];
      aEntity.GetLocationWCS(wcs);
      double xfm[3][3];
      aEntity.GetWCSToNEDTransform(xfm);

      aMatrix.set(AT(0, 0),
                  AT(1, 0),
                  AT(2, 0),
                  0,
                  AT(0, 1),
                  AT(1, 1),
                  AT(2, 1),
                  0,
                  AT(0, 2),
                  AT(1, 2),
                  AT(2, 2),
                  0,
                  wcs[0],
                  wcs[1],
                  wcs[2],
                  1.0);
   }
   else
   {
      double scene[3];
      Convert(aEntity, scene);
      double xfm[3][3];
      aEntity.GetNEDToECSTransform(xfm);
      aMatrix.set(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1.0);
      aMatrix.postMultRotate(osg::Quat(UtMath::cPI, osg::Vec3d(0, 1, 0)));
      aMatrix.postMultRotate(osg::Quat(-UtMath::cPI_OVER_2, osg::Vec3d(0, 0, 1)));
      aMatrix.postMultTranslate(osg::Vec3f(scene[0], scene[1], scene[2]));
   }
}

UtoPosition UtoViewerProjection::CalcPoint(UtEntity& aEntity)
{
   double xyz[3];
   Convert(aEntity, xyz);
   UtoPosition NewPos(xyz[0], xyz[1], xyz[2]);
   return (NewPos += m_Offset);
}

UtoPosition UtoViewerProjection::CalcPointNoOffset(UtEntity& aEntity)
{
   double xyz[3];
   ConvertNoOffset(aEntity, xyz);
   UtoPosition NewPos(xyz[0], xyz[1], xyz[2]);
   return NewPos;
}

UtoPosition UtoViewerProjection::CalcPointOrient(UtEntity& aEntity, UtoOrientation& aOrient)
{
   double xyz[3];
   double ptp[3];
   Convert(aEntity, xyz, ptp);
   UtoPosition NewPos(xyz[0], xyz[1], xyz[2]);
   aOrient.roll  = ptp[0];
   aOrient.pitch = ptp[1];
   aOrient.yaw   = ptp[2];
   return NewPos;
}

UtoPosition UtoViewerProjection::CalcPointOrientNoOffset(UtEntity& aEntity, UtoOrientation& aOrient)
{
   double xyz[3];
   double ptp[3];
   ConvertNoOffset(aEntity, xyz, ptp);
   UtoPosition NewPos(xyz[0], xyz[1], xyz[2]);
   aOrient.roll  = ptp[0];
   aOrient.pitch = ptp[1];
   aOrient.yaw   = ptp[2];
   return NewPos;
}
