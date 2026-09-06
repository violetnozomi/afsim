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
// GLShape.cpp : implementation file

#include "UtoGLShape.hpp"

#include "UtoGLShapeImp.hpp"

/**
\ingroup shape
\class UtoGLShape
\brief Allows for custome OpenGL drawing.

The GL shape is used to call a user-defined callback function for
custom OpenGL drawing.  The user-defined callback may be a free
function with the signature:
\code
         int Draw(void* data)
\endcode
Or a class method with the signature:
\code
         int MyClass::Draw()
\endcode
When creating GL shapes the "this" pointer or data object pointer and
function must be specified.  This information can be changed later
using the SetFn method.  For more usage information on using the GL
shape callback function, see the SetFn documentation.

It is important to note that the SetSize function must be called before
trying to show the GL shape.  Otherwise, UTO could only 1) always call
the callback even when off the display, or 2) think the shape has no size
and never shows the shape.  The current implementation opts for #2.

The GL shape extends the helper class UtoGLShapeBase.  This is done
solely as an implementation issue.  UtoGLShape makes use of template
functions to make type-safe method calls into the UTO DLL, and
UtoGLShapeBase provides the necessary workaround for the Visual C++
compiler.

The OpenGL matrices are setup for the current shape location and
rotations.  If the callback were to draw a point at 0,0,0 the
point would be drawn in
the center of the shape and 10,0,0 would be off an airplane's right
as compared to an aircraft model at the same location.
Refer to the MfcDemo sample, where the UtoGLShape is used to implement
approach plates (thought it currently isn't being executed).

Setting the color of a GLShape doesn't have any effect on what is actually
drawn, but the same color will be returned from the GLShape accessor
function.  Setting the transparency doesn't effect the geometry drawn,
but it does change the drawing order.  If it is completely opaque it
will be drawn along with all the opaque objects, if it is at all transparent,
the callback will be called depth sorted according to the bounding volume
compared to other transparent objects.

\note The user-defined callback function must restore the GL render
state back to its original state before returning.  Otherwise,
undefined behavior may result.
*/
class UtoGLShape;

/**
\ingroup shape
\class UtoGLShapeBase
\brief Base interface class for custome OpenGL drawing.

This class is used as an implementation helper for UtoGLShape.  It is not intended for
use by a user.
*/
class UtoGLShapeBase;

//{secret}
inline UtoGLShapeImp* GetImpl(UtoGLShapeBase* shape)
{
   return static_cast<UtoGLShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoGLShapeImp* GetImpl(const UtoGLShapeBase* shape)
{
   return static_cast<const UtoGLShapeImp*>(shape->Impl());
}

UtoGLShapeBase::UtoGLShapeBase()
   : UtoShape(new UtoGLShapeImp)
{
}

UtoGLShapeBase::UtoGLShapeBase(const UtoGLShapeBase& rhs)
   : UtoShape(new UtoGLShapeImp(*GetImpl(&rhs)))
{
}

/** @name Constructor & Destructor */
//@{
/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoGLShapeBase::~UtoGLShapeBase() {}
//@}

// {secret}
void UtoGLShapeBase::Dump(std::ostream& o)
{
   o << "  Type: UtoGLShape" << std::endl;
   UtoShape::Dump(o);
}

// {secret}
void UtoGLShapeBase::Set_i(DrawFunctor* func)
{
   GetImpl(this)->SetFn(func);
}

/** @name Operations */
//@{
/**
\brief Sets the size of the bounding volume for the geometry.

Sets the size of the bounding volume for the geometry. The bounding
volume is used to determine when the shape needs to be drawn. If the
bounding volume is calculated to be off screen the callback will
not be made.

Calling this function is important in order to get the callback function called.
\param radius size of the bounding sphere radius for the geometry in feet.
*/
void UtoGLShapeBase::SetSize(double radius)
{
   GetImpl(this)->SetSize(radius);
}

/**
\brief Sets the size of the bounding volume for the geometry.

Sets the size of the bounding volume for the geometry. The bounding
volume is used to determine when the shape needs to be drawn. If the
bounding volume is calculated to be off screen the callback will
not be made.

Calling this function is important in order to get the callback function called.
\param cx x size of the bounding box for the geometry in feet.
\param cy y size of the bounding box for the geometry in feet.
*/
void UtoGLShapeBase::SetSize(double cx, double cy)
{
   GetImpl(this)->SetSize(cx, cy);
}

osg::RenderInfo* UtoGLShapeBase::GetRenderInfo() const
{
   return GetImpl(this)->GetRenderInfo();
}

//@}

/** @name Type Definitions */
//@{
/**
\typedef UtoGLShapeBase::Utok::DrawFunctor UtoGLShapeBase::DrawFunctor;
Callback
*/
//@}

///////////////////////////////////////////////////////
///  UtoGLShape
///////////////////////////////////////////////////////

/** @name Constructor & Destructor */
//@{

/**
\fn template < class T, class Func > UtoGLShape::UtoGLShape ( T * ptr, Func fn )
\brief Creates an instance of the object.

Creates an instance of the object.
\param ptr the callback data for the callback function.
\param fn the callback function.
*/

/**
\fn UtoGLShape::UtoGLShape( const UtoGLShape & rhs )
\brief Creates an instance of the object.

Creates an instance of the object.
\param rhs the shape instance to be copied.
*/

/**
\fn virtual UtoGLShape::~UtoGLShape( )
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
//@}

/** @name Assignment */
//@{
/**
\fn UtoGLShape & UtoGLShape::operator = ( const UtoGLShape & rhs )
\brief Assignemtn operator.

This will set the current object on the LHS to the value given on the RHS.

\param rhs the shape instance to be copied.
\return UtoGLShape & - a reference to the current object.
*/
//@}

/** @name Operations */
//@{
/**
\fn virtual UtoShape * UtoGLShape::Clone( ) const
\brief Creates a clone of the shape

Creates a clone of the shape.

\return UtoShape * - returns a new instance of the current shape.
*/

/**
\fn template < class T, class Func > void UtoGLShape::SetFn ( T * ptr, Func fn )
\brief Sets the callback function and data.

Sets the callback function and data.

If passing a 'free' function (not a class method), ptr is the callback
data passed as the single parameter to fn:

\code
int fn(void* ptr)

In some function:
   my_struct* ptr = ...;
   UtoGLShape glshape (...);
   glshape.SetFn (ptr, fn);
\endcode

Otherwise if passing a class member function, ptr is the 'this'
pointer of the class:

\code
int MyClass::fn()

In some function:
   UtoGLShape glshape (...);
   glshape.SetFn (this, std::mem_fun (&MyClass::fn));
\endcode

The value returned isn't used, it just needs to have an int as
the return type.
\param ptr the callback data for the callback function.
\param fn the callback function.
*/
//@}
