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
// UtoPointListShape.hpp : header file

#if !defined(_UTOPOINTLISTSHAPE_HPP_)
#define _UTOPOINTLISTSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoPointListShapeImp;

class UTILOSG_EXPORT UtoPointListShape : public UtoShape
{
public:
   UtoPointListShape();
   UtoPointListShape(const UtoPosition pts[], int num);
   UtoPointListShape(const UtoPointListShape&);
   UtoPointListShape& operator=(const UtoPointListShape&);
   ~UtoPointListShape() override;

   int         Insert(int pos, const UtoPosition&);
   int         Insert(int pos, const UtoPosition pts[], int num);
   int         Remove(int pos);
   void        Clear();
   UtoPosition At(int n);
   int         SetAt(int n, const UtoPosition& pos);
   int         Size() const;

   double PointSize() const;
   void   SetPointSize(double pointSize);

   double PointSmooth() const;
   void   SetPointSmooth(bool pointSmooth);

   UtoShape* Clone() const override;

   void Dump(std::ostream&) override;

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoPointListShape(UtoPointListShapeImp*);
#endif

private:
};

#endif // !defined(_UTOPOINTLISTSHAPE_HPP_)
