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
// UtoEllipseShapeImp.hpp : header file

#if !defined(_UTOELLIPSESHAPEIMP_HPP_)
#define _UTOELLIPSESHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoGeometryShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoEllipseShape;

class UtoEllipseShapeImp : public UtoGeometryShapeImp
{
public:
   UtoEllipseShapeImp();
   UtoEllipseShapeImp(double fXRadius, double fYRadius, double Angle);
   UtoEllipseShapeImp(const UtoEllipseShapeImp&);
   ~UtoEllipseShapeImp() override;

   void SetRadii(double fXRadius, double fYRadius);
   void GetRadii(double& fXRadius, double& fYRadius) const;

   void SetMinRadius(double aMinRadius);

   void SetOrientationTransform(const UtoOrientation& aOrientation);

   void   SetSliceAngle(double angle);
   double SliceAngle() const { return m_SliceAngle; }

   void EnableLineMode(bool enable);

   void SetAngleBounds(double aBeginAngle, double aEndAngle);

   // Computes the geometry, override the base callback.
   void Update(osg::NodeVisitor* nv, osg::Drawable* drawable) override;

private:
   UtoEllipseShapeImp& operator=(const UtoEllipseShapeImp&);

   double m_XRadius;
   double m_YRadius;
   double m_MinRadius;
   double m_SliceAngle;
   double m_BeginAngle;
   double m_EndAngle;

   osg::MatrixTransform* mOrientationTransform;
};

#endif // !defined(_UTOELLIPSESHAPEIMP_HPP_)
