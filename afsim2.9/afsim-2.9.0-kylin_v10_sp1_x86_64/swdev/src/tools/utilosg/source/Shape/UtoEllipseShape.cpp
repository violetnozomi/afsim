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
// UtoEllipseShape.cpp : implementation file

#include "UtoEllipseShape.hpp"

#include "UtoEllipseShapeImp.hpp"

/**
\ingroup shape
\class UtoEllipseShape
\brief Used to draw ellipses.

The ellipse shape is used to draw ellipses defined by two radii.  The radii can be specified
when creating the ellipse, and can be changed using the SetRadii method.  The ellipse has a
slice angle, which determines the quality of the ellipse edge.  Since the underlying graphics
engine must draw the ellipse as a set of triangles, the slice angle determines the size of
the line segments on the edge of the ellipse.  The slice angle can be modified using the
SetSliceAngle method.  By default, the ellipse will be solid, however, this can be changed
so the ellipse is hollow or draws a line only.  This can be accomplished by using the
EnableLineMode method.  The line width can be modified using the SetLineWidth method.
*/
class UtoEllipseShape;

//{secret}
inline UtoEllipseShapeImp* GetImpl(UtoEllipseShape* shape)
{
   return static_cast<UtoEllipseShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoEllipseShapeImp* GetImpl(const UtoEllipseShape* shape)
{
   return static_cast<const UtoEllipseShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructor */
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoEllipseShape::UtoEllipseShape()
   : UtoShape(new UtoEllipseShapeImp)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param fXRadius the ellipse X radius.
\param fYRadius the ellipse Y radius.
\param fAngle the ellipse slice angle in degrees.
*/
UtoEllipseShape::UtoEllipseShape(double fXRadius, double fYRadius, double fAngle)
   : UtoShape(new UtoEllipseShapeImp(fXRadius, fYRadius, fAngle))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/
UtoEllipseShape::UtoEllipseShape(const UtoEllipseShape& rhs)
   : UtoShape(new UtoEllipseShapeImp(*GetImpl(&rhs)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoEllipseShape::~UtoEllipseShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignemtn operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoEllipseShape & - a reference to the current object.
*/
UtoEllipseShape& UtoEllipseShape::operator=(const UtoEllipseShape& rhs)
{
   UtoEllipseShape T(rhs);
   Swap(T);
   return *this;
}
//@}

// {secret}
void UtoEllipseShape::Dump(std::ostream& o)
{
   o << "  Type: UtoEllipseShape" << std::endl;
   double fXRadius, fYRadius;
   GetImpl(this)->GetRadii(fXRadius, fYRadius);
   o << "  X Radius: " << fXRadius << std::endl;
   o << "  Y Radius: " << fYRadius << std::endl;
   UtoShape::Dump(o);
}

/** @name Accessors */
//@{

/**
\brief Returns the two radii of the ellipse.

Returns the two raddio of the ellipse as out parameters.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param[out] fXRadius the ellipse X radius.
\param[out] fYRadius the ellipse Y radius.
*/
void UtoEllipseShape::GetRadii(double& fXRadius, double& fYRadius)
{
   GetImpl(this)->GetRadii(fXRadius, fYRadius);
}

/**
\brief Sets the minimum radius of the ellipse.

Sets the minimum radius of the ellipse, which is the size of the hole within the center of the circle.
It must be greater than 0, and less than the (maximum) radius.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param aMinRadius the new minimum radius of the circle.
*/
void UtoEllipseShape::SetMinRadius(double aMinRadius)
{
   GetImpl(this)->SetMinRadius(aMinRadius);
}

/**
\brief Returns the current slice angle value in degrees.

Returns the current slice angle value in degrees.
\return double - the current slice angle in degrees.
*/
double UtoEllipseShape::SliceAngle() const
{
   return GetImpl(this)->SliceAngle();
}

/**
\brief Returns the line stipple pattern factor.

Returns the line stipple pattern factor.
\return int - the current line stipple pattern in use.
*/
int UtoEllipseShape::PatternFactor() const
{
   return GetImpl(this)->PatternFactor();
}


/**
\brief Returns the line stipple pattern.

Returns the line stipple pattern.
\return short - the current line stipple pattern.
*/
short UtoEllipseShape::Pattern() const
{
   return GetImpl(this)->Pattern();
}

/**
\brief Returns the current line width in pixels.

Returns the current line width in pixels. Line width is only used
if the ellipse has line mode enabled.
\return double - the current line width in pixels.
*/
double UtoEllipseShape::LineWidth() const
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
UtoShape* UtoEllipseShape::Clone() const
{
   return new UtoEllipseShape(*this);
}

/**
\brief Sets the two radii of the ellipse

Sets the two radii of the ellipse.

Units may be either feet or pixels.  If the shape is being used by a world object
(i.e., UtoEntity, UtoWorld::iterator), then the units are feet.  If the shape is being
used by a window object (i.e., UtoWindow::iterator), then the units are pixels.
\param fXRadius the ellipse X radius.
\param fYRadius the ellipse Y radius.
*/
void UtoEllipseShape::SetRadii(double fXRadius, double fYRadius)
{
   GetImpl(this)->SetRadii(fXRadius, fYRadius);
}

/**
\brief Sets the relative orientation of the shape.

Sets the orientation of the shape. The orientation is relative to the entity the shape is attached to.

Units are radians. To set the absolute heading, call this function
with a heading of absHeading - entityHeading.
\param aOrientation the new relative orientation of the shape.
*/
void UtoEllipseShape::SetOrientation(const UtoOrientation& aOrientation)
{
   return GetImpl(this)->SetOrientationTransform(aOrientation);
}

/**
\brief Sets the ellipse slice angle in degrees.

Sets the ellipse slice angle in degrees.  The slice angle determines how
fine the ellipse edge will be given that the ellipse must be simulated by triangles.
\param angle the slice angle in degrees.
*/
void UtoEllipseShape::SetSliceAngle(double angle)
{
   GetImpl(this)->SetSliceAngle(angle);
}

/**
\brief Sets the line patter and factor.

Sets the line patter and factor which are used to create patterned lines.
\param pattern the line stipple pattern.
\param factor the line stipple pattern factor.
*/
void UtoEllipseShape::SetPattern(int pattern, int factor)
{
   GetImpl(this)->SetPattern((short)pattern, factor);
}

/**
\brief Sets the line width in pixels.

Sets the line width in pixels.  Line width is only used if the ellipse
has line mode enabled.
\param width the new line width in pixels.
*/
void UtoEllipseShape::SetLineWidth(double width)
{
   GetImpl(this)->SetWidth(width);
}

/**
\brief Enables / disables the line mode.

Enables or disables the line mode for the ellipse.  Line mode causes
the ellipse to be drawn as a hollow ellipse.  Otherwise, the ellipse is
drawn as a solid.  By default, the ellipse will be a solid (line mode disabled).
\param enable enables or disables the line mode.
*/
void UtoEllipseShape::EnableLineMode(bool enable)
{
   GetImpl(this)->EnableLineMode(enable);
}

void UtoEllipseShape::SetAngleBounds(double aBeginAngle, double aEndAngle)
{
   GetImpl(this)->SetAngleBounds(aBeginAngle, aEndAngle);
}

/**
\brief Adds sides to make an elliptic cylinder.

Adds sides to an ellipse by duplicating the ellipse face with the given
offset and connecting the resulting edges with a surface.  The offset
position is added to each vertex to get the location of the duplicate
ellipse face.  If the offset is 0,0,0 then the sides are disabled.  If
ellipse line mode has been enabled only the sides are drawn.
\param offset 	Specifies the vertex offset.
*/
void UtoEllipseShape::SetSides(const UtoPosition& offset)
{
   GetImpl(this)->SetSides(offset);
}

/**
\brief Get the current side offset.

Returns the current side offset.  The default is 0,0,0 which is disabled.
\return offset
*/
UtoPosition UtoEllipseShape::Sides() const
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
void UtoEllipseShape::EnableSmoothSides(bool smooth)
{
   GetImpl(this)->EnableSmoothSides(smooth);
}

/**
\brief Get the smooth shading setting.

Returns the current value of side smooth shading.
\return smooth shading
*/
bool UtoEllipseShape::SmoothSides() const
{
   return GetImpl(this)->SmoothSides();
}

/**
\brief Enable lighting on the shape

Enables lighting on the shape
\param aState True for lit, false for unlit
*/
void UtoEllipseShape::EnableLighting(bool aState)
{
   GetImpl(this)->EnableLighting(aState);
}

/**
\brief Sets colors to reflect shader use.

Enables transparency if a shader is attempting to change transparency values of the shape
\param aUsesShader True if shader is used to add transparency
*/
void UtoEllipseShape::SetUsesShader(bool aUsesShader)
{
   GetImpl(this)->SetUsesShader(aUsesShader);
}

//@}
