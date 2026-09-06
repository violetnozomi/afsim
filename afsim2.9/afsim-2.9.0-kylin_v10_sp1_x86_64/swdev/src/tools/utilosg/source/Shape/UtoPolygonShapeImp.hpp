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

#if !defined(_UTOPOLYGONSHAPEIMP_HPP_)
#define _UTOPOLYGONSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoPolyShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoPolygonShape;

class UtoPolygonShapeImp : public UtoPolyShapeImp
{
public:
   UtoPolygonShapeImp();
   UtoPolygonShapeImp(const UtoPosition pts[], int num);
   UtoPolygonShapeImp(const UtoPolygonShapeImp&);
   ~UtoPolygonShapeImp() override;

   void Update(osg::NodeVisitor* nv, osg::Drawable* drawable) override;

private:
   UtoPolygonShapeImp& operator=(const UtoPolygonShapeImp&);
};

#endif // !defined(_UTOPOLYGONSHAPEIMP_HPP_)
