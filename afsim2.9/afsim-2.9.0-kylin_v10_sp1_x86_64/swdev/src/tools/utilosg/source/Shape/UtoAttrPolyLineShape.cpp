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

#include "UtoAttrPolyLineShape.hpp"

#include "UtoAttrPolyLineShapeImp.hpp"

/**
\ingroup shape
\class UtoAttrPolyLineShape
\brief Draws lines from multiple vertices with per vertex coloring.

The cmepolyline shape is used to construct lines consisting of multiple vertices with
multiple colors.  Vertices and colors may be specified at construction using an array
of UtoPosition and UtoColor.  Vertices and colors can be added to the line using the
Insert method, and removed using the Remove method.  Vertices and colors can be
accessed using the At and ColorAt method and modified using the SetAt method.
*/
class UtoAttrPolyLineShape;

//{secret}
inline UtoAttrPolyLineShapeImp* GetImpl(UtoAttrPolyLineShape* shape)
{
   return static_cast<UtoAttrPolyLineShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoAttrPolyLineShapeImp* GetImpl(const UtoAttrPolyLineShape* shape)
{
   return static_cast<const UtoAttrPolyLineShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoAttrPolyLineShape::UtoAttrPolyLineShape()
   : UtoPolyLineShape(new UtoAttrPolyLineShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param pts an array of UtoPosition specifying polyline points.
\param num the size of the pts array.
*/
UtoAttrPolyLineShape::UtoAttrPolyLineShape(const UtoPosition pts[], const UtoColor cls[], int num)
   : UtoPolyLineShape(new UtoAttrPolyLineShapeImp(pts, cls, num))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoAttrPolyLineShape::UtoAttrPolyLineShape(const UtoAttrPolyLineShape& rhs)
   : UtoPolyLineShape(new UtoAttrPolyLineShapeImp(*GetImpl(&rhs)))
{
}

UtoAttrPolyLineShape::UtoAttrPolyLineShape(UtoAttrPolyLineShapeImp* imp)
   : UtoPolyLineShape(imp)
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoAttrPolyLineShape::~UtoAttrPolyLineShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoPolyLineShape & - a reference to the current object.
*/
UtoAttrPolyLineShape& UtoAttrPolyLineShape::operator=(const UtoAttrPolyLineShape& rhs)
{
   UtoAttrPolyLineShape T(rhs);
   Swap(T);
   return *this;
}
//@}

/** @name Accessors */
//@{

/**
\brief Returns the position of a given vertex index.

Returns the position of a given vertex index.
\param n the integer index of the polyline vertex.
\return UtoPosition - the position of the given vertex at n.
*/
UtoPosition UtoAttrPolyLineShape::At(int n)
{
   return GetImpl(this)->At(n);
}

/**
\brief Returns the color of a given vertex index.

Returns the color of a given vertex index.
\param n the integer index of the polyline vertex.
\return UtoColor - the color of the given vertex at n.
*/
UtoColor UtoAttrPolyLineShape::ColorAt(int n)
{
   return GetImpl(this)->ColorAt(n);
}

/**
\brief Returns the number of polyline vertices.

Returns the number of polyline vertices.
\return int - the number of vertices.
*/
int UtoAttrPolyLineShape::Size() const
{
   return GetImpl(this)->Size();
}

//@}

/** @name Operations */
//@{

/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoAttrPolyLineShape::Clone() const
{
   return new UtoAttrPolyLineShape(*this);
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
int UtoAttrPolyLineShape::Insert(int pos, const UtoPosition& pt)
{
   UtoColor cl(255, 255, 255, 255);
   return GetImpl(this)->Insert(pos, &pt, &cl, 1);
}

/**
\brief Inserts new position(s) and colors before the specified index.

Inserts new position(s) and color(s) before the specified index.
\param pos the index location.
\param pt the position to insert at pos.
\param cl the color to insert at pos.
\return int - <table>
<tr><td>0</td><td>The position and color were inserted successfully.</td></tr>
<tr><td>&lt; 0</td><td>The position and color were not inserted successfully.</td></tr>
</table>
*/
int UtoAttrPolyLineShape::Insert(int pos, const UtoPosition& pt, const UtoColor& cl)
{
   return GetImpl(this)->Insert(pos, &pt, &cl, 1);
}

/**
\brief Inserts new position(s) before the specified index.

Inserts new position(s) before the specified index.
\param pos the index location.
\param pts the position(s) to insert at pos.
\param num the array size of pts.
\return int - <table>
<tr><td>0</td><td>The position was inserted successfully.</td></tr>
<tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
</table>
*/
int UtoAttrPolyLineShape::Insert(int pos, const UtoPosition pts[], int num)
{
   int       retval;
   int       i;
   UtoColor* cls = new UtoColor[num];
   for (i = 0; i < num; ++i)
   {
      cls[i] = UtoColor(255, 255, 255, 255);
   } // this isn't super efficient, but hopefully this will only be used by people who really want vertex color
   retval = GetImpl(this)->Insert(pos, pts, cls, num);
   delete[] cls;
   return retval;
}

/**
\brief Inserts new position(s) and color(s) before the specified index.

Inserts new position(s) and color(s) before the specified index.
\param pos the index location.
\param pts the position(s) to insert at pos.
\param cls the color(s) to insert at pos
\param num the array size of pts.
\return int - <table>
<tr><td>0</td><td>The position and color were inserted successfully.</td></tr>
<tr><td>&lt; 0</td><td>The position and color were not inserted successfully.</td></tr>
</table>
*/
int UtoAttrPolyLineShape::Insert(int pos, const UtoPosition pts[], const UtoColor cls[], int num)
{
   return GetImpl(this)->Insert(pos, pts, cls, num);
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
int UtoAttrPolyLineShape::Remove(int pos)
{
   return GetImpl(this)->Remove(pos);
}

/**
\brief Clears all positions

Clears all positions.
*/
void UtoAttrPolyLineShape::Clear()
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
int UtoAttrPolyLineShape::SetAt(int n, const UtoPosition& pos)
{
   UtoColor col(255, 255, 255, 255);
   return GetImpl(this)->SetAt(n, pos, col);
}

/**
\brief Sets the position and color at a given vertex index.

Sets the position and color at a given vertex index.
\param n the integer index of the polygon vertex.
\param pos the position value to set.
\param col the color value to set
\return int - <table>
<tr><td>0</td><td>The position and color was inserted successfully.</td></tr>
<tr><td>&lt; 0</td><td>The position and color was not inserted successfully.</td></tr>
</table>
*/
int UtoAttrPolyLineShape::SetAt(int n, const UtoPosition& pos, const UtoColor& col)
{
   return GetImpl(this)->SetAt(n, pos, col);
}

//@}
bool UtoAttrPolyLineShape::SetShaders(const char* aVShader, const char* aFShader, std::set<std::string>& aDefList)
{
   return GetImpl(this)->SetShaders(aVShader, aFShader, aDefList);
}


// {secret}
void UtoAttrPolyLineShape::Dump(std::ostream& o)
{
   o << "  Type: UtoAttrPolyLineShape" << std::endl;
   UtoShape::Dump(o);
}
