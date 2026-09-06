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

#include "UtOptimalAssignment.hpp"

#include <algorithm>
#include <cfloat>
#include <climits>
#include <limits>

UtOptimalAssignment::UtOptimalAssignment()
   : mSize(0)
   , mMaxMatches(0)
   , mCost(nullptr)
   , mLabelRow()
   , mLabelCol()
   , mSlack()
   , mSlackX()
   , mRowToColAssign()
   , mColToRowAssign()
   , mS()
   , mT()
   , mPrev()
{
}

std::vector<int> UtOptimalAssignment::AssignMinCost(std::vector<std::vector<int>>& aCosts)
{
   if (aCosts.empty() || aCosts[0].empty())
   {
      return std::vector<int>();
   }

   // find maxCost, so we can invert matrix to a value matrix
   int R       = static_cast<int>(aCosts.size());
   int C       = static_cast<int>(aCosts[0].size());
   int maxCost = 0;
   for (int i = 0; i < R; ++i)
   {
      for (int j = 0; j < C; ++j)
      {
         maxCost = std::max(aCosts[i][j], maxCost);
      }
   }
   // invert matrix to hold value (maxCost - cost)
   for (int i = 0; i < R; ++i)
   {
      for (int j = 0; j < C; ++j)
      {
         aCosts[i][j] = (maxCost - aCosts[i][j]);
      }
   }

   // the costs[][] matrix now holds value (profit)
   return AssignMaxProfit(aCosts);
}

std::vector<int> UtOptimalAssignment::AssignMinCost(std::vector<std::vector<double>>& aCosts)
{
   if (aCosts.empty() || aCosts[0].empty())
   {
      return std::vector<int>();
   }

   // find maxCost, so we can invert matrix to a value matrix
   int R = static_cast<int>(aCosts.size());
   int C = static_cast<int>(aCosts[0].size());
   // do not make a square matrix yet, the integer based routine needs the original sizes still
   std::vector<std::vector<int>> intProfits(R, std::vector<int>(C, 0));

   double maxCost = 0.0;
   double minCost = std::numeric_limits<double>::max();
   for (int i = 0; i < R; ++i)
   {
      for (int j = 0; j < C; ++j)
      {
         maxCost = std::max(aCosts[i][j], maxCost);
         minCost = std::min(aCosts[i][j], minCost);
      }
   }
   double invertMaxCost = maxCost - minCost;
   double maxIntVal     = 1000000.0;
   // scale to int (maxInt*cost/maxCost), and invert to hold value (maxCost - cost)
   for (int i = 0; i < R; ++i)
   {
      for (int j = 0; j < C; ++j)
      {
         intProfits[i][j] = static_cast<int>(((maxCost - aCosts[i][j]) / invertMaxCost) * maxIntVal);
      }
   }

   return AssignMaxProfit(intProfits);
}

std::vector<int> UtOptimalAssignment::AssignMaxProfit(std::vector<std::vector<double>>& aProfits)
{
   if (aProfits.empty() || aProfits[0].empty())
   {
      return std::vector<int>();
   }

   // convert double matrix to integer matrix
   int R = static_cast<int>(aProfits.size());
   int C = static_cast<int>(aProfits[0].size());
   // do not make a square matrix yet, the integer based routine needs the original sizes still
   std::vector<std::vector<int>> intProfits(R, std::vector<int>(C, 0));
   // find the max value so we can scale these double values to integers
   double maxVal = 0.0;
   for (int i = 0; i < R; ++i)
   {
      for (int j = 0; j < C; ++j)
      {
         maxVal = std::max(aProfits[i][j], maxVal);
      }
   }
   // fill in the cost matrix with converted values from the profit matrix
   // scale it from the given max value (double) to 1 million (int)?
   double maxIntVal = 1000000.0;
   for (int i = 0; i < R; ++i)
   {
      for (int j = 0; j < C; ++j)
      {
         intProfits[i][j] = static_cast<int>((aProfits[i][j] / maxVal) * maxIntVal);
      }
   }

   return AssignMaxProfit(intProfits);
}

