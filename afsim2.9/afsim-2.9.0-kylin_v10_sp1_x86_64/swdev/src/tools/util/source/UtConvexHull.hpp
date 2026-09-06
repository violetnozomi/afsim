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

#ifndef UTCONVEXHULL_HPP
#define UTCONVEXHULL_HPP

#include "ut_export.h"

#include <vector>

#include "UtVec2.hpp"

/** This class computes the convex hull from a set of 2D points.

   //The algorithm here uses Andrew's variant of the Graham scan:

   //[1] R.L. Graham, An efficient algorithm for determining the convex hull of a finite planar set,
   //Info. Proc. Lett. 1, 132-133 (1972).
   //http://www.math.ucsd.edu/~sbutler/ron/72_10_convex_hull.pdf

   //[2] A. M. Andrew. Another efficient algorithm for convex hulls in two dimensions.
   //Inform. Process. Lett., 9(5):216-219, 1979.
   //http://www.mpi-inf.mpg.de/~stschirr/exact/cost_of_geometric_computing/algorithms.html
*/
class UT_EXPORT UtConvexHull
{
public:
   static std::vector<UtVec2d> ConvexHull(std::vector<UtVec2d>& aPoints);

protected:
   static void BuildHalfHull(std::vector<UtVec2d>  aInput,
                             std::vector<UtVec2d>& aOutput,
                             UtVec2d               aLeft,
                             UtVec2d               aRight,
                             double                aFactor);

   // Essentially this is a 2D cross product
   static double Direction(UtVec2d aP0, UtVec2d aP1, UtVec2d aP2);
};

#endif
