// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoIconBoardShape.cpp : implementation file

#include "UtoIconBoardShape.hpp"

#include "UtoIconBoardShapeImp.hpp"

/**
\ingroup shape
\class UtoIconBoardShape
\brief Draws status icons on a grid/board

*/

//{secret}
inline UtoIconBoardShapeImp* GetImpl(UtoIconBoardShape* shape)
{
   return static_cast<UtoIconBoardShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoIconBoardShapeImp* GetImpl(const UtoIconBoardShape* shape)
{
   return static_cast<const UtoIconBoardShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoIconBoardShape::UtoIconBoardShape()
   : UtoShape(new UtoIconBoardShapeImp())
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param aSrc the shape instance to be copied.
*/
UtoIconBoardShape::UtoIconBoardShape(const UtoIconBoardShape& aSrc)
   : UtoShape(new UtoIconBoardShapeImp(*GetImpl(&aSrc)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoIconBoardShape::~UtoIconBoardShape() {}

UtoShape* UtoIconBoardShape::Clone() const
{
   return new UtoIconBoardShape(*this);
}

void UtoIconBoardShape::Clear()
{
   GetImpl(this)->Clear();
}

void UtoIconBoardShape::AddTexture(const std::string& aCardId, unsigned int aWidth, unsigned int aHeight, unsigned char* aBits)
{
   GetImpl(this)->AddTexture(aCardId, aWidth, aHeight, aBits);
}

void UtoIconBoardShape::AddCard(const std::string& aCardId)
{
   GetImpl(this)->AddCard(aCardId);
}
