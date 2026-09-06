// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEORBITALPROPAGATORCONDITIONVISITOR_HPP
#define WSFSPACEORBITALPROPAGATORCONDITIONVISITOR_HPP

#include "WsfSpaceOrbitalPropagatorCondition.hpp"

namespace wsf
{
namespace space
{
class OrbitalPropagatorConditionVisitor
{
public:
   virtual void Accept(ApoapsisCondition& aCondition)             = 0;
   virtual void Accept(AscendingNodeCondition& aCondition)        = 0;
   virtual void Accept(AscendingRadiusCondition& aCondition)      = 0;
   virtual void Accept(DescendingNodeCondition& aCondition)       = 0;
   virtual void Accept(DescendingRadiusCondition& aCondition)     = 0;
   virtual void Accept(EclipseEntryCondition& aCondition)         = 0;
   virtual void Accept(EclipseExitCondition& aCondition)          = 0;
   virtual void Accept(NoneCondition& aCondition)                 = 0;
   virtual void Accept(NorthernIntersectionCondition& aCondition) = 0;
   virtual void Accept(PeriapsisCondition& aCondition)            = 0;
   virtual void Accept(RelativeTimeCondition& aCondition)         = 0;
   virtual void Accept(SouthernIntersectionCondition& aCondition) = 0;
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITALPROPAGATORCONDITIONVISITOR_HPP
