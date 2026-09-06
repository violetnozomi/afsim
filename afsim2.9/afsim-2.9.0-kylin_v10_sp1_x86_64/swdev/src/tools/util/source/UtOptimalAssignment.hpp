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

#ifndef UT_OPTIMAL_ASSIGNMENT_HPP
#define UT_OPTIMAL_ASSIGNMENT_HPP

#include "ut_export.h"

#include <cstddef> // for size_t
#include <vector>


///////////////////////////////////////////////////////////////////////////////
// This optimal profit assignment algorithm is based on: the Hungarian Method (Denes Konig and Jeno Egervary)
// Overall algorithm described in notes from a HKUST graduate course:
//     Course:              COMP 572 "Introduction to Combinatorial Optimization" -- Fall 2004
//     Course Notes:        http://www.cse.ust.hk/~golin/COMP572/Notes/Matching.pdf
//     Course Instructor:   Professor Mordecai J.GOLIN - http://www.cse.ust.hk/~golin/
// Comments in the code (steps 1-4) are with reference to the Hungarian Method as outlined in those notes
// Parts of this code derived from example code found at:
// http://community.topcoder.com/tc?module=Static&d1=tutorials&d2=hungarianAlgorithm
//     Specifically: the general structure and the BFS queue cycle in step 4
// This implementation uses the bipartite graph matching method (instead of the matrix method)
// The Hopcroft-Karp algorithm is used internally to achieve an O(N^3) complexity:
// http://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm
///////////////////////////////////////////////////////////////////////////////
class UT_EXPORT UtOptimalAssignment
{
public:
   UtOptimalAssignment();
   ~UtOptimalAssignment() = default;

   // these Assign methods return a vector of indices that maps rows to columns (the assignments)
   //  vector[i] = j  means that row i is assigned to column j
   // input is a rectangular matrix of profits or costs
   // if there are less cols than rows, then some rows may not be assigned to a col
   std::vector<int> AssignMinCost(std::vector<std::vector<double>>& aCosts);
   std::vector<int> AssignMinCost(std::vector<std::vector<int>>& aCosts);
   std::vector<int> AssignMaxProfit(std::vector<std::vector<double>>& aProfits);
   std::vector<int> AssignMaxProfit(std::vector<std::vector<int>>& aProfits); // this is the most efficient method to
                                                                              // call if you can.
   // the other three methods just make conversions and then call into it

private:
   void RecursiveSolve();
   void UpdateLabels();
   void AddToAlternatingTree(int aI, int aP); // index of vertex to add and index of previous vertex

   int                            mSize;
   int                            mMaxMatches; // utility, stores size current matching while algorithm running
   std::vector<std::vector<int>>* mCost; // cost is a misnomer, this is "weight" for max weight assignment (N^3 version
                                         // of Hungarian method)
   std::vector<int> mLabelRow;
   std::vector<int> mLabelCol;
   std::vector<int> mSlack;  // mSlack[i] is in general: min val using (i, all j) of: l(i) + l(j) - cost(i,j)
   std::vector<int> mSlackX; // mSlackX[i] such a vertex, that:   l(mSlackX[i]) + l(i) - cost(mSlackX[i],i) = mSlack[i]

   std::vector<int>  mRowToColAssign;
   std::vector<int>  mColToRowAssign;
   std::vector<bool> mS;    // algorithm set (for labeling?)
   std::vector<bool> mT;    // algorithm set (for labeling?)
   std::vector<int>  mPrev; // stores alternating tree links (from vertex to child in tree, -2 = root)
};

///////////////////////////////////////////////////////////////////////////////
// good test cases:
///////////////////////////////////////////////////////////////////////////////

// UtOptimalAssignment oa;
// std::vector<int> assigns;
// std::vector<std::vector<int> > profits(3,std::vector<int>(3));
// profits[0][0] = 7; profits[0][1] = 4; profits[0][2] = 3;
// profits[1][0] = 3; profits[1][1] = 1; profits[1][2] = 2;
// profits[2][0] = 3; profits[2][1] = 0; profits[2][2] = 0;
// assigns = oa.AssignMaxProfit(profits);  //expect [1,2,0] or [0,2,1]

// profits[0][0] = 3; profits[0][1] = 1; profits[0][2] = 2;
// profits[1][0] = 3; profits[1][1] = 0; profits[1][2] = 0;
// profits[2][0] = 7; profits[2][1] = 4; profits[2][2] = 3;
// assigns = oa.AssignMaxProfit(profits);  //expect [2,0,1] or [2,1,0]

// std::vector<std::vector<int> > profits(4,std::vector<int>(4));
// profits[0][0] = 35; profits[0][1] = 50; profits[0][2] = 50; profits[0][3] = 45;
// profits[1][0] = 90; profits[1][1] = 40; profits[1][2] = 70; profits[1][3] = 60;
// profits[2][0] =  0; profits[2][1] = 30; profits[2][2] = 35; profits[2][3] = 20;
// profits[3][0] = 80; profits[3][1] = 15; profits[3][2] = 30; profits[3][3] = 10;
// assigns = oa.AssignMaxProfit(profits);  //expect [3,2,1,0] or [1,3,2,0]

// std::vector<std::vector<int> > profits(4,std::vector<int>(4));
// profits[0][0] =  0; profits[0][1] =  9; profits[0][2] =  6; profits[0][3] =  7;
// profits[1][0] = 12; profits[1][1] =  2; profits[1][2] =  8; profits[1][3] =  9;
// profits[2][0] =  7; profits[2][1] =  6; profits[2][2] = 11; profits[2][3] =  5;
// profits[3][0] = 12; profits[3][1] = 10; profits[3][2] =  8; profits[3][3] =  4;
// assigns = oa.AssignMaxProfit(profits);  //expect [1,3,2,0]

// std::vector<std::vector<double> > profitsD(4,std::vector<double>(4));
// profitsD[0][0] =  0.0; profitsD[0][1] =  9.0; profitsD[0][2] =  6.0; profitsD[0][3] =  7.0;
// profitsD[1][0] = 12.0; profitsD[1][1] =  2.0; profitsD[1][2] =  8.0; profitsD[1][3] =  9.0;
// profitsD[2][0] =  7.0; profitsD[2][1] =  6.0; profitsD[2][2] = 11.0; profitsD[2][3] =  5.0;
// profitsD[3][0] = 12.0; profitsD[3][1] = 10.0; profitsD[3][2] =  8.0; profitsD[3][3] =  4.0;
// assigns = oa.AssignMaxProfit(profitsD);  //expect [1,3,2,0]

// std::vector<std::vector<double> > costsD(5,std::vector<double>(5));
// double maxd = 1000000.0;
// costsD[0][0] = maxd; costsD[0][1] =  8.0; costsD[0][2] =  6.0; costsD[0][3] = 12.0; costsD[0][4] =  1.0;
// costsD[1][0] = 15.0; costsD[1][1] = 12.0; costsD[1][2] =  7.0; costsD[1][3] = maxd; costsD[1][4] = 10.0;
// costsD[2][0] = 10.0; costsD[2][1] = maxd; costsD[2][2] =  5.0; costsD[2][3] = 14.0; costsD[2][4] = maxd;
// costsD[3][0] = 12.0; costsD[3][1] = maxd; costsD[3][2] = 12.0; costsD[3][3] = 16.0; costsD[3][4] = 15.0;
// costsD[4][0] = 18.0; costsD[4][1] = 17.0; costsD[4][2] = 14.0; costsD[4][3] = maxd; costsD[4][4] = 13.0;
// assigns = oa.AssignMinCost(costsD);  //expect [4,2,3,0,1]

#endif
