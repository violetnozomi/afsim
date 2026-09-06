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
// UtoTimerShapeImp.hpp : header file

#if !defined(_UTOTIMERSHAPEIMP_HPP_)
#define _UTOTIMERSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "UtoTypes.hpp"

// uto includes
#include "UtoShapeImp.hpp"

class UtoTimerShape;

class UtoTimerShapeImp : public UtoShapeImp
{
public:
   UtoTimerShapeImp();
   UtoTimerShapeImp(const UtoTimerShapeImp& aRHS);
   ~UtoTimerShapeImp() override;

   void         SetColor(const UtoColor& aColor, unsigned int aClrMask, unsigned int aReserved = 0) override;
   virtual void SetTiming(double aStartTime, double aDuration);
   virtual void UpdateTime(double aTime);
   virtual void SetSize(float aSize);

private:
   void                     BuildGeometry();
   osg::ref_ptr<osg::Geode> mGeode;

   double mStartTime;
   double mDuration;
   double mCurrentTime;
   float  mSize;
   int    mStyle;
};

#endif // !defined(_UTOTIMERSHAPEIMP_HPP_)
