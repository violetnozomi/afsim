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
// UtoPolygonShape.hpp : header file

#if !defined(_UTOPOLYGONSHAPE_HPP_)
#define _UTOPOLYGONSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoPolygonShapeImp;

class UTILOSG_EXPORT UtoPolygonShape : public UtoShape
{
public:
   UtoPolygonShape();
   UtoPolygonShape(const UtoPolygonShape&);
   UtoPolygonShape(const UtoPosition pts[], int num);
   ~UtoPolygonShape() override;
   UtoPolygonShape& operator=(const UtoPolygonShape&);

   UtoShape* Clone() const override;

   int         Insert(int pos, const UtoPosition&);
   int         Insert(int pos, const UtoPosition pts[], int num);
   int         Remove(int pos);
   void        Clear();
   UtoPosition At(int n);
   int         SetAt(int n, const UtoPosition& pos);
   int         Size() const;

   void EnableComplexPoly(bool enable);
   bool ComplexPoly() const;

   void EnableLighting(bool aState);

   void        SetSides(const UtoPosition& offset);
   UtoPosition Sides() const;

   void EnableSmoothSides(bool enable);
   bool SmoothSides() const;

   void Dump(std::ostream&) override;

protected:
private:
};

#endif // !defined(_UTOPOLYGONSHAPE_HPP_)
