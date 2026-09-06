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
// UtoPolygonShape.cpp : implementation file

#include "UtoPolygonShape.hpp"

#include "UtoPolygonShapeImp.hpp"

/**
\ingroup shape
\class UtoPolygonShape
\brief Draws polygons from multiple vertices.

The polygon shape is used to construct polygons consisting of multiple vertices
(i.e.  rectangle, pentagon, hexagon, etc).  Vertices may be specified at
construction using an array of UtoPosition.  Vertices can be added to the line
using the Insert method, and removed using the Remove method.  Vertices can be
accessed using the At method and modified using the SetAt method.

The polygon shape currently can only be a solid.  If a hollow polygon is
desired, the UtoPolyLineShape can be used with the last vertex closing the
line.

\note The polygon must be a simple convex polygon.  Concave polygons
are not supported.  A polygon where the edges intersect would be
non-simple, ie a nonplanar polygon viewed edge on.  See EnableComplexPoly
to remove this restriction.  Also the normal of the entire polygon is
set using the first three verticies.
*/
class UtoPolygonShape;

//{secret}
inline UtoPolygonShapeImp* GetImpl(UtoPolygonShape* shape)
{
   return static_cast<UtoPolygonShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoPolygonShapeImp* GetImpl(const UtoPolygonShape* shape)
{
   return static_cast<const UtoPolygonShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoPolygonShape::UtoPolygonShape()
   : UtoShape(new UtoPolygonShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoPolygonShape::UtoPolygonShape(const UtoPolygonShape& rhs)
   : UtoShape(new UtoPolygonShapeImp(*GetImpl(&rhs)))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param pts an array of UtoPosition specifying polygon points.
\param num the size of the pts array.
*/
UtoPolygonShape::UtoPolygonShape(const UtoPosition pts[], int num)
   : UtoShape(new UtoPolygonShapeImp(pts, num))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoPolygonShape::~UtoPolygonShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoPolygonShape & - a reference to the current object.
*/
UtoPolygonShape& UtoPolygonShape::operator=(const UtoPolygonShape& rhs)
{
   UtoPolygonShape T(rhs);
   Swap(T);
   return *this;
}
//@}

/** @name Accessors */
//@{

/**
\brief Returns the position of a given vertex at the specified index.

Returns the position of a given vertex at the specified index.
\param n the integer index of the polygon vertex.
\return UtoPosition - the position of the vertex.
*/
UtoPosition UtoPolygonShape::At(int n)
{
   return GetImpl(this)->At(n);
}

/**
\brief Returns the number of polygon vertices.

Returns the number of polygon vertices.
\return int - the number of vertices.
*/
int UtoPolygonShape::Size() const
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
UtoShape* UtoPolygonShape::Clone() const
{
   return new UtoPolygonShape(*this);
}

/**
\brief Inserts new positions(s) before the specified index.

Inserts new positions(s) before the specified index.
\param pos the insert location.
\param pt the position to insert.
\return int - <table>
                  <tr><td>0</td><td>The position was inserted successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoPolygonShape::Insert(int pos, const UtoPosition& pt)
{
   return GetImpl(this)->Insert(pos, &pt, 1);
}

/**
\brief Inserts new positions(s) before the specified index.

Inserts new positions(s) before the specified index.
\param pos the insert location.
\param pts the position(s) to insert.
\param num the size of pts.
\return int - <table>
                  <tr><td>0</td><td>The position was inserted successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoPolygonShape::Insert(int pos, const UtoPosition pts[], int num)
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
int UtoPolygonShape::Remove(int pos)
{
   return GetImpl(this)->Remove(pos);
}

void UtoPolygonShape::Clear()
{
   GetImpl(this)->Clear();
}

/**
\brief Sets the position at a given vertex index.

Sets the position at a given vertex index, which must already exist.
\param n the integer index of the polygon vertex.
\param pos the position value to set.
\return int - <table>
                  <tr><td>0</td><td>The position was updated successfully.</td></tr>
                  <tr><td>&lt; 0</td><td>The position was not inserted successfully.</td></tr>
              </table>
*/
int UtoPolygonShape::SetAt(int n, const UtoPosition& pos)
{
   return GetImpl(this)->SetAt(n, pos);
}

/**
\brief Enables extra calculations for complex polygons.

If set to true the restriction of simple convex polygons is removed at
the cost of extra processing to break the polygon into additional
geometry and then drawing that extra geometry.  The tessleation of the
polygon is performed the first time the shape is drawn after the
geometry has been changed.  This allows concave and self intersecting
polygons to be drawn.  For self intersecting polygons enclosed areas
will only be drawn if the winding count is odd.  See gluTessProperty
for further details.
\param enable 	enables or disables complex polygons.
*/
void UtoPolygonShape::EnableComplexPoly(bool enable)
{
   GetImpl(this)->EnableComplexPoly(enable);
}

/**
\brief Returns if complex polygon processing is enabled.

Returns if complex polygon processing is enabled for this polygon.
\return bool - <table>
                  <tr><td>true</td><td>Complex polygon processing is enabled.</td></tr>
                  <tr><td>false</td><td>Complex polygon processing is disabled.</td></tr>
              </table>
*/
bool UtoPolygonShape::ComplexPoly() const
{
   return GetImpl(this)->ComplexPoly();
}

/**
\brief Enables lighting.

If set to true the polygon will be lit.
\param enable 	enables or disables lighting.
*/
void UtoPolygonShape::EnableLighting(bool aState)
{
   GetImpl(this)->EnableLighting(aState);
}

/**
\brief Adds sides to a polygon.

Adds sides to a polygon by duplicating the polygon face with the given
offset and connecting the resulting edges with a surface.  The offset
position is added to each vertex to get the location of the duplicate
polygon face.  If the offset is 0,0,0 then the sides are disabled.
\param offset 	Specifies the vertex offset.
*/
void UtoPolygonShape::SetSides(const UtoPosition& offset)
{
   GetImpl(this)->SetSides(offset);
}

/**
\brief Get the current side offset.

Returns the current side offset.  The default is 0,0,0 which is disabled.
\return offset
*/
UtoPosition UtoPolygonShape::Sides() const
{
   return GetImpl(this)->Sides();
}

/**
\brief Enable smooth shading for the sides.

By default sides are drawn with flat shading, that is the normals are
perpendicular to the surface for all four corners.  If set to true the
normals for each adjacent side are added (averaged) and assigned to the
vertices on the edge.  The smooth shading hides the edge.
\param smooth 	True for smooth shaded sides, false for flat faces.
*/
void UtoPolygonShape::EnableSmoothSides(bool smooth)
{
   GetImpl(this)->EnableSmoothSides(smooth);
}

/**
\brief Get the smooth shading setting.

Returns the current value of side smooth shading.
\return smooth shading
*/
bool UtoPolygonShape::SmoothSides() const
{
   return GetImpl(this)->SmoothSides();
}

//@}

// {secret}
void UtoPolygonShape::Dump(std::ostream& o)
{
   o << "  Type: UtoPolygonShape" << std::endl;
   UtoShape::Dump(o);
}
