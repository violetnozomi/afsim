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
// UtoEllipseShape.hpp : header file

#if !defined(_UTOELLIPSESHAPE_HPP_)
#define _UTOELLIPSESHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UTILOSG_EXPORT UtoEllipseShape : public UtoShape
{
public:
   UtoEllipseShape();
   UtoEllipseShape(const UtoEllipseShape&);
   UtoEllipseShape(double fXRadius, double fYRadius, double fAngle = 15.0);
   ~UtoEllipseShape() override;
   UtoEllipseShape& operator=(const UtoEllipseShape&);

   UtoShape* Clone() const override;

   void SetRadii(double fXRadius, double fYRadius);
   void GetRadii(double& fXRadius, double& fYRadius);

   void SetOrientation(const UtoOrientation& aOrientation);

   void SetMinRadius(double aMinRadius);

   double SliceAngle() const;
   void   SetSliceAngle(double angle);

   short Pattern() const;
   int   PatternFactor() const;
   void  SetPattern(int pattern, int factor = 1);

   double LineWidth() const;
   void   SetLineWidth(double width);
   void   EnableLineMode(bool enable);

   void SetAngleBounds(double aBeginAngle, double aEndAngle);

   void        SetSides(const UtoPosition& offset);
   UtoPosition Sides() const;

   void EnableSmoothSides(bool enable);
   bool SmoothSides() const;

   void EnableLighting(bool aState);
   void SetUsesShader(bool aUsesShader);

   void Dump(std::ostream&) override;

protected:
private:
};

#endif // !defined(_UTOELLIPSESHAPE_HPP_)
