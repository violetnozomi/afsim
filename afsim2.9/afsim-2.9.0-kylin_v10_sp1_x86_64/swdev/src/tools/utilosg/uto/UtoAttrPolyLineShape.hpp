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
// UtoAttrPolyLineShape.hpp : header file

#if !defined(_UTOATTRPOLYLINESHAPE_HPP_)
#define _UTOATTRPOLYLINESHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <set>

#include "UtoPolyLineShape.hpp"
#include "UtoTypes.hpp"

class UtoAttrPolyLineShapeImp;

class UTILOSG_EXPORT UtoAttrPolyLineShape : public UtoPolyLineShape
{
public:
   UtoAttrPolyLineShape();
   UtoAttrPolyLineShape(const UtoPosition pts[], const UtoColor cls[], int num);
   UtoAttrPolyLineShape(const UtoAttrPolyLineShape&);
   UtoAttrPolyLineShape& operator=(const UtoAttrPolyLineShape&);
   ~UtoAttrPolyLineShape() override;

   int         Insert(int pos, const UtoPosition&);
   int         Insert(int pos, const UtoPosition&, const UtoColor&);
   int         Insert(int pos, const UtoPosition pts[], int num);
   int         Insert(int pos, const UtoPosition pts[], const UtoColor col[], int num);
   int         Remove(int pos);
   void        Clear();
   UtoPosition At(int n);
   UtoColor    ColorAt(int n);
   int         SetAt(int n, const UtoPosition& pos);
   int         SetAt(int n, const UtoPosition& pos, const UtoColor& col);
   int         Size() const;
   void        SetColor(const UtoColor& col) {} // don't do anything here

   UtoShape* Clone() const override;

   void Dump(std::ostream&) override;

   virtual bool SetShaders(const char* aVShader, const char* aFShader, std::set<std::string>& aDefList);

protected:
#ifdef DOCJET // hide from DocJet or it is tied to the other constructor
#else
   UtoAttrPolyLineShape(UtoAttrPolyLineShapeImp*);
#endif

private:
};

#endif // !defined(_UTOATTRPOLYLINESHAPE_HPP_)
