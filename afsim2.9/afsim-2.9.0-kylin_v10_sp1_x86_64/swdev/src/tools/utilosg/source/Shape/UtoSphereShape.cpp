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

#include "UtoSphereShape.hpp"

#include "UtoSphereShapeImp.hpp"

// obtains the correct impl pointer from the basic shape
inline UtoSphereShapeImp* GetImpl(UtoSphereShape* pSphere)
{
   return static_cast<UtoSphereShapeImp*>(pSphere->Impl());
}

inline const UtoSphereShapeImp* GetImpl(const UtoSphereShape* pSphere)
{
   return static_cast<const UtoSphereShapeImp*>(pSphere->Impl());
}

/**
\ingroup shape
\class UtoSphereShape
\brief Shows a 3D sphere with slice and stack lines.

The sphere shape is used to construct a three dimensional sphere with
slice and stack lines.  This shape allow manipulation of the slice and
stack line per sphere and is allowed to hide the lines, if need be.  If
hiding both lines, the shape of the sphere will be viewed as a disk on
the 3D view, unless lighting is applied to the shape.
*/

/** @name Constructor & Destructors **/
//@{
/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rRadius the radius of the sphere defaulted to 1.0.
\param nSlices the number of vertical slices defaulted to 30.
\param nStacks the number of horizontal slices defaulted to 30.
\param rCFactorX the x compression factor defaulted to 1.0.
\param rCFactorY the y compression factor defaulted to 1.0.
\param rCFactorZ the z compression factor defaulted to 1.0.
\param bShowSlices shows the slice lines defaulted to true.
\param bShowStacks shows the stack lines defaulted to true.
*/
UtoSphereShape::UtoSphereShape(const double& rRadius,
                               unsigned int  nSlices,
                               unsigned int  nStacks,
                               const double& rCFactorX,
                               const double& rCFactorY,
                               const double& rCFactorZ,
                               bool          bShowSlices,
                               bool          bShowStacks)
   : UtoShape(new UtoSphereShapeImp(rRadius, nSlices, nStacks, rCFactorX, rCFactorY, rCFactorZ, bShowSlices, bShowStacks))
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rSphere the shape instance to be copied.
*/
UtoSphereShape::UtoSphereShape(const UtoSphereShape& rSphere)
   : UtoShape(new UtoSphereShapeImp(*GetImpl(&rSphere)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoSphereShape::~UtoSphereShape() {}
//@}

/** @name Assignment */
//@{
/**
\brief Assignment operator.

This will set the current object on the LHS to the value given on the RHS.

\param rSphere the shape instance to be copied.
\return UtoSphereShape & - a reference to the current object.
*/
UtoSphereShape& UtoSphereShape::operator=(const UtoSphereShape& rSphere)
{
   // validate the shape is not the same
   if (this == &rSphere)
      return *this;

   return *this;
}
//@}


/** @name Accessors */
//@{
/**
\brief Returns the radius of the sphere.

Returns the radius of the sphere.
\return const double & - the current radius.
*/
double UtoSphereShape::Radius() const
{
   return GetImpl(this)->Radius();
}

/**
\brief Returns the number of vertical slices.

Returns the number of vertical slices.
\return unsigned int - the number of vertical slices.
*/
unsigned int UtoSphereShape::Slices() const
{
   return GetImpl(this)->Slices();
}

/**
\brief Returns the number of horizontal stacks.

Returns the number of horizontal stacks.
\return unsigned int - the number of horizontal stacks.
*/
unsigned int UtoSphereShape::Stacks() const
{
   return GetImpl(this)->Stacks();
}

/**
\brief Returns the x compression factor.

Returns the x compression factor.
\return const double & - the x compression factor.
*/
const double& UtoSphereShape::GetCompressionFactorX() const
{
   return GetImpl(this)->GetCompressionFactorX();
}

/**
\brief Returns the y compression factor.

Returns the y compression factor.
\return const double & - the y compression factor.
*/
const double& UtoSphereShape::GetCompressionFactorY() const
{
   return GetImpl(this)->GetCompressionFactorY();
}

/**
\brief Returns the z compression factor.

Returns the z compression factor.
\return const double & - the z compression factor.
*/
const double& UtoSphereShape::GetCompressionFactorZ() const
{
   return GetImpl(this)->GetCompressionFactorZ();
}

/**
\brief Indicates the visibility of slice lines.

Indicates the visibility of slice lines.
\return <table><tr><td>true</td><td>Slice lines are visible.</td></tr>
               <tr><td>false</td><td>Slice lines are not visible.</td></tr></table>
*/
bool UtoSphereShape::SliceLinesVisible() const
{
   return GetImpl(this)->SliceLinesVisible();
}

/**
\brief Indicates the visibility of stack lines.

Indicates the visibility of stack lines.
\return <table><tr><td>true</td><td>Stack lines are visible.</td></tr>
               <tr><td>false</td><td>Stack lines are not visible.</td></tr></table>
*/
bool UtoSphereShape::StackLinesVisible() const
{
   return GetImpl(this)->StackLinesVisible();
}
//@}

/** @name Operations */
//@{
/**
\brief Sets the radius of the sphere.

Sets teh radius of the sphere.  The radius is equal to world units.
\param rRadius The radius of the sphere in world units.
*/
void UtoSphereShape::SetRadius(double rRadius)
{
   GetImpl(this)->SetRadius(rRadius);
}

void UtoSphereShape::SetMinimumRadius(double rMinRadius)
{
   GetImpl(this)->SetMinimumRadius(rMinRadius);
}

/**
\brief Sets the minimum altitude of the sphere.

Sets the minimum altitude, which determines what absolute altitude
the bottom of the sphere should be cut off at.
\param aMinAltitude the minimum altitude.
*/
void UtoSphereShape::SetMinAltitude(double aMinAltitude)
{
   GetImpl(this)->SetMinAltitude(aMinAltitude);
}

/**
\brief Sets the maximum altitude of the sphere.

Sets the maximum altitude, which determines what absolute altitude
the top of the sphere should be cut off at.
\param aMaxAltitude the maximum altitude.
*/
void UtoSphereShape::SetMaxAltitude(double aMaxAltitude)
{
   GetImpl(this)->SetMaxAltitude(aMaxAltitude);
}

/**
\brief Sets the relative orientation of the shape.

Sets the orientation of the shape. The orientation is relative to the entity the shape is attached to.

Units are radians. To set the absolute heading, call this function
with a heading of absHeading - entityHeading.
\param aOrientation the new relative orientation of the shape.
*/
void UtoSphereShape::SetOrientation(const UtoOrientation& aOrientation)
{
   return GetImpl(this)->SetOrientationTransform(aOrientation);
}

/**
\brief Sets the start and stop angles of the sphere.

Sets the start and stop angles, which determine what vertical slice
of the sphere is drawn. Defaults to 0, 360, or the full sphere.
\param aStartAngle the start angle, in degrees.
\param aStopAngle the stop angle, in degrees.
*/
void UtoSphereShape::SetAngleBounds(double aStartAngle, double aStopAngle)
{
   GetImpl(this)->SetAngleBounds(aStartAngle, aStopAngle);
}

/**
\brief Sets the number of vertical slice lines.

Sets the number of vertical slice lines.  This number will
determine how detailed the sphere will be viewed 360 degrees around.
To calculate the slice value from a predetermined degree, use the equation
360 / degrees = nSlices.
\param nSlices the number of slices used to construct the sphere.
*/
void UtoSphereShape::SetSlices(unsigned int nSlices)
{
   GetImpl(this)->SetSlices(nSlices);
}

/**
\brief Sets the number of horizontal stack lines.

Sets the number of horizontal stack lines. This number will
determine how detailed the sphere will be viewed 180 degrees from top to bottom.
To calculate the stack value from a predetermined degree, use the equation
180 / degrees = nStacks.
\param nStacks the number of stacks used to construct the sphere.
*/
void UtoSphereShape::SetStacks(unsigned int nStacks)
{
   GetImpl(this)->SetStacks(nStacks);
}

/**
\brief Sets the line color of the sphere.

Sets the color of the lines covering the surface of the sphere.  This allows the sphere
to be compressed or stretched along its x axis.
\param aColor the line color.
*/
void UtoSphereShape::SetLineColor(const UtoColor& aColor)
{
   return GetImpl(this)->SetLineColor(aColor);
}

/**
\brief Sets the x compression factor of the sphere.

Sets the x compression factor of the sphere.  This allows the sphere
to be compressed or stretched along its x axis.
\param rFactor the compression factor.
*/
void UtoSphereShape::SetCompressionFactorX(const double& rFactor)
{
   GetImpl(this)->SetCompressionFactorX(rFactor);
}

/**
\brief Sets the y compression factor of the sphere.

Sets the y compression factor of the sphere.  This allows the sphere
to be compressed or stretched along its y axis.
\param rFactor the compression factor.
*/
void UtoSphereShape::SetCompressionFactorY(const double& rFactor)
{
   GetImpl(this)->SetCompressionFactorY(rFactor);
}

/**
\brief Sets the z compression factor of the sphere.

Sets the z compression factor of the sphere.  This allows the sphere
to be compressed or stretched along its z axis.
\param rFactor the compression factor.
*/
void UtoSphereShape::SetCompressionFactorZ(const double& rFactor)
{
   GetImpl(this)->SetCompressionFactorZ(rFactor);
}

/**
\brief Sets the radius, slice, and stack values.

Sets the radius, slice, and stack values.
\param rRadius the radius of the sphere in world units.
\param nSlices the number of slices used to construct the sphere.
\param nStacks the number of stacks used to construct the sphere.
\param rCFactorX the x compression factor used to construct the sphere.
\param rCFactorY the y compression factor used to construct the sphere.
\param rCFactorZ the z compression factor used to construct the sphere.
\param rMinRadius the minimum radius of the sphere in world units
*/
void UtoSphereShape::Set(const double& rRadius,
                         unsigned int  nSlices,
                         unsigned int  nStacks,
                         const double& rCFactorX,
                         const double& rCFactorY,
                         const double& rCFactorZ,
                         const double& rMinRadius)
{
   GetImpl(this)->Set(rRadius, rMinRadius, nSlices, nStacks, rCFactorX, rCFactorY, rCFactorZ);
}

/**
\brief Shows the slice lines of the sphere.

Shows the slice lines of the sphere.  The slice lines will take
on the color of the sphere, minus the alpha value.
\param bShow -
<table><tr><td>true</td><td>Show slice lines.</td></tr>
       <tr><td>false</td><td>Hide slice lines.</td></tr></table>
*/
void UtoSphereShape::ShowSliceLines(bool bShow)
{
   GetImpl(this)->ShowSliceLines(bShow);
}

/**
\brief Shows the stack lines of the sphere.

Shows the stack lines of the sphere.  The stack lines will take
on the color of the sphere, minus the alpha value.
\param bShow -
<table><tr><td>true</td><td>Show stack lines.</td></tr>
       <tr><td>false</td><td>Hide stack lines.</td></tr></table>
*/
void UtoSphereShape::ShowStackLines(bool bShow)
{
   GetImpl(this)->ShowStackLines(bShow);
}

void UtoSphereShape::ShowSphereGeometry(bool bShow)
{
   GetImpl(this)->ShowSphereGeometry(bShow);
}

void UtoSphereShape::SetLineWidth(double width)
{
   GetImpl(this)->SetLineWidth(width);
}

double UtoSphereShape::LineWidth() const
{
   return GetImpl(this)->LineWidth();
}

void UtoSphereShape::SetPattern(int pattern, int factor /* = 1 */)
{
   GetImpl(this)->SetPattern((short)pattern, factor);
}

//@}

UtoShape* UtoSphereShape::Clone() const
{
   return new UtoSphereShape(*this);
}

void UtoSphereShape::EnableLighting(bool aState)
{
   GetImpl(this)->EnableLighting(aState);
}
