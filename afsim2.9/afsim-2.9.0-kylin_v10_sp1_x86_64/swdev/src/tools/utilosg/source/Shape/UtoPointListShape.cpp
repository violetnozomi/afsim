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
// UtoPointListShape.cpp : implementation file

#include "UtoPointListShape.hpp"

#include "UtoPointListShapeImp.hpp"

/**
\ingroup shape
\class UtoPointListShape
\brief Draws a point at multiple vertices.

The pointlist shape is used to construct a list of points.
*/
class UtoPointListShape;

//{secret}
inline UtoPointListShapeImp* GetImpl(UtoPointListShape* shape)
{
   return static_cast<UtoPointListShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoPointListShapeImp* GetImpl(const UtoPointListShape* shape)
{
   return static_cast<const UtoPointListShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoPointListShape::UtoPointListShape()
   : UtoShape(new UtoPointListShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param pts an array of UtoPosition specifying multiple points.
\param num the size of the pts array.
*/
UtoPointListShape::UtoPointListShape(const UtoPosition pts[], int num)
   : UtoShape(new UtoPointListShapeImp(pts, num))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoPointListShape::UtoPointListShape(const UtoPointListShape& rhs)
   : UtoShape(new UtoPointListShapeImp(*GetImpl(&rhs)))
{
}

UtoPointListShape::UtoPointListShape(UtoPointListShapeImp* imp)
   : UtoShape(imp)
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoPointListShape::~UtoPointListShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoPointListShape & - a reference to the current object.
*/
UtoPointListShape& UtoPointListShape::operator=(const UtoPointListShape& rhs)
{
   UtoPointListShape T(rhs);
   Swap(T);
   return *this;
}
//@}

/** @name Accessors */
//@{

/**
\brief Returns the position of a given vertex index.

Returns the position of a given vertex index.
\param n the integer index of the point list vertex.
\return UtoPosition - the position of the given vertex at n.
*/
UtoPosition UtoPointListShape::At(int n)
{
   return GetImpl(this)->At(n);
}

/**
\brief Returns the number of point vertices.

Returns the number of point vertices.
\return int - the number of vertices.
*/
int UtoPointListShape::Size() const
{
   return GetImpl(this)->Size();
}

double UtoPointListShape::PointSize() const
{
   return GetImpl(this)->PointSize();
}

double UtoPointListShape::PointSmooth() const
{
   return GetImpl(this)->PointSmooth();
}

//@}

/** @name Operations */
//@{

/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoPointListShape::Clone() const
{
   return new UtoPointListShape(*this);
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
int UtoPointListShape::Insert(int pos, const UtoPosition& pt)
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
int UtoPointListShape::Insert(int pos, const UtoPosition pts[], int num)
{
   return GetImpl(this)->Insert(pos, pts, num);
}

/**
\brief Remove the position at the specified index.

Remove the position at the specified index.
\param pos the index to be removed.
\return int - <table>
                  <tr><td>0</td><td>The position was inserted successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoPointListShape::Remove(int pos)
{
   return GetImpl(this)->Remove(pos);
}

/**
\brief Clears all positions

Clears all positions.
*/
void UtoPointListShape::Clear()
{
   GetImpl(this)->Clear();
}

/**
\brief Sets the position at a given vertex index.

Sets the position at a given vertex index.
\param n the integer index of the polygon vertex.
\param pos the position value to set.
\return int - <table>
                  <tr><td>0</td><td>The position was inserted successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoPointListShape::SetAt(int n, const UtoPosition& pos)
{
   return GetImpl(this)->SetAt(n, pos);
}

void UtoPointListShape::SetPointSize(double pointSize)
{
   GetImpl(this)->SetPointSize(pointSize);
}

void UtoPointListShape::SetPointSmooth(bool pointSmooth)
{
   GetImpl(this)->SetPointSmooth(pointSmooth);
}


// {secret}
void UtoPointListShape::Dump(std::ostream& o)
{
   o << "  Type: UtoPointListShape" << std::endl;
   UtoShape::Dump(o);
}
