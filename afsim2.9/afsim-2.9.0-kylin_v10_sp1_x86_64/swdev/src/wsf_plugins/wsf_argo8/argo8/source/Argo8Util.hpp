// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ARGO8UTIL_HPP
#define ARGO8UTIL_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include "UtQuaternion.hpp"
#include "UtVec3.hpp"

//! Utilities for ARGO.
//!
//! This class is a collection of static methods and constants used throughout ARGO.
class Argo8Util
{
   public:

      //static const double cPI;
      //static const double cPI_OVER_2;
      //static const double cTWO_PI;
      //static const double cFOUR_PI;
      //static const double cDEG_PER_RAD;
      //static const double cRAD_PER_DEG;
      //static const double cEARTH_RADIUS;
      //static const double cRF;

      static bool Double2Bool(double aValue) { return (aValue > 0.999); }
      static double Bool2Double(bool aValue) { return (aValue?1.0:0.0); }
};

#endif
