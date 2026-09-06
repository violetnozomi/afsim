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
// UtoShape.cpp : implementation file

#include "UtoShape.hpp"

#include <algorithm>

#include <osg/io_utils>

#include "UtoShapeImp.hpp"

/**
\ingroup shape
\class UtoShape
\brief Basic shape class.

This is the root class of the shape hierarchy.  This class cannot be directly
instantiated.  This class provides the basic features for all shapes including:
transformations, color, transparency, flags, and cloning.

The shape class provides position, orientation, and scaling transformations.
The position is used as an offset or translation and is set using the
SetPosition method.  The shape position is used as an offset from the entity
location to position a text label for example.  The orientation is used as a
rotation and is set using the SetOrientation method.  The shape orientation can
be used to rotate geometry relative to the entity.  The scaling is used to
change the size of the shape and is set using the SetScaling method.  The shape
scaling can be used to increase or decrease the size of the shape.

The shape class provides the visual properties color and transparency.  The
color property is set using the SetColor method, and the transparency property
is set using the SetTransparency method.

The shape class provides shape flags, which modify some shape behavior.  The
flags can be set using SetShapeFlags, and tested using HasFlags.
Following are
the shape flag summaries, see \ref SetShapeFlags for details.

   <table>
      <tr><td><b>Flag</td>       <td>Description</b></td></tr>
      <tr><td>NO_ROLL</td>       <td>Disable roll rotation.
                                     </td></tr>
      <tr><td>NO_PITCH</td>      <td>Disable pitch rotation.
                                     </td></tr>
      <tr><td>NO_YAW</td>        <td>Disable yaw rotation.
                                     </td></tr>
      <tr><td>NO_PITCHROLL</td>  <td>Disable pitch and roll rotations.
      </td></tr>
      <tr><td>NO_YPR</td>        <td>Disable yaw, pitch, and roll rotations.
      </td></tr>
      <tr><td>AUTO_SCALE</td>    <td>Keeps the size on screen the same.
                                 </td></tr>
      <tr><td>BILLBOARD</td>     <td>A shape will always face the camera.
                                     </td></tr>
      <tr><td>HORIZON_STABILIZED</td> <td>A shape will always face up and point north.
                                      </td></tr>
   </table>

Shapes can be cloned, which is the process of making an exact duplicate of the
shape without needing to know the concrete shape type.  Shapes are cloned every
time you pass a shape to an Uto API.  This design was selected to minimize the
need for dynamic allocation by the user and allow shape prototypes to be
utilized by the user.
*/
class UtoShape;

/** @name Type Definitions */
//@{
/*! \enum UtoShape::Flags \brief Shape actions.

    Defines specific actions for a shape. */
/*! \var UtoShape::Flags UtoShape::NO_ROLL

    Disable roll rotation. */
/*! \var UtoShape::Flags UtoShape::NO_PITCH

    Disable pitch rotation. */
/*! \var UtoShape::Flags UtoShape::NO_YAW

    Disable yaw rotation. */
/*! \var UtoShape::Flags UtoShape::NO_PITCHROLL

    Disable pitch and roll rotations. */
/*! \var UtoShape::Flags UtoShape::NO_YPR

    Disable yaw, pitch, and roll rotations. */
/*! \var UtoShape::Flags UtoShape::AUTO_SCALE

    Keeps the size on screen the same.
    */
/*! \var UtoShape::Flags UtoShape::BILLBOARD

    A shape will always face the camera.
    */
/*! \var UtoShape::Flags UtoShape::HORIZON_STABILIZED

    A shape will always face up and point north.
    */
//@}

UtoShape::UtoShape(UtoShapeImp* shape)
   : m_pImpRoot(shape)
{
   m_pImpRoot->m_PublicPtr = this;
}

/** @name Destructor */
//@{
/**
\brief Destroys the object

This destroys all the subordinate objects for this object.
*/
UtoShape::~UtoShape()
{
   delete m_pImpRoot;
}
//@}


