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
// UtoPolygonShapeImp.hpp : header file

#if !defined(_UTOPOLYSHAPEIMP_HPP_)
#define _UTOPOLYSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoGeometryShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoPolyShape;

class UtoPolyShapeImp : public UtoGeometryShapeImp
{
public:
   UtoPolyShapeImp();
   UtoPolyShapeImp(const UtoPolyShapeImp& rhs);
   int         Insert(int pos, const UtoPosition pts[], int num);
   int         Remove(int pos);
   void        Clear();
   int         Size() const;
   UtoPosition At(int n);
   int         SetAt(int n, const UtoPosition& pos);
};

#endif // !defined(_UTOPOLYSHAPEIMP_HPP_)
