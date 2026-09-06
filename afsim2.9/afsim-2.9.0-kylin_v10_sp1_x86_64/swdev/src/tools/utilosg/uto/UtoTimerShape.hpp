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
// UtoTimerShape.hpp : header file

#if !defined(_UTOTIMERSHAPE_HPP_)
#define _UTOTIMERSHAPE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include "UtoShape.hpp"
#include "UtoTypes.hpp"

class UtoTimerShapeImp;

class UTILOSG_EXPORT UtoTimerShape : public UtoShape
{
public:
   UtoTimerShape();
   UtoTimerShape(const UtoTimerShape& aRHS);
   ~UtoTimerShape() override;
   UtoTimerShape& operator=(const UtoTimerShape&);

   UtoShape* Clone() const override;

   void SetColor(const UtoColor& aColor);
   void SetTiming(double aStartTime, double aDuration);
   void SetSize(float aSize);

   void UpdateTime(double aCurrentTime);

   enum TimerStyle
   {
      STYLE_ANALOG1,
      STYLE_HBAR1
   };

protected:
private:
};

#endif // !defined(_UTOTIMERSHAPE_HPP_)