const std::string& UtoShape::GetName() const
{
   return m_pImpRoot->Name();
}

void UtoShape::SetName(const std::string& aName)
{
   m_pImpRoot->SetName(aName);
}

/** @name Accessors */
//@{

/**
\brief Returns the shape color.

Returns the shape color.
\return UtoColor - the current color of the shape.
*/
const UtoColor& UtoShape::Color() const
{
   return m_pImpRoot->Color();
}

/**
\brief Returns the shape scale factor.

Returns the shape scale factor.
\return double - the current scale factor.
*/
double UtoShape::Scaling() const
{
   return m_pImpRoot->Scaling();
}

/**
\brief Returns the shape's static scale factor.

Returns the shape's static scale factor.
\return double - the current static scale factor.
*/
double UtoShape::StaticScale() const
{
   return m_pImpRoot->StaticScale();
}

/**
\brief Returns the shape's total scale, including screen space scaling
*/
double UtoShape::FinalScale() const
{
   return m_pImpRoot->FinalScale();
}

/**
\brief Returns the shape position.

Returns the shape position.
\return UtoPosition - the current position for the shape.
*/
const UtoPosition& UtoShape::Position() const
{
   return m_pImpRoot->Position();
}

/**
\brief Returns the shape orientation.

Returns the shape orientation.
\return UtoOrientation - the current orientation for the shape.
*/
const UtoOrientation& UtoShape::Orientation() const
{
   return m_pImpRoot->Orientation();
}

/**
\brief Returns the culling active flag for the shape.

Returns the shape culling active flag.
\return bool - the shape culling active flag.
*/
bool UtoShape::CullingActive()
{
   return m_pImpRoot->CullingActive();
}

/**
\brief Tests for shape flags.

Determines the flag states for a shape indicated by mask.
\param mask the type of tests to perform on the shape.
\return bool
\return
<table>
   <tr><td>true</td><td>The shape has the given flag(s) set.</td></tr>
   <tr><td>false</td><td>The shape does not have the given flag(s) set.</td></tr>
</table>
*/
bool UtoShape::HasFlag(long mask) const
{
   return m_pImpRoot->HasFlag(mask);
}

/**
\brief Obtains the current state for the shape flags.

Obtains the current state for the shape flags.
\return long - The current state for the shape flags.
*/
long UtoShape::Flags() const
{
   return m_pImpRoot->Flags();
}
//@}

/** @name Operations */
//@{

/**
\brief Sets the shape color.

Sets the shape color.  If the shape is texture mapped this
color will blend with the texture.  Setting this color to
white will maintain the textures original colors.  Some 3D
models may not be effected by the color set here.
\param clr the color to be set on the shape.
\param nClrMask used to set the specified component.
\param nReserved reserved for later use by Uto.
*/
void UtoShape::SetColor(const UtoColor& clr, unsigned int nClrMask, unsigned int nReserved)
{
   // ShapeHelper::CopyOnWrite (this);
   m_pImpRoot->SetColor(clr, nClrMask, nReserved);
}

/**
\brief Sets the scale factor.

Sets the shape scaling.  The scaling is relative to the size
that the shape was originally created.  If the scaling is set
to 2.0 the shape will be twice the original size.

When the scaling isn't 1 the OpenGL attribute GL_RESCALE_NORMAL is enabled
to rescale normals to a unit length vector for the lighting calculations
to display correctly.  Any affect on run time performance is dependent
on the OpenGL implementation.  See the Resource definition files page
for more details.
\param scale scale factor applied to the shape.
*/
void UtoShape::SetScaling(double scale)
{
   // ShapeHelper::CopyOnWrite (this);
   m_pImpRoot->SetScaling(scale);
}

/**
\brief Sets the position of the shape.

The shape position is in relation to the parent.  For instance,
a shape bound to an entity is relative to the entity position.
\param Pos the new position for the shape.
*/
void UtoShape::SetPosition(const UtoPosition& Pos)
{
   // ShapeHelper::CopyOnWrite (this);
   m_pImpRoot->SetPosition(Pos);
}

