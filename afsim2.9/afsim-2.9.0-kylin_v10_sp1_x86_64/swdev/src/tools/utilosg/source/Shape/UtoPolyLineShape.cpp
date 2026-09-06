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
// UtoPolyLineShape.cpp : implementation file

#include "UtoPolyLineShape.hpp"

#include "UtoPolyLineShapeImp.hpp"

/**
\ingroup shape
\class UtoPolyLineShape
\brief Draws lines from multiple vertices.

The polyline shape is used to construct lines consisting of multiple vertices.  Vertices may
be specified at construction using an array of UtoPosition.  Vertices can be added to the
line using the Insert method, and removed using the Remove method.  Vertices can be accessed
using the At method and modified using the SetAt method.

The polyline shape may be configured with a line pattern.  The poly line shape will use the
same line pattern for the entire shape.  This is not defined on a per segement basis.
Following are some of the pre-defined patterns, but other patterns are possible.

   <table>
      <tr><td><b>Pattern</td> <td> Effect </b></td></tr>
      <tr><td>DASHED</td>     <td> \image HTML Dashed.bmp </td></tr>
      <tr><td>LONG DASHED</td><td> \image HTML Long_dashed.bmp </td></tr>
      <tr><td>DOTTED</td>     <td> \image HTML Dotted.bmp </td></tr>
      <tr><td>DOTTED2</td>    <td> \image HTML Dotted2.bmp </td></tr>
      <tr><td>DASH DOT</td>   <td> \image HTML Dash_dot.bmp </td></tr>
      <tr><td>DASH DOT2</td>  <td> \image HTML Dash_dot2.bmp </td></tr>
      <tr><td>SOLID</td>      <td> \image HTML Solid.bmp </td></tr>
   </table>

The polyline shape allows specification of line with using the SetWidth method.
*/
class UtoPolyLineShape;

