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
// UtoTimerShape.cpp : implementation file

#include "UtoTimerShape.hpp"

#include "UtoTimerShapeImp.hpp"

class UtoTimerShapeImp;

//{secret}
inline UtoTimerShapeImp* GetImpl(UtoTimerShape* shape)
{
   return static_cast<UtoTimerShapeImp*>(shape->Impl());
}

//{secret}
inline const UtoTimerShapeImp* GetImpl(const UtoTimerShape* shape)
{
   return static_cast<const UtoTimerShapeImp*>(shape->Impl());
}

/** @name Constructor & Destructors **/
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoTimerShape::UtoTimerShape()
   : UtoShape(new UtoTimerShapeImp())
{
}

/**
\brief Creates an instance of the object.

Creates an instance of the object.
\param rAzEl the shape instance to be copied.
*/
UtoTimerShape::UtoTimerShape(const UtoTimerShape& rAzEl)
   : UtoShape(new UtoTimerShapeImp(*GetImpl(&rAzEl)))
{
}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoTimerShape::~UtoTimerShape() {}

UtoShape* UtoTimerShape::Clone() const
{
   return new UtoTimerShape(*this);
}

void UtoTimerShape::SetColor(const UtoColor& aColor)
{
   GetImpl(this)->SetColor(aColor, UtoColor::RGBA);
}

void UtoTimerShape::SetTiming(double aStartTime, double aDuration)
{
   GetImpl(this)->SetTiming(aStartTime, aDuration);
}

void UtoTimerShape::UpdateTime(double aCurrentTime)
{
   GetImpl(this)->UpdateTime(aCurrentTime);
}

void UtoTimerShape::SetSize(float aSize)
{
   GetImpl(this)->SetSize(aSize);
}
