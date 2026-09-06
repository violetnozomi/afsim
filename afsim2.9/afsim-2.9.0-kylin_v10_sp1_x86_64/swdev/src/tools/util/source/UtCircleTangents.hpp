// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTCIRCLETANGENTS_HPP
#define UTCIRCLETANGENTS_HPP

#include "ut_export.h"

#include "UtVec2.hpp"

//! Computes tangent lines between a point and circle or between two circles
class UT_EXPORT UtCircleTangents
{
public:
   enum TurnDirection
   {
      //! Clockwise
      cCW = 0,
      //! Counter-Clockwise
      cCCW = 1
   };

   static bool CirclePointTangents(const UtVec2d& aCenter,
                                   double         aRadius,
                                   const UtVec2d& aPoint,
                                   UtVec2d&       aTangentPoint1,
                                   UtVec2d&       aTangentPoint2);

   static bool CirclePointTangent(const UtVec2d& aCenter,
                                  double         aRadius,
                                  const UtVec2d& aPoint,
                                  int            aDirection2,
                                  UtVec2d&       aTangentPoint);

   static bool CircleCircleTangent(const UtVec2d& aCenter1,
                                   double         aRadius1,
                                   int            aDirection1,
                                   const UtVec2d& aCenter2,
                                   double         aRadius2,
                                   int            aDirection2,
                                   UtVec2d&       aPoint1,
                                   UtVec2d&       aPoint2);

private:
   static bool InnerTangents(const UtVec2d& aCenter1,
                             double         aRadius1,
                             const UtVec2d& aCenter2,
                             double         aRadius2,
                             UtVec2d&       aTangentA1,
                             UtVec2d&       aTangentA2,
                             UtVec2d&       aTangentB1,
                             UtVec2d&       aTangentB2);

   static bool OuterTangents(const UtVec2d& aCenter1,
                             double         aRadius1,
                             const UtVec2d& aCenter2,
                             double         aRadius2,
                             UtVec2d&       aTangentA1,
                             UtVec2d&       aTangentA2,
                             UtVec2d&       aTangentB1,
                             UtVec2d&       aTangentB2);
};

#endif
