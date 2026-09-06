// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoVectorShape.cpp : implementation file

#include "UtoVectorShape.hpp"

#include "UtoVectorShapeImp.hpp"

/**
\ingroup shape
\class UtoVectorShape
\brief Draws vector arrows

*/

//{secret}
inline UtoVectorShapeImp* GetImpl(UtoVectorShape* shape)
{
   return static_cast<UtoVectorShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoVectorShapeImp* GetImpl(const UtoVectorShape* shape)
{
   return static_cast<const UtoVectorShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoVectorShape::UtoVectorShape()
   : UtoShape(new UtoVectorShapeImp())
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param aSrc the shape instance to be copied.
*/
UtoVectorShape::UtoVectorShape(const UtoVectorShape& aSrc)
   : UtoShape(new UtoVectorShapeImp(*GetImpl(&aSrc)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoVectorShape::~UtoVectorShape() {}

UtoShape* UtoVectorShape::Clone() const
{
   return new UtoVectorShape(*this);
}

void UtoVectorShape::SetScalar(float aValue)
{
   GetImpl(this)->SetScalar(aValue);
}

float UtoVectorShape::GetScalar() const
{
   return GetImpl(this)->GetScalar();
}

int UtoVectorShape::AddVector(float aAz, float aEl, const osg::Vec4& aColor, bool aHead)
{
   return GetImpl(this)->AddVector(aAz, aEl, aColor, aHead);
}

void UtoVectorShape::RemoveVector(int aId)
{
   GetImpl(this)->RemoveVector(aId);
}

void UtoVectorShape::UpdateVector(int aVectorId, float aAz, float aEl)
{
   GetImpl(this)->UpdateVector(aVectorId, aAz, aEl);
}

void UtoVectorShape::UpdateVectorColor(int aVectorId, const osg::Vec4& aColor)
{
   GetImpl(this)->UpdateVectorColor(aVectorId, aColor);
}

void UtoVectorShape::ShowVector(int aVectorId, bool aState)
{
   GetImpl(this)->ShowVector(aVectorId, aState);
}
