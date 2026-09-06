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
// UtoCircleShape.cpp : implementation file

#include "UtoCircleShape.hpp"

#include "UtoEllipseShapeImp.hpp"

/**
\ingroup shape
\class UtoCircleShape
\brief Used to draw circles.

The circle shape is used to draw circles.  The circle is defined by a
radius value, which can be specified when creating the circle, and can be
changed using the SetRadius method.  The circle has a slice angle, which
determines the quality of the circle edge.  Since the underlying graphics
engine must draw the circle as a set of triangles, the slice angle
determines the size of the line segments on the edge of the circle.  The
slice angle can be modified using the SetSliceAngle method.  By default,
the circle will be solid, however, this can be changed so the circle is
hollow or draws a line only.  This can be accomplished by using the
EnableLineMode method.  The line width can be modified using the
SetLineWidth method.
*/
class UtoCircleShape;

//{secret}
inline UtoEllipseShapeImp* GetImpl(UtoCircleShape* shape)
{
   return static_cast<UtoEllipseShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoEllipseShapeImp* GetImpl(const UtoCircleShape* shape)
{
   return static_cast<const UtoEllipseShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructor */
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoCircleShape::UtoCircleShape()
   : UtoShape(new UtoEllipseShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator ), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator ), then the units are pixels.

\param fRadius the circle radius
\param fAngle the circle slice angle in degrees.
*/
UtoCircleShape::UtoCircleShape(double fRadius, double fAngle)
   : UtoShape(new UtoEllipseShapeImp(fRadius, fRadius, fAngle))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoCircleShape::UtoCircleShape(const UtoCircleShape& rhs)
   : UtoShape(new UtoEllipseShapeImp(*GetImpl(&rhs)))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param pos the circle position.
\param fRadius the circle radius.
\param the circle slice angle in degrees.
*/
UtoCircleShape::UtoCircleShape(const UtoPosition& pos, double fRadius, double fAngle)
   : UtoShape(new UtoEllipseShapeImp(fRadius, fRadius, fAngle))
{
   GetImpl(this)->SetPosition(pos);
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoCircleShape::~UtoCircleShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignemtn operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoCircleShape & - a reference to the current object.
*/
UtoCircleShape& UtoCircleShape::operator=(const UtoCircleShape& rhs)
{
   UtoCircleShape T(rhs);
   Swap(T);
   return *this;
}
//@}

// Disable lighting to make sure color is displayed correctly
void UtoCircleShape::DisableLighting()
{
   GetImpl(this)->EnableLighting(false);
}

/**
\brief Sets colors to reflect shader use.

Enables transparency if a shader is attempting to change transparency values of the shape
\param aUsesShader True if shader is used to add transparency
*/
void UtoCircleShape::SetUsesShader(bool aUsesShader)
{
   GetImpl(this)->SetUsesShader(aUsesShader);
}

/** @name Accessors */
//@{

/**
\brief Returns the current circle radius.

Returns the current circle radius.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\return double - the radius of the circle.
*/
double UtoCircleShape::Radius() const
{
   double fRadius, dummy;
   GetImpl(this)->GetRadii(fRadius, dummy);
   return fRadius;
}

/**
\brief Returns the current slice angle in degrees.

Returns the current slice angle in degrees.
\return double the current slice angle of the circle.
*/
double UtoCircleShape::SliceAngle() const
{
   return GetImpl(this)->SliceAngle();
}

/**
\brief Returns the line stipple pattern factor.

Returns the line stipple pattern factor.
\return int - the current line stipple pattern in use.
*/
int UtoCircleShape::PatternFactor() const
{
   return GetImpl(this)->PatternFactor();
}


/**
\brief Returns the line stipple pattern.

Returns the line stipple pattern.
\return short - the current line stipple pattern.
*/
short UtoCircleShape::Pattern() const
{
   return GetImpl(this)->Pattern();
}

/**
\brief Returns the current line widht in pixels.

Returns the current line widht in pixels.  Line width is only
used if the circle has line mode enabled.
\return double the current line width of the circle.
*/
double UtoCircleShape::LineWidth() const
{
   return GetImpl(this)->Width();
}
//@}

/** @name Operations */
//@{

/**
\brief Creates a clone of the shape.

Creates a clone of the shape.
\return UtoShape * - returns a new instance of the current shape.
*/
UtoShape* UtoCircleShape::Clone() const
{
   return new UtoCircleShape(*this);
}

/**
\brief Sets the radius of the circle.

Sets the radius of the circle.  The radius must be greater than 0.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param radius the new radius of the circle.
\return bool - value is ignored.
*/
bool UtoCircleShape::SetRadius(double radius)
{
   GetImpl(this)->SetRadii(radius, radius);
   return 0;
}

/**
\brief Sets the relative orientation of the shape.

Sets the orientation of the shape. The orientation is relative to the entity the shape is attached to.

Units are radians. To set the absolute heading, call this function
with a heading of absHeading - entityHeading.
\param aOrientation the new relative orientation of the shape.
*/
void UtoCircleShape::SetOrientation(const UtoOrientation& aOrientation)
{
   return GetImpl(this)->SetOrientationTransform(aOrientation);
}

/**
\brief Sets the minimum radius of the circle.

Sets the minimum radius of the circle, which is the size of the hole within the center of the circle.
It must be greater than 0, and less than the (maximum) radius.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param aMinRadius the new minimum radius of the circle.
*/
void UtoCircleShape::SetMinRadius(double aMinRadius)
{
   GetImpl(this)->SetMinRadius(aMinRadius);
}

/**
\brief Sets the slice angle of the circle in degrees.

Sets the slice angle of the circle in degrees.

The slice angle determines how fine the circle edge will be given that the circle
must be simulated by triangles.
\param angle the slice angle in degrees.
*/
void UtoCircleShape::SetSliceAngle(double angle)
{
   GetImpl(this)->SetSliceAngle(angle);
}

/**
\brief Sets the line patter and factor.

Sets the line patter and factor which are used to create patterned lines.
\param pattern the line stipple pattern.
\param factor the line stipple pattern factor.
*/
void UtoCircleShape::SetPattern(int pattern, int factor)
{
   GetImpl(this)->SetPattern((short)pattern, factor);
}

/**
\brief Sets the circle line width in pixels.

Sets the circle line width in pixels.  Line width is
only used if the circle has line mode enabled.
\param width the new line width in pixels.
*/
void UtoCircleShape::SetLineWidth(double width)
{
   GetImpl(this)->SetWidth(width);
}

/**
\brief Enables / disables the line mode for the circle.

Enables or disables the line mode for the circle.  Line mode causes the
circle to be drawn as a hollow circle.  Otherwise, the circle is drawn
as a solid.  By default, the circle will be a solid (line mode disabled).
\param enable enables or disables the line mode capability of a circle.
*/
void UtoCircleShape::EnableLineMode(bool enable)
{
   GetImpl(this)->EnableLineMode(enable);
}

/**
\brief Adds sides to make a cylinder.

Adds sides to an circle by duplicating the circle face with the given
offset and connecting the resulting edges with a surface.  The offset
position is added to each vertex to get the location of the duplicate
circle face.  If the offset is 0,0,0 then the sides are disabled.  If
circle line mode has been enabled only the sides are drawn.
\param offset 	Specifies the vertex offset.
*/
void UtoCircleShape::SetSides(const UtoPosition& offset)
{
   GetImpl(this)->SetSides(offset);
}

/**
\brief Get the current side offset.

Returns the current side offset.  The default is 0,0,0 which is disabled.
\return offset
*/
UtoPosition UtoCircleShape::Sides() const
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
void UtoCircleShape::EnableSmoothSides(bool smooth)
{
   GetImpl(this)->EnableSmoothSides(smooth);
}

/**
\brief Get the smooth shading setting.

Returns the current value of side smooth shading.
\return smooth shading
*/

bool UtoCircleShape::SmoothSides() const
{
   return GetImpl(this)->SmoothSides();
}

void UtoCircleShape::SetAngleBounds(double aMinAngle, double aMaxAngle)
{
   GetImpl(this)->SetAngleBounds(aMinAngle, aMaxAngle);
}

//@}

// {secret}
void UtoCircleShape::Dump(std::ostream& o)
{
   o << "  Type: UtoCircleShape" << std::endl;
   double fRadius, dummy;
   GetImpl(this)->GetRadii(fRadius, dummy);
   o << "  Radius: " << fRadius << std::endl;
   UtoShape::Dump(o);
}
