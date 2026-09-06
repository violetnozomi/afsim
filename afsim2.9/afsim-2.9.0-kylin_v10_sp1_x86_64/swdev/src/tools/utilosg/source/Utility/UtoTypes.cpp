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

#include "Uto.hpp"
#include "UtoPriv.hpp"


/**
\ingroup types
\class UtoOrientation
\brief Represents Euler angles.

UtoOrientation represents the Euler angles that describe the
orientation of a reference frame relative to a local tangent plane on the ground.  The
order of rotation is first yaw, then pitch, and finally roll (psi, theta, phi).  Yaw
measures the angle from north (+ clockwise), pitch measures the angle from level with
the local tangent plane (+ nose up), and roll measure a roll angle (+right side down) .

UtoOrientation is always specified as degrees.
*/
struct UtoOrientation;

/**
\ingroup types
\class UtoPoint
\brief A 2D point.
*/
struct UtoPoint;

/**
\ingroup types
\class UtoRect
\brief Represents an area defined by two points.
*/
struct UtoRect;

/**
\ingroup types
\class UtoSize
\brief Represents width and height.
*/
struct UtoSize;

/**
\ingroup types
\class UtoPosOrient
\brief Encapsulates both position and orientation.
*/

/**
\ingroup types
\class Utok::NameOverlay
\brief Provides access to bound overlays.
*/

/**
\ingroup types
\class Utok::NameShape
\brief Provides access to bound shapes.
*/

/**
\ingroup types
\class Utok::TraitValue
\brief Provides access to bound traits.
*/

/** @name Constructor & Destructor */
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoOrientation::UtoOrientation()
   : yaw(0)
   , pitch(0)
   , roll(0)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param y yaw
\param p pitch
\param r roll
*/
UtoOrientation::UtoOrientation(double y, double p, double r)
   : yaw(y)
   , pitch(p)
   , roll(r)
{
}
//@}


/** @name Constructor & Destructor */
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoPoint::UtoPoint()
   : x(0)
   , y(0)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param x1 x
\param y1 y
*/
UtoPoint::UtoPoint(double x1, double y1)
   : x(x1)
   , y(y1)
{
}
//@}

/** @name Assignment */
//@{
/**
\brief Assignemtn operator.

This will set the current object on the LHS to the value given on the RHS.

\param pos the point to be copied.
\return UtoPoint & - a reference to the current object.
*/
UtoPoint& UtoPoint::operator=(const UtoPosition& pos)
{
   x = pos.x();
   y = pos.y();
   return *this;
}
//@}

UtoSize const UtoSize::null;

/** @name Constructor & Destructor */
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoSize::UtoSize()
   : cx(0)
   , cy(0)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param cx1 cx
\param cy1 cy
*/
UtoSize::UtoSize(double cx1, double cy1)
   : cx(cx1)
   , cy(cy1)
{
}
//@}

UtoRect const UtoRect::null;

/** @name Constructor & Destructor */
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoRect::UtoRect()
   : top(0)
   , bottom(0)
   , left(0)
   , right(0)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param t top
\param b bottom
\param l left
\param r right
*/
UtoRect::UtoRect(double t, double b, double l, double r)
   : top(t)
   , bottom(b)
   , left(l)
   , right(r)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param ptTopLeft the top, left point.
\param ptBottomRight the bottom, right point.
*/
UtoRect::UtoRect(const UtoPoint& ptTopLeft, const UtoPoint& ptBottomRight)
   : top(ptTopLeft.y)
   , bottom(ptBottomRight.y)
   , left(ptTopLeft.x)
   , right(ptBottomRight.x)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param ptTopLeft the top, left point.
\param size the width and height of the UtoRect.
*/
UtoRect::UtoRect(const UtoPoint& ptTopLeft, const UtoSize& size)
   : top(ptTopLeft.y)
   , bottom(ptTopLeft.y - size.cy)
   , left(ptTopLeft.x)
   , right(ptTopLeft.x + size.cx)
{
}
//@}

/** @name Operations */
//@{
/**
\brief Sets the top, left position.

Sets the top, left position.  The x and y components
will be used for positions.
\param pos the top, left position.
*/
void UtoRect::TopLeft(const UtoPosition& pos)
{
   top  = pos.y();
   left = pos.x();
}