/**
\brief Sets the orientation for the shape.

The shape orientation is in relation to the parent.  For instance,
a shape bound to an entity is relative to the entity orientation.
\param Orientation the new orientation for the shape.
*/
void UtoShape::SetOrientation(const UtoOrientation& Orientation)
{
   // ShapeHelper::CopyOnWrite (this);
   m_pImpRoot->SetOrientation(Orientation);
}

/**
\brief Sets the culling active flag for the shape.

Sets the culling active flag for the shape.
*/
void UtoShape::SetCullingActive(bool aCullingActive)
{
   m_pImpRoot->SetCullingActive(aCullingActive);
}

/**
\brief Sets and clears shape flags.

Sets the flags for the shape which will enable or disable various shape features.
The mask is used to limit which bits will be affected, while state is used to
set the actual values of those bits.

<table>
   <tr><td><b>Flag</td>       <td>Description</b></td></tr>
      <tr><td>AUTO_SCALE</td>    <td>Enables shape auto-scaling.  When enabled
                                 the shape will maintain its relative size in
                                 the viewer as the camera is zoomed.
                                 Otherwise, the shape will get smaller or
                                 larger as the camera is zoomed.

                                 <p> The shape will take up as many pixels on
                                 screen as the local coordinate (for example
                                 feet) size and scaling would produce.

                                 <p> For the orthographic camera when
                                 billboard is not set the shape offset will be
                                 scaled by the same amount as the shape is
                                 scaled.  In all other cases the shape offset
                                 is fixed.

                                 </td></tr>
   <tr><td>BILLBOARD</td>     <td>Enables shape billboarding.  Billboarding will force the shape to face the
                                  viewer at all times.  This allows a 2-D shape to maximize its visibility in a
                                  3-D scene.</td></tr>
</table>


\param mask a bit mask that will be used to set the shape flags.
\param state a state mask that will be used to set the shape flags.
*/
void UtoShape::SetShapeFlags(long mask, long state)
{
   // ShapeHelper::CopyOnWrite (this);

   // filter the user mask & state with the public mask.  this prevents the user
   // from playing with the 'internal' bits, while letting the implementation
   // set them via the UtoShapeImp interface.
   m_pImpRoot->SetShapeFlags(mask & UtoShapeImp::PUBLIC_MASK, state & UtoShapeImp::PUBLIC_MASK);
}
//@}

// {secret}
void UtoShape::SetImpl(UtoShapeImp* p)
{
   if (m_pImpRoot)
   {
      delete m_pImpRoot;
   }

   m_pImpRoot = p;
}

// {secret}
void UtoShape::Swap(UtoShape& rhs)
{
   std::swap(m_pImpRoot, rhs.m_pImpRoot);
}

// {secret}
void UtoShape::Dump(std::ostream& o)
{
   o << "  Color: " << m_pImpRoot->Color() << std::endl;
   o << "  Scaling: " << m_pImpRoot->Scaling() << std::endl;
   o << "  Position: " << m_pImpRoot->Position() << std::endl;
   o << "  Orientation: " << m_pImpRoot->Orientation() << std::endl;
}

void UtoShape::SetAuxData(const std::string& a_AuxData)
{
   m_pImpRoot->SetAuxData(a_AuxData);
}

const std::string& UtoShape::AuxData() const
{
   return m_pImpRoot->AuxData();
}

void UtoShape::OverrideRenderBin(int aBin, bool aNested)
{
   m_pImpRoot->OverrideRenderBin(aBin, aNested);
}

unsigned int UtoShape::GetViewerId()
{
   return m_pImpRoot->GetViewerId();
}

void UtoShape::SetViewerId(unsigned int aViewerId)
{
   m_pImpRoot->SetViewerId(aViewerId);
}