std::vector<int> UtOptimalAssignment::AssignMaxProfit(std::vector<std::vector<int>>& aProfits)
{
   if (aProfits.empty() || aProfits[0].empty())
   {
      return std::vector<int>();
   }

   mCost = &aProfits;

   int R = static_cast<int>(mCost->size());
   int C = static_cast<int>((*mCost)[0].size());
   mSize = std::max(R, C);

   std::vector<int> assignments(R); // you can only assign as many rows as you are given (fake rows are not returned)

   // ensure cost matrix is square
   // this gives a complete weighted bipartite graph (added edges can be weighted at zero)
   if (R < mSize)
   {
      // add rows (with zero profits for all cols)
      mCost->resize(mSize, std::vector<int>(mSize, 0));
   }
   else if (C < mSize)
   {
      // add cols (with zero profits for all rows)
      for (int i = 0; i < mSize; ++i)
      {
         (*mCost)[i].resize(mSize, 0);
      }
   }

   // init
   mSlack.resize(mSize);
   mSlackX.resize(mSize);

   // initial matching: empty (no rows assigned to any cols, which means no cols assigned to rows as well)
   mMaxMatches = 0;
   mRowToColAssign.assign(mSize, -1);
   mColToRowAssign.assign(mSize, -1);

   // find an initial feasible labeling: all col labels to zero, all row labels to the max of the weights for that row
   mLabelRow.resize(mSize);    // init values below
   mLabelCol.assign(mSize, 0); // start at all zero
   for (int x = 0; x < mSize; ++x)
   {
      for (int y = 0; y < mSize; ++y)
      {
         mLabelRow[x] = std::max(mLabelRow[x], (*mCost)[x][y]);
      }
   }

   // starting with this (any) feasible labeling (which also gives us a simple equality graph)
   // recursively perform these two steps until we find a perfect matching:
   //   1. find an augmenting path in our equality graph & flip all edges (this increases size of our matching)
   //   2. if no augmenting path exists, improve the labeling (to grow our equality graph)
   RecursiveSolve();

   for (int i = 0; i < R; ++i)
   {
      assignments[i] = (mRowToColAssign[i] < C) ? (mRowToColAssign[i]) : -1;
   }
   return assignments;
}