//{secret}
inline UtoPolyLineShapeImp* GetImpl(UtoPolyLineShape* shape)
{
   return static_cast<UtoPolyLineShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoPolyLineShapeImp* GetImpl(const UtoPolyLineShape* shape)
{
   return static_cast<const UtoPolyLineShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoPolyLineShape::UtoPolyLineShape()
   : UtoShape(new UtoPolyLineShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param pts an array of UtoPosition specifying polyline points.
\param num the size of the pts array.
*/
UtoPolyLineShape::UtoPolyLineShape(const UtoPosition pts[], int num)
   : UtoShape(new UtoPolyLineShapeImp(pts, num))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoPolyLineShape::UtoPolyLineShape(const UtoPolyLineShape& rhs)
   : UtoShape(new UtoPolyLineShapeImp(*GetImpl(&rhs)))
{
}

UtoPolyLineShape::UtoPolyLineShape(UtoPolyLineShapeImp* imp)
   : UtoShape(imp)
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoPolyLineShape::~UtoPolyLineShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoPolyLineShape & - a reference to the current object.
*/
UtoPolyLineShape& UtoPolyLineShape::operator=(const UtoPolyLineShape& rhs)
{
   UtoPolyLineShape T(rhs);
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
UtoPosition UtoPolyLineShape::At(int n)
{
   return GetImpl(this)->At(n);
}

/**
\brief Returns the number of polyline vertices.

Returns the number of polyline vertices.
\return int - the number of vertices.
*/
int UtoPolyLineShape::Size() const
{
   return GetImpl(this)->Size();
}

/**
\brief Returns the line stipple pattern factor.

Returns the line stipple pattern factor.
\return int - the current line stipple pattern in use.
*/
int UtoPolyLineShape::PatternFactor() const
{
   return GetImpl(this)->PatternFactor();
}

/**
\brief Returns the line width.

Returns the line width.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\return double - the current line width.
*/
double UtoPolyLineShape::Width() const
{
   return GetImpl(this)->Width();
}

/**
\brief Returns the line stipple pattern.

Returns the line stipple pattern.
\return short - the current line stipple pattern.
*/
short UtoPolyLineShape::Pattern() const
{
   return GetImpl(this)->Pattern();
}
unsigned int UtoPolyLineShape::Aggregation() const
{
   return GetImpl(this)->Aggregation();
}
//@}

/** @name Operations */
//@{

/**
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoPolyLineShape::Clone() const
{
   return new UtoPolyLineShape(*this);
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
int UtoPolyLineShape::Insert(int pos, const UtoPosition& pt)
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
int UtoPolyLineShape::Insert(int pos, const UtoPosition pts[], int num)
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
int UtoPolyLineShape::Remove(int pos)
{
   return GetImpl(this)->Remove(pos);
}

/**
\brief Clears all positions

Clears all positions.
*/
void UtoPolyLineShape::Clear()
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
int UtoPolyLineShape::SetAt(int n, const UtoPosition& pos)
{
   return GetImpl(this)->SetAt(n, pos);
}

/**
\brief Sets the line patter and factor.

Sets the line patter and factor which are used to create patterned lines.
\param pattern the line stipple pattern.
\param factor the line stipple pattern factor.
*/
void UtoPolyLineShape::SetPattern(int pattern, int factor)
{
   GetImpl(this)->SetPattern((short)pattern, factor);
}

/**
\brief Sets the line width.

Sets the line width.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param width the new width of the line.
*/
void UtoPolyLineShape::SetWidth(double width)
{
   GetImpl(this)->SetWidth(width);
}

/**
\brief Sets the relative orientation of the shape.

Sets the orientation of the shape. The orientation is relative to the entity the shape is attached to.

Units are radians. To set the absolute heading, call this function
with a heading of absHeading - entityHeading.
\param aOrientation the new relative orientation of the shape.
*/
void UtoPolyLineShape::SetOrientation(const UtoOrientation& aOrientation)
{
   return GetImpl(this)->SetOrientationTransform(aOrientation);
}

void UtoPolyLineShape::SetAggregation(unsigned int aggregation)
{
   GetImpl(this)->SetAggregation(aggregation);
}

/**
\brief Adds sides.

Turns a line into ribbon by duplicating each vertex by the offset and
connecting them with a surface.  If offset is 0,0,0 only the line is
drawn, otherwise only the surface is drawn.
\param offset 	Specifies the vertex offset.
*/
void UtoPolyLineShape::SetSides(const UtoPosition& offset)
{
   GetImpl(this)->SetSides(offset);
}

/**
\brief Get the current side offset.

Returns the current side offset.  The default is 0,0,0 which is disabled.
\return offset
*/
UtoPosition UtoPolyLineShape::Sides() const
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
void UtoPolyLineShape::EnableSmoothSides(bool smooth)
{
   GetImpl(this)->EnableSmoothSides(smooth);
}

/**
\brief Get the smooth shading setting.

Returns the current value of side smooth shading.
\return smooth shading
*/
bool UtoPolyLineShape::SmoothSides() const
{
   return GetImpl(this)->SmoothSides();
}

/**
\brief Enable anti alias lines.

\param smooth 	True for anti alias lines.
*/
void UtoPolyLineShape::EnableAntiAliasLines(bool smooth)
{
   GetImpl(this)->EnableAntiAliasLines(smooth);
}

/**
\brief Get the anti alias lines setting.

Returns the current value of side smooth shading.
\return anti alias lines
*/
bool UtoPolyLineShape::AntiAliasLines() const
{
   return GetImpl(this)->AntiAliasLines();
}
/**
\brief Sets the depth render bin number

Sets the depth render bin number changing the draw order of the shape.
*/
void UtoPolyLineShape::SetDepthBin(int binNumber)
{
   return GetImpl(this)->SetDepthBin(binNumber);
}

//@}

// {secret}
void UtoPolyLineShape::Dump(std::ostream& o)
{
   o << "  Type: UtoPolyLineShape" << std::endl;
   UtoShape::Dump(o);
}

void UtoPolyLineShape::SetDepthTesting(bool bDepthTest)
{
   GetImpl(this)->SetDepthTesting(bDepthTest);
}

/** @name Type Definitions */
//@{
/**
\enum UtoPolyLineShape::Patterns
\brief Drawing patterns.
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::DASHED
\brief 0xAAAA
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::LONG_DASH
\brief 0x00FF
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::DOTTED
\brief 0x1111
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::DOTTED2
\brief 0x0101
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::DASH_DOT
\brief 0x264C
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::DASH_DOT2
\brief 0x1C47
*/

/**
\var UtoPolyLineShape::Patterns UtoPolyLineShape::SOLID
\brief 0xFFFF
*/
//@}