/**
\brief Sets the bottom, right position.

Sets the bottom, right position.  The x and y components
will be used for positions.
\param pos the bottom, right position.
*/
void UtoRect::BottomRight(const UtoPosition& pos)
{
   bottom = pos.y();
   right  = pos.x();
}
//@}

/** @name Constructor & Destructor */
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoBox::UtoBox()
   : xmin(0)
   , xmax(0)
   , ymin(0)
   , ymax(0)
   , zmin(0)
   , zmax(0)
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param _xmin
\param _xmax
\param _ymin
\param _ymax
\param _zmin
\param _zmax
*/
UtoBox::UtoBox(float _xmin, float _xmax, float _ymin, float _ymax, float _zmin, float _zmax)
   : xmin(_xmin)
   , xmax(_xmax)
   , ymin(_ymin)
   , ymax(_ymax)
   , zmin(_zmin)
   , zmax(_zmax)
{
}
//@}

/////////////////////////////////////////////////////////
// Extra type defs for doxygen
////////////////////////////////////////////////////////
/** @name Data Members */
//@{
/**
\var double & UtoPosition::x
\brief The +East position component.

The +East position component.
*/

/**
\var double & UtoPosition::y
\brief The +North position component.

The +North position component.
*/

/**
\var double & UtoPosition::z
\brief The +Up position component.

The +Up position component.
*/

/**
\var double UtoOrientation::pitch
\brief The theta orientation component.

The theta orientation component.
*/

/**
\var double UtoOrientation::roll
\brief The phi orientation component.

The phi orientation component.
*/

/**
\var double UtoOrientation::yaw
\brief The psi orientation component.

The psi orientation component.
*/

/**
\var double UtoRect::bottom
\brief The bottom edge of the rectangle.

The bottom edge of the rectangle.
*/

/**
\var double UtoRect::left
\brief The left edge of the rectangle.

The left edge of the rectangle.
*/

/**
\var double UtoRect::right
\brief The right edge of the rectangle.

The right edge of the rectangle.
*/

/**
\var double UtoRect::top
\brief The top edge of the rectangle.

The top edge of the rectangle.
*/

/**
\var double UtoSize::cx
\brief The width component.

The width component.
*/

/**
\var double UtoSize::cy
\brief The height component.

The height component.
*/

/**
\var UtoPosition UtoPosOrient::pos
\brief The position component.

The position component.
*/

/**
\var UtoOrientation UtoPosOrient::orient
\brief The orientation component.

The orientation component.
*/

/**
\ingroup types
\typedef HWND UtoWindowId
\brief The window type ID.

The window type ID.
*/

/**
\var std::string Utok::NameOverlay::name
\brief The bound name for the overlay.

The bound name for the overlay.
*/

/**
\var UtoOverlay * Utok::NameOverlay::overlay
\brief A pointer to the bound overlay.

A pointer to the bound overlay.
*/

/**
\var const std::string & Utok::NameShape::name
\brief The bound name for the shape.

The bound name for the shape.
*/

/**
\var UtoShape & Utok::NameShape::shape
\brief A reference to the bound shape.

A reference to the bound shape.
*/

/**
\var const std::string & Utok::TraitValue::name
\brief The bound name for the trait.

The bound name for the trait.
*/

/**
\var const std::string & Utok::TraitValue::value
\brief A reference to the bound value.

A reference to the bound value.
*/
//@}

///////////////////////////////////////////////////
// class no_resource - doxygen documentation
///////////////////////////////////////////////////
/**
\class no_resource
\ingroup resource
\brief An exception thrown by Uto.

An exception thrown by Uto.
*/

/** @name Constructors */
//@{
/**
\fn no_resource::no_resource( const std::string & name )
\brief Creates an instance of the object.

Creates an instance of the object.
\param name the type of exception being thrown.
*/
//@}

/** @name Accessors */
//@{
/**
\fn virtual const char* no_resourc::what() const
\brief Returns the type of exception.

Returns the type of exception.
\return const char * - the type of exception thrown.
*/
//@}
