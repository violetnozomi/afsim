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
// UtoPolyLineShape.hpp : header file

#if !defined(_UTOPOLYLINESHAPE_HPP_)
#define _UTOPOLYLINESHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoPolyLineShapeImp;

class UTILOSG_EXPORT UtoPolyLineShape : public UtoShape
{
public:
   enum Patterns
   {
      DASHED      = 0xAAAA,
      LONG_DASHED = 0x00FF,
      DOTTED      = 0x1111,
      DOTTED2     = 0x0101,
      DASH_DOT    = 0x264C,
      DASH_DOT2   = 0x1C47,
      SOLID       = 0xffff
   };

   enum Aggregation
   {
      LINE_STRIP,
      LINES,
      LINE_LOOP
   };

   UtoPolyLineShape();
   UtoPolyLineShape(const UtoPosition pts[], int num);
   UtoPolyLineShape(const UtoPolyLineShape&);
   UtoPolyLineShape& operator=(const UtoPolyLineShape&);
   ~UtoPolyLineShape() override;

   int         Insert(int pos, const UtoPosition&);
   int         Insert(int pos, const UtoPosition pts[], int num);
   int         Remove(int pos);
   void        Clear();
   UtoPosition At(int n);
   int         SetAt(int n, const UtoPosition& pos);
   int         Size() const;

   short Pattern() const;
   int   PatternFactor() const;
   void  SetPattern(int pattern, int factor = 1);

   double Width() const;
   void   SetWidth(double width);

   void SetOrientation(const UtoOrientation& aOrientation);

   unsigned int Aggregation() const;
   void         SetAggregation(unsigned int aggregation);

   UtoShape* Clone() const override;

   void        SetSides(const UtoPosition& offset);
   UtoPosition Sides() const;

   void EnableSmoothSides(bool enable);
   bool SmoothSides() const;

   void EnableAntiAliasLines(bool enable);
   bool AntiAliasLines() const;

   void SetDepthBin(int binNumber);

   // determines if the shape is allowed to be depth tested
   void SetDepthTesting(bool bDepthTest);

   void Dump(std::ostream&) override;

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoPolyLineShape(UtoPolyLineShapeImp*);
#endif

private:
};

#endif // !defined(_UTOPOLYLINESHAPE_HPP_)
