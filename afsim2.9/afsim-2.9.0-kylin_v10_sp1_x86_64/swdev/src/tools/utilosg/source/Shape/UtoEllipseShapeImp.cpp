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
// UtoEllipseShapeImp.cpp : implementation file

#include "UtoEllipseShapeImp.hpp"

#include <stdlib.h>

#include <osg/Geode>
#include <osg/Geometry>

#include "UtMath.hpp"

UtoEllipseShapeImp::UtoEllipseShapeImp()
   : UtoGeometryShapeImp()
   , m_XRadius(1.0)
   , m_YRadius(1.0)
   , m_MinRadius(0.0)
   , m_SliceAngle(15)
   , m_BeginAngle(0.0)
   , m_EndAngle(360.0)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::TRIANGLE_FAN);
   m_Geode->setName("UtoEllipseShape");

   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_Geode.get());
   RemoveShapeChildNode(m_Geode.get());
   AddShapeChildNode(mOrientationTransform);

   NeedCallback();
}

UtoEllipseShapeImp::UtoEllipseShapeImp(double fXRadius, double fYRadius, double Angle)
   : UtoGeometryShapeImp()
   , m_XRadius(fXRadius)
   , m_YRadius(fYRadius)
   , m_MinRadius(0.0)
   , m_SliceAngle(Angle)
   , m_BeginAngle(0.0)
   , m_EndAngle(360.0)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::TRIANGLE_FAN);
   m_Geode->setName("UtoEllipseShape");

   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_Geode.get());
   RemoveShapeChildNode(m_Geode.get());
   AddShapeChildNode(mOrientationTransform);

   NeedCallback();
}

UtoEllipseShapeImp::UtoEllipseShapeImp(const UtoEllipseShapeImp& rhs)
   : UtoGeometryShapeImp(rhs)
   , m_XRadius(rhs.m_XRadius)
   , m_YRadius(rhs.m_YRadius)
   , m_MinRadius(rhs.m_MinRadius)
   , m_SliceAngle(rhs.m_SliceAngle)
   , m_BeginAngle(rhs.m_BeginAngle)
   , m_EndAngle(rhs.m_EndAngle)
{
   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_Geode.get());
   RemoveShapeChildNode(m_Geode.get());
   AddShapeChildNode(mOrientationTransform);

   NeedCallback();
}

UtoEllipseShapeImp::~UtoEllipseShapeImp() {}

void UtoEllipseShapeImp::SetRadii(double fXRadius, double fYRadius)
{
   if (fXRadius >= 0.0)
   {
      m_XRadius = fXRadius;
   }
   if (fYRadius >= 0.0)
   {
      m_YRadius = fYRadius;
   }

   NeedCallback();
}

void UtoEllipseShapeImp::GetRadii(double& fXRadius, double& fYRadius) const
{
   fXRadius = m_XRadius;
   fYRadius = m_YRadius;
}

void UtoEllipseShapeImp::SetMinRadius(double aMinRadius)
{
   m_MinRadius = aMinRadius;
   NeedCallback();
}

void UtoEllipseShapeImp::SetOrientationTransform(const UtoOrientation& aOrientation)
{
   mOrientationTransform->setMatrix(osg::Matrix::rotate(aOrientation.yaw,
                                                        osg::Vec3d(0, 0, 1),
                                                        aOrientation.pitch,
                                                        osg::Vec3d(0, 1, 0),
                                                        aOrientation.roll,
                                                        osg::Vec3d(1, 0, 0)));
}

void UtoEllipseShapeImp::SetSliceAngle(double angle)
{
   m_SliceAngle = angle;
   NeedCallback();
}

void UtoEllipseShapeImp::EnableLineMode(bool enable)
{
   if (enable)
   {
      m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_LOOP);
   }
   else
   {
      m_PrimitiveSet->setMode(osg::PrimitiveSet::TRIANGLE_FAN);
   }
   // If it has sides the base geometry needs updating, otherwise just the
   // display lists.
   if (m_SideOffset.x() || m_SideOffset.y() || m_SideOffset.z())
      NeedCallback();
   else
      m_Geometry->dirtyDisplayList();
}