void UtOptimalAssignment::RecursiveSolve()
{
   ////////////////////////////////////////////////////////////////////////////
   // step 2, Hungarian method - check for done else initialize mS and mT
   if (mMaxMatches == mSize)
   {
      return; // matching is already perfect, done searching
   }

   int              x    = 0; // counters
   int              y    = 0;
   int              root = 0;        // root vertex
   std::vector<int> bfsQueue(mSize); // queue for BFS (breadth first search)
   int              wr = 0;          // write position in queue
   int              rd = 0;          // read position in queue
   mS.assign(mSize, false);
   mT.assign(mSize, false);
   mPrev.assign(mSize, -1); // to remember for the alternating tree
   // find root of the alternating tree (first unassigned row)
   // this lets us initialize mS and mT (mS = root, mT is empty)
   for (int i = 0; i < mSize; ++i)
   {
      if (mRowToColAssign[i] == -1)
      {
         bfsQueue[wr++] = root = i;
         mPrev[i]              = -2;   // the root of the tree has no previous connecting vertex
         mS[i]                 = true; // this means it's part of the alternating tree (also used to update labeling)
         break;
      }
   }
   // now that we know the root of the alternating tree, initialize the slack values
   for (int i = 0; i < mSize; ++i)
   {
      mSlack[i]  = mLabelRow[root] + mLabelCol[i] - (*mCost)[root][i];
      mSlackX[i] = root;
   }
   // end of step 2
   ////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////
   // steps 3-4, Hungarian method -
   //            4. find augmenting path to increase matching
   //            3. update labels to increase the equality [sub] graph
   while (true) // main cycle
   {
      /////////////////////////////////////////////////////////////////////////
      // step 4b - build alternating tree, looking for augmenting path
      while (rd < wr) // building alternating tree with BFS cycle, looking for an augmenting path
      {
         x = bfsQueue[rd++];         // current vertex from X part
         for (y = 0; y < mSize; ++y) // iterate through all edges in equality graph
         {
            // this is the check to see if the vertex is part of the equality subgraph
            if (((*mCost)[x][y] == (mLabelRow[x] + mLabelCol[y])) && !mT[y])
            {
               if (mColToRowAssign[y] == -1)
               {
                  // an exposed vertex in Y found, so augmenting path exists!
                  break; // from: for (y = 0; y < mSize; ++y)
               }
               mT[y]          = true;               // y not free (exposed) so add it to mT,
               bfsQueue[wr++] = mColToRowAssign[y]; // add vertex mColToRowAssign[y], which is matched with y, to the queue
               AddToAlternatingTree(mColToRowAssign[y], x); // add edges (x,y) and (y,mColToRowAssign[y]) to the tree
            }
         }
         if (y < mSize)
         {
            break;
         } // from: while (rd < wr)    // augmenting path found!
      }
      if (y < mSize)
      {
         break;
      } // from: while (true)    //augmenting path found!
      // end of step 4b
      /////////////////////////////////////////////////////////////////////////

      /////////////////////////////////////////////////////////////////////////
      // step 3 - update labels - this allows more vertices into the equality graph
      UpdateLabels(); // augmenting path not found, so improve labeling
      wr = rd = 0;
      for (y = 0; y < mSize; ++y)
      {
         // add edges to the equality graph because of improving labeling
         // we add edge (mSlackX[y], y) to the tree if and only if !mT[y] && mSlack[y] == 0
         // if y is matched, then add another one: (y, mColToRowAssign[y])
         // else y is exposed (free), so augment the matching
         if (!mT[y] && mSlack[y] == 0)
         {
            if (mColToRowAssign[y] == -1) // exposed vertex in Y found - augmenting path exists!
            {
               x = mSlackX[y];
               break; // from: for (y = 0; y < mSize; ++y)
            }
            else
            {
               mT[y] = true; // y is matched, just add it to mT,
               if (!mS[mColToRowAssign[y]])
               {
                  bfsQueue[wr++] =
                     mColToRowAssign[y]; // add vertex mColToRowAssign[y], which is matched with y, to the queue
                  AddToAlternatingTree(mColToRowAssign[y],
                                       mSlackX[y]); // and add edges (x,y) and (y,mColToRowAssign[y]) to the tree
               }
            }
         }
      }
      if (y < mSize)
      {
         break;
      } // from: while (true)    // augmenting path found!
      // end of step 3
      /////////////////////////////////////////////////////////////////////////
   }

   ////////////////////////////////////////////////////////////////////////////
   // step 4a - use augmenting path to increase matching, then go to step 2 (beginning of method, use recursion)
   // if augmenting path found
   if (y < mSize)
   {
      // add to the matching (step 4a) using the augmenting path & start over
      ++mMaxMatches; // increment matching
      // inverse edges along augmenting path
      for (int cx = x, cy = y, ty; cx != -2; cx = mPrev[cx], cy = ty)
      {
         ty                  = mRowToColAssign[cx];
         mColToRowAssign[cy] = cx;
         mRowToColAssign[cx] = cy;
      }
      RecursiveSolve(); // recall function, go to step 1 of the algorithm
   }
   // end of step 4a
   ////////////////////////////////////////////////////////////////////////////
}

void UtOptimalAssignment::UpdateLabels()
{
   // delta = min of all slack values from set mT
   int delta = INT_MAX;
   for (int i = 0; i < mSize; ++i)
   {
      if (!mT[i])
      {
         delta = std::min(delta, mSlack[i]);
      }
   }

   for (int i = 0; i < mSize; ++i)
   {
      // subtract delta from all i row labels in set S
      if (mS[i])
      {
         mLabelRow[i] -= delta;
      }
      // add delta to all i column labels in set T
      if (mT[i])
      {
         mLabelCol[i] += delta;
      }
      else // if (!mT[i]) // subtract delta from all i slack in set T
      {
         mSlack[i] -= delta;
      }
   }
}

// arguments:  i - vertex [index] to be added
//            p - vertex [index] before 'i' in the alternating path
// internal:   v - vertex mRowToColAssign[i]
// method:     add edge: (p,v)-(v,i) to tree
void UtOptimalAssignment::AddToAlternatingTree(int aI, int aP)
{
   mS[aI]    = true;               // add x to S
   mPrev[aI] = aP;                 // we need this when augmenting
   for (int j = 0; j < mSize; ++j) // update slacks, because we add new vertex to S
   {
      if ((mLabelRow[aI] + mLabelCol[j] - (*mCost)[aI][j]) < mSlack[j])
      {
         mSlack[j]  = mLabelRow[aI] + mLabelCol[j] - (*mCost)[aI][j];
         mSlackX[j] = aI;
      }
   }
}
