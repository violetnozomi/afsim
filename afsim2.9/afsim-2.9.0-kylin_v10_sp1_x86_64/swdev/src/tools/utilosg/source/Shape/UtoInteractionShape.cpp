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
// CmePolyLineShape.cpp : implementation file

#include "UtoInteractionShape.hpp"

#include "UtoInteractionShapeImp.hpp"

/**
\ingroup shape
\class UtoInteractionShape
\brief Draws direction lines from multiple sources and targets with per segment coloring.

The cmeinteraction shape is used to construct lines consisting of multiple segments with
multiple colors.  Segments and colors can be added to the interaction shape using the
AppendSegment method, and removed using the Clear method.
*/
class UtoInteractionShape;

//{secret}
inline UtoInteractionShapeImp* GetImpl(UtoInteractionShape* shape)
{
   return static_cast<UtoInteractionShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoInteractionShapeImp* GetImpl(const UtoInteractionShape* shape)
{
   return static_cast<const UtoInteractionShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoInteractionShape::UtoInteractionShape()
   : UtoAttrPolyLineShape(new UtoInteractionShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoInteractionShape::UtoInteractionShape(const UtoInteractionShape& rhs)
   : UtoAttrPolyLineShape(new UtoInteractionShapeImp(*GetImpl(&rhs)))
{
}

UtoInteractionShape::UtoInteractionShape(UtoInteractionShapeImp* imp)
   : UtoAttrPolyLineShape(imp)
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoInteractionShape::~UtoInteractionShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoInteractionShape & - a reference to the current object.
*/
UtoInteractionShape& UtoInteractionShape::operator=(const UtoInteractionShape& rhs)
{
   UtoInteractionShape T(rhs);
   Swap(T);
   return *this;
}
//@}


/** @name Operations */
//@{

/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoInteractionShape::Clone() const
{
   return new UtoInteractionShape(*this);
}

int UtoInteractionShape::AddInboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor)
{
   return GetImpl(this)->AddInboundSegment(aSrc, aOffset, aColor);
}

int UtoInteractionShape::AddOutboundSegment(UtoEntity* aSrc, int aOffset, const UtoColor& aColor)
{
   return GetImpl(this)->AddOutboundSegment(aSrc, aOffset, aColor);
}

/**
\brief Sets a texture pattern for the line

Sets a texture pattern for the line
\param aPattern the pattern in a character array
\param aSize the length of the pattern.
*/
void UtoInteractionShape::SetTexturePattern(unsigned char aPattern[], const int aSize)
{
   GetImpl(this)->SetTexturePattern(aPattern, aSize);
}

/**
\brief Clears all positions

Clears all positions.
*/
void UtoInteractionShape::Clear()
{
   GetImpl(this)->Clear();
}

void UtoInteractionShape::ArchSegments(bool aState)
{
   GetImpl(this)->ArchSegments(aState);
}

//@}

// {secret}
void UtoInteractionShape::Dump(std::ostream& o)
{
   o << "  Type: UtoInteractionShape" << std::endl;
   UtoShape::Dump(o);
}

void UtoInteractionShape::SetSceneProjection(const UtoMapProjection& aProjection)
{
   GetImpl(this)->SetSceneProjection(aProjection);
}

void UtoInteractionShape::SetFillFactor(double aFactor)
{
   GetImpl(this)->SetFillFactor(aFactor);
}
