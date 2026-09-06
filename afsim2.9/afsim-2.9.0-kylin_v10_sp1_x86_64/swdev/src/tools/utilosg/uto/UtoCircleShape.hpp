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
// UtoCircleShape.hpp : header file

#if !defined(_UTOCIRCLESHAPE_HPP_)
#define _UTOCIRCLESHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UTILOSG_EXPORT UtoCircleShape : public UtoShape
{
public:
   UtoCircleShape();
   UtoCircleShape(double fRadius, double fAngle = 15.0);
   UtoCircleShape(const UtoCircleShape&);
   UtoCircleShape(const UtoPosition& pos, double radius, double fAngle = 15.0);
   ~UtoCircleShape() override;
   UtoCircleShape& operator=(const UtoCircleShape&);

   UtoShape* Clone() const override;

   double Radius() const;
   bool   SetRadius(double radius);

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

   void        SetSides(const UtoPosition& offset);
   UtoPosition Sides() const;

   void EnableSmoothSides(bool enable);
   bool SmoothSides() const;

   void DisableLighting();
   void SetUsesShader(bool aUsesShader);

   void SetAngleBounds(double minAngle, double maxAngle);

   void Dump(std::ostream&) override;

protected:
private:
};

#endif // !defined(_UTOCIRCLESHAPE_HPP_)
