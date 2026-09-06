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
// UtoRawShape.cpp : implementation file

#include "UtoRawShape.hpp"

#include <algorithm>

#include "UtoShapeImp.hpp"


/**
\ingroup shape
\class UtoRawShape
\brief An empty shape allowing access to the shape's root node.

An empty shape allowing access to the shape's root node.
*/
class UtoRawShape;

UtoRawShape::UtoRawShape()
   : UtoShape(new UtoShapeImp)
{
}

UtoRawShape::UtoRawShape(const UtoRawShape& aSrc)
   : UtoShape(new UtoShapeImp(*aSrc.Impl()))
{
}

UtoShape* UtoRawShape::Clone() const
{
   return new UtoRawShape(*this);
}

osg::Transform* UtoRawShape::GetRoot()
{
   return (osg::Transform*)Impl()->GetTree();
}
