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
// UtoTessellatedPolygonShape.cpp : implementation file

#include "UtoTessellatedPolygonShape.hpp"

#include "UtoTessellatedPolygonShapeImp.hpp"

/**
\ingroup shape
\class UtoTessellatedPolygonShape
\brief Draws polygons from multiple vertices.

The composite polygon shape is used to construct surfaces consisting of multiple vertices.  Vertices may
be specified at construction using an array of UtoPosition.  Vertices can be added to the
line using the Insert method, and removed using the Remove method.  Vertices can be accessed
using the At method and modified using the SetAt method.

*/
class UtoTessellatedPolygonShape;

//{secret}
inline UtoTessellatedPolygonShapeImp* GetImpl(UtoTessellatedPolygonShape* shape)
{
   return static_cast<UtoTessellatedPolygonShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoTessellatedPolygonShapeImp* GetImpl(const UtoTessellatedPolygonShape* shape)
{
   return static_cast<const UtoTessellatedPolygonShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoTessellatedPolygonShape::UtoTessellatedPolygonShape()
   : UtoShape(new UtoTessellatedPolygonShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param pts an array of UtoPosition specifying polyline points.
\param num the size of the pts array.
*/
UtoTessellatedPolygonShape::UtoTessellatedPolygonShape(const UtoPosition pts[], int num)
   : UtoShape(new UtoTessellatedPolygonShapeImp(pts, num))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoTessellatedPolygonShape::UtoTessellatedPolygonShape(const UtoTessellatedPolygonShape& rhs)
   : UtoShape(new UtoTessellatedPolygonShapeImp(*GetImpl(&rhs)))
{
}

UtoTessellatedPolygonShape::UtoTessellatedPolygonShape(UtoTessellatedPolygonShapeImp* imp)
   : UtoShape(imp)
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoTessellatedPolygonShape::~UtoTessellatedPolygonShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoPolyLineShape & - a reference to the current object.
*/
UtoTessellatedPolygonShape& UtoTessellatedPolygonShape::operator=(const UtoTessellatedPolygonShape& rhs)
{
   UtoTessellatedPolygonShape T(rhs);
   Swap(T);
   return *this;
}
//@}

/** @name Accessors */
//@{

//@}

/** @name Operations */
//@{

/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoTessellatedPolygonShape::Clone() const
{
   return new UtoTessellatedPolygonShape(*this);
}

/**
\brief Inserts new positions(s) before the specified index.

Inserts new positions(s) before the specified index.
\param pos the index location.
\param pt the position to insert at pos.
\return int - <table>
                  <tr><td>0</td><td>The position was inserted successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoTessellatedPolygonShape::Insert(int pos, const UtoPosition& pt)
{
   return GetImpl(this)->Insert(pos, &pt, 1);
}

/**
\brief Inserts new positions(s) before the specified index.

Inserts new positions(s) before the specified index.
\param pos the index location.
\param pts the position(s) to insert at pos.
\param num the array size of pts.
\return int - <table>
                  <tr><td>0</td><td>The position was inserted successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoTessellatedPolygonShape::Insert(int pos, const UtoPosition pts[], int num)
{
   return GetImpl(this)->Insert(pos, pts, num);
}

/**
\brief Sets the relative orientation of the shape.

Sets the orientation of the shape. The orientation is relative to the entity the shape is attached to.

Units are radians. To set the absolute heading, call this function
with a heading of absHeading - entityHeading.
\param aOrientation the new relative orientation of the shape.
*/
void UtoTessellatedPolygonShape::SetOrientation(const UtoOrientation& aOrientation)
{
   return GetImpl(this)->SetOrientationTransform(aOrientation);
}

void UtoTessellatedPolygonShape::SetOffset(const UtoPosition& offset)
{
   GetImpl(this)->SetOffset(offset);
}


UtoPosition UtoTessellatedPolygonShape::GetOffset() const
{
   return GetImpl(this)->GetOffset();
}

void UtoTessellatedPolygonShape::SetClosed(bool closed)
{
   GetImpl(this)->SetClosed(closed);
}

bool UtoTessellatedPolygonShape::GetClosed() const
{
   return GetImpl(this)->GetClosed();
}

/**
\brief Clears all positions

Clears all positions.
*/
void UtoTessellatedPolygonShape::Clear()
{
   GetImpl(this)->Clear();
}

void UtoTessellatedPolygonShape::EnableLighting(bool aState)
{
   GetImpl(this)->EnableLighting(aState);
}

//@}

void UtoTessellatedPolygonShape::Rebuild()
{
   GetImpl(this)->Rebuild();
}

// {secret}
void UtoTessellatedPolygonShape::Dump(std::ostream& o)
{
   o << "  Type: UtoTessellatedPolygonShape" << std::endl;
   UtoShape::Dump(o);
}

void UtoTessellatedPolygonShape::SetDepthTesting(bool bDepthTest)
{
   GetImpl(this)->SetDepthTesting(bDepthTest);
}
