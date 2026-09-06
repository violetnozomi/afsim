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

#ifndef UTINTEGRAND_HPP
#define UTINTEGRAND_HPP

#include "ut_export.h"

#include "UtIntegrate.hpp"

// Interface to define a complex integrand function that may
// need access to more than a dummy variable (i.e., aVal)
// Create a derived class, and override Function
class UT_EXPORT UtIntegrand
{
public:
   UtIntegrand()          = default;
   virtual ~UtIntegrand() = default;

   virtual double Function(double aVal) = 0; // pure virtual

   double Integrate(double aA, double aB, int aM, double aTol)
   {
      return UtIntegrate::AdaptQNC(&UtIntegrand::Function, this, aA, aB, aM, aTol);
   }
};

#endif
