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

#ifndef UT_BINARY_SEARCH_HPP
#define UT_BINARY_SEARCH_HPP

#include "ut_export.h"

#include <string>
#include <utility> // Needed for std::pair

//!  A binary search utility.  This utility may be used to find the edges of a
//!  solution space, such as a minimum or maximum range at which a given object
//!  is effective.  User provides some evaluation function that accepts a double
//!  and returns a boolean.  For the search to succeed, the first guess supplied
//!  for the independent variable (IV) should be within the success region, or
//!  the search direction should carry subsequent samples into the success region.
//!  After bracketing the pass/fail region while moving in the correct direction,
//!  a binary search will quickly converge upon the cross-over point, to some
//!  desired degree of precision.  Current implementation assumes ONLY positive
//!  IV's are viable, as it halves the intervals toward zero when in a
//!  "decreasing" mode, and no lower bound (bracket value) has been established.

class UT_EXPORT UtBinarySearch
{
public:
   //! Default constructor does not result in a ready-to-use object.
   //! Must be Initialized() first.
   UtBinarySearch();

   //! Four-valued constructor yields a ready-to-use object.
   //! @param aFindMaximum True if desiring to find a maximum, else finds a minimum.
   //! @param aIterationMax Maximum number of iterations, after which considered failed.
   //! @param aTrialTolerance Acceptable error for convergence.
   //! @param aIncrementRatio Ratio to use to increase/decrease the current IV when
   //!     searching for the next edge transition.  Supply a number greater than unity.
   UtBinarySearch(bool aFindMaximum, size_t aIterationMax, double aTrialTolerence, double aIncrementRatio);

   //! Do not need to Initialize() if constructed with three values as above.
   //! Must re-Initialize to restart a new search using the same search object.
   //! @param aFindMaximum True if desiring to find a maximum, else finds a minimum.
   //! @param aIterationMax Maximum number of iterations, after which considered failed.
   //! @param aTrialTolerance Acceptable error for convergence.
   //! @param aIncrementRatio Ratio to use to increase/decrease the current IV when
   //!     searching for the next edge transition.  Supply a number greater than unity.
   void Initialize(bool aFindMaximum, size_t aIterationMax, double aTrialTolerence, double aIncrementRatio);

   //! Refine the search in progress.  User supplies the first pair from Eval function.
   //! @param aTrialValue New Independent Variable (IV) value.  This may be wisely
   //!   selected by the user (with intimate knowledge of the solution space), or may
   //!   be the recommended value given by this same function in a previous call.
   //! @param aSuccessValue New success value (as a function of aTrialValue).
   //! @param aSearchFailed Value indicating the search was unsuccessful.
   //! @param aSearchConverged Value indicating the search has converged.
   //! @param aValueIsBracketed Value indicating the desired value has been bracketed (nearing convergence).
   //! @param aNextTrialValue Suggested IV value for the next Trial.  Note that when
   //!   aSearchConverged is true, aNextTrialValue is set to the best past success.
   void Update(double  aTrialValue,
               bool    aSuccessValue,
               bool&   aSearchFailed,
               bool&   aSearchConverged,
               bool&   aValueIsBracketed,
               double& aNextTrialValue);

   //! If search has converged, will return the past success closest to the crossover point.
   double BestSuccess() const { return mBestSuccess; }

   //! If search has converged, will return the past failure closest to the crossover point.
   double BestFailure() const { return mBestFailure; }

protected:
   void        SaveBests(const std::pair<double, bool>& aNewValue);
   inline bool IsBracketed() const;
   inline bool HasAHit() const;
   inline bool Continue() const;

private:
   size_t                  mIterCount;
   size_t                  mIterLimit;
   bool                    mFindMaximum;
   bool                    mConverged;
   bool                    mFailed;
   double                  mTolerance;
   double                  mIncrRatio;
   double                  mBestSuccess;
   double                  mBestFailure;
   std::pair<double, bool> mCurrent;
   std::pair<double, bool> mPrevious;
};

#endif