void UtoShape::SetVespaCoordinateSystem(bool aState)
{
   UtoShapeImp::SetVespaCoordinateSystem(aState);
}

void UtoShape::SetNodeMask(unsigned int aMask, bool aState)
{
   m_pImpRoot->SetNodeMask(aMask, aState);
}

bool UtoShape::AllowUpdates()
{
   return m_pImpRoot->AllowUpdates();
}

void UtoShape::SetAllowUpdates(bool aState)
{
   m_pImpRoot->SetAllowUpdates(aState);
}

void UtoShape::SetUniformf(const char* aName, float aValue)
{
   m_pImpRoot->SetUniformf(aName, aValue);
}

void UtoShape::SetUniform3f(const char* aName, float aValueX, float aValueY, float aValueZ)
{
   m_pImpRoot->SetUniform3f(aName, aValueX, aValueY, aValueZ);
}

void UtoShape::SetUniform3i(const char* aName, int aValueX, int aValueY, int aValueZ)
{
   m_pImpRoot->SetUniform3i(aName, aValueX, aValueY, aValueZ);
}

void UtoShape::SetUniform4f(const char* aName, float aValueX, float aValueY, float aValueZ, float aValueW)
{
   m_pImpRoot->SetUniform4f(aName, aValueX, aValueY, aValueZ, aValueW);
}

void UtoShape::SetUniform4i(const char* aName, int aValueX, int aValueY, int aValueZ, int aValueW)
{
   m_pImpRoot->SetUniform4i(aName, aValueX, aValueY, aValueZ, aValueW);
}
void UtoShape::SetUniformSampler(const char*          aName,
                                 const unsigned char* aSampler1D,
                                 const unsigned int   aComponents,
                                 const unsigned int   aLength)
{
   m_pImpRoot->SetUniformSampler(aName, aSampler1D, aComponents, aLength);
}

void UtoShape::SetUniformSampler(const char*     aName,
                                 const char*     aFilename,
                                 unsigned char*& aData,
                                 unsigned int&   aComponents,
                                 unsigned int&   aSize)
{
   m_pImpRoot->SetUniformSampler(aName, aFilename, aData, aComponents, aSize);
}

void UtoShape::SetProgram(osg::Program* aProgramPtr, bool aEnable)
{
   m_pImpRoot->SetProgram(aProgramPtr, aEnable);
}

osg::Group* UtoShape::GetTree()
{
   return m_pImpRoot->GetTree();
}

const osg::Group* UtoShape::GetTree() const
{
   return m_pImpRoot->GetTree();
}

bool UtoShape::BindToViewer(UtoViewer& aViewer)
{
   return m_pImpRoot->BindToViewer(aViewer);
}
bool UtoShape::BindToViewer(UtoViewer& aViewer, osg::Group* aParentNode)
{
   return m_pImpRoot->BindToViewer(aViewer, aParentNode);
}

void UtoShape::UnbindViewer(UtoViewer& aViewer)
{
   m_pImpRoot->UnbindViewer(aViewer);
}
bool UtoShape::Show(bool aShow, UtoViewer& aViewer)
{
   return m_pImpRoot->Show(aShow, aViewer);
}
bool UtoShape::IsVisible(UtoViewer* aViewer) const
{
   return m_pImpRoot->IsVisible(aViewer);
}

const UtoShape::ReferenceFrame& UtoShape::GetReferenceFrame() const
{
   return m_pImpRoot->GetReferenceFrame();
}

UtoEntity* UtoShape::Owner() const
{
   return m_pImpRoot->Owner();
}

void UtoShape::UnbindViewers()
{
   m_pImpRoot->UnbindViewers();
}

void UtoShape::SetReferenceFrame(const UtoShape::ReferenceFrame& aFrame)
{
   m_pImpRoot->SetReferenceFrame(aFrame);
}

UtoShapeFrame* UtoShape::FindFrame(const UtoViewer& aViewer) const
{
   return Impl()->FindFrame(aViewer);
}