void UtoEllipseShapeImp::Update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
   /* There are different possible algorithms for calculating the number of
    * slices and where to stop.
    * 1. float num_slices=360.0/m_SliceAngle;
    *    vertex.resize(num_slices+1);
    *    for(float angle = 0; angle <= 2*M_PI; angle+=increment, ++i)
    *    Problems:  The ending point will be equal to the beginning point, or
    *    with floating point errors will almost be equal to.  As it is a
    *    line loop or a triangle fan that is only an unnecessary vertex and
    *    primitive, with the SetSides and smooth shading the primitive before
    *    and after the nearly non-existent primitive are smooth shaded with
    *    the nearly non-existant primitive and not between the previous
    *    and next.  That creates a visible seam.
    * 2. float num_slices=360.0/m_SliceAngle;
    *    vertex.resize(num_slices+1);
    *    stop=M_PI-increment/32
    *    for(float angle = 0; angle <stop; angle+=increment, ++i)
    *    vertex.resize(i);
    *    Except it would be better to resize the vertex once
    * 3. stop=M_PI-increment/32
    *    for(float angle = 0; angle<stop; angle+=increment)
    *       ++size;
    *    vertex.resize(size);
    *    for(float angle = 0, i=0; i<size; angle+=increment, ++i)
    *    double loops, but the floating point errors can't cause a array
    *    bounding violation.
    * 4. float num_slices=2*M_PI/increment;
    *    if(increment*num_slices >= 2*M_PI-increment/32)
    *       --num_slices;
    *    ++num_slices
    *    vertex.resize(num_slices)
    *    for(float angle = 0, i=0; i<num_slices; angle+=increment, ++i)
    *    This will avoid out of bounds array assignment and doesn't require
    *    two loops.
    *
    * stop=M_PI-increment/32; 32 is somewhat arbitrary, it just can't be
    * too big or the last side will be extra big, or too small or the
    * smoothing will be a problem when smoothed sides are used.
    * - David Fries
    */

   if (UtMath::NearlyEqual(m_EndAngle - m_BeginAngle, 360.0))
   {
      const float increment  = DtoR(m_SliceAngle);
      int         num_slices = (int)(2 * M_PI / increment);

      // Keep the last point from being the first point or just short of that.
      if (increment * num_slices >= 2 * M_PI - increment / 32)
         --num_slices;

      // for loop less than, but that point needs to be calculated
      ++num_slices;

      m_PrimitiveSet->setCount(num_slices + 1);

      osg::Vec3Array& vertex = *m_Vertex.get();
      vertex.resize(num_slices + 1);
      size_t i = 0;

      // Shader requires first point to be 0,0,0, so indexing starts at 1.
      if (m_PrimitiveSet->getMode() == osg::PrimitiveSet::TRIANGLE_FAN)
      {
         i++;
         m_PrimitiveSet->setCount(num_slices + 2);
         vertex.resize(num_slices + 2);
      }

      for (float angle = 0; i < vertex.size(); angle += increment, ++i)
      {
         vertex[i] = osg::Vec3(cos(angle) * m_XRadius, sin(angle) * m_YRadius, 0);
      }
      m_FullCircle = true;
   }
   else
   {
      double endAngle = m_EndAngle;
      if (m_EndAngle < m_BeginAngle)
      {
         while (endAngle >= 360.0)
         {
            endAngle -= 360.0;
         }
         while (endAngle < 0.0)
         {
            endAngle += 360.0;
         }
         endAngle = UtMath::NormalizeAngle0_360(m_EndAngle);
      }
      int num_slices = (int)fabs(((endAngle - m_BeginAngle) / m_SliceAngle));
      if (num_slices < 1)
         num_slices = 1;
      m_PrimitiveSet->setCount(num_slices + 3);

      osg::Vec3Array& vertex = *m_Vertex.get();
      vertex.resize(num_slices + 3);

      float     outerWarpAngle = atan2(sin(DtoR(m_BeginAngle)) * m_XRadius, cos(DtoR(m_BeginAngle)) * m_YRadius);
      osg::Vec3 outerPoint(cos(outerWarpAngle) * m_XRadius, sin(outerWarpAngle) * m_YRadius, 0);
      float     innerWarpAngle = atan2(sin(DtoR(m_BeginAngle)) * m_MinRadius, cos(DtoR(m_BeginAngle)) * m_MinRadius);
      osg::Vec3 innerPoint(cos(innerWarpAngle) * m_MinRadius, sin(innerWarpAngle) * m_MinRadius, 0);
      vertex[0] = outerPoint.length() < innerPoint.length() ? outerPoint : innerPoint;

      const float increment = DtoR(m_SliceAngle);
      float       angle     = DtoR(m_BeginAngle);
      int         i;

      for (i = 0; i < num_slices; ++i)
      {
         outerWarpAngle = atan2(sin(angle) * m_XRadius, cos(angle) * m_YRadius);
         vertex[i + 1]  = osg::Vec3(cos(outerWarpAngle) * m_XRadius, sin(outerWarpAngle) * m_YRadius, 0);
         angle += increment;
      }

      outerWarpAngle = atan2(sin(DtoR(endAngle)) * m_XRadius, cos(DtoR(endAngle)) * m_YRadius);
      vertex[i + 1]  = osg::Vec3(cos(outerWarpAngle) * m_XRadius, sin(outerWarpAngle) * m_YRadius, 0);
      i++;
      // If minimum radius is specified, draw around minimum edge back to start angle.
      if (m_MinRadius != 0)
      {
         vertex.resize(num_slices * 2 + 3);
         m_PrimitiveSet->setCount(num_slices * 2 + 3);

         angle = DtoR(endAngle);
         for (; i <= num_slices * 2; ++i)
         {
            outerWarpAngle = atan2(sin(angle) * m_XRadius, cos(angle) * m_YRadius);
            osg::Vec3 outerPoint(cos(outerWarpAngle) * m_XRadius, sin(outerWarpAngle) * m_YRadius, 0);

            innerWarpAngle = atan2(sin(angle) * m_MinRadius, cos(angle) * m_MinRadius);
            osg::Vec3 innerPoint(cos(innerWarpAngle) * m_MinRadius, sin(innerWarpAngle) * m_MinRadius, 0);

            vertex[i + 1] = outerPoint.length() < innerPoint.length() ? outerPoint : innerPoint;
            angle -= increment;
         }

         outerWarpAngle = atan2(sin(DtoR(m_BeginAngle)) * m_XRadius, cos(DtoR(m_BeginAngle)) * m_YRadius);
         osg::Vec3 outerPoint(cos(outerWarpAngle) * m_XRadius, sin(outerWarpAngle) * m_YRadius, 0);

         innerWarpAngle = atan2(sin(DtoR(m_BeginAngle)) * m_MinRadius, cos(DtoR(m_BeginAngle)) * m_MinRadius);
         osg::Vec3 innerPoint(cos(innerWarpAngle) * m_MinRadius, sin(innerWarpAngle) * m_MinRadius, 0);

         vertex[i + 1] = outerPoint.length() < innerPoint.length() ? outerPoint : innerPoint;
         i++;
      }
      else
      {
         vertex[i + 1] = osg::Vec3(0, 0, 0);
      }
      m_FullCircle = false;
   }
   m_Geometry->dirtyDisplayList();
   m_Geometry->dirtyBound();

   UtoGeometryShapeImp::Update(nv, drawable);
}

void UtoEllipseShapeImp::SetAngleBounds(double aBeginAngle, double aEndAngle)
{
   m_BeginAngle = aBeginAngle;
   m_EndAngle   = aEndAngle;

   NeedCallback();
}
