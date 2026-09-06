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

#include "UtBinarySearch.hpp"

#include <cfloat>

#include "UtMath.hpp"

namespace
{
const double cSUCCESS_FAILURE_NOT_SET = -DBL_MAX;
}

UtBinarySearch::UtBinarySearch()
   : mIterCount(0)
   , mIterLimit(20)
   , mFindMaximum(true)
   , mConverged(false)
   , mFailed(false)
   , mTolerance(10.0)
   , mIncrRatio(2.0)
   , mBestSuccess(cSUCCESS_FAILURE_NOT_SET)
   , mBestFailure(cSUCCESS_FAILURE_NOT_SET)
   , mCurrent()
   , mPrevious()
{
}

UtBinarySearch::UtBinarySearch(bool aFindMaximum, size_t aIterationMax, double aTrialTolerance, double /*aIncrementRatio*/)
   : mIterCount(0)
   , mIterLimit(aIterationMax)
   , mFindMaximum(aFindMaximum)
   , mConverged(false)
   , mFailed(false)
   , mTolerance(aTrialTolerance)
   , mIncrRatio(2.0)
   , mBestSuccess(cSUCCESS_FAILURE_NOT_SET)
   , mBestFailure(cSUCCESS_FAILURE_NOT_SET)
   , mCurrent()
   , mPrevious()
{
}

void UtBinarySearch::Initialize(bool aFindMaximum, size_t aIterationMax, double aTrialTolerance, double aIncrementRatio)
{
   mIterCount   = 0;
   mIterLimit   = aIterationMax;
   mFindMaximum = aFindMaximum;
   mConverged   = false;
   mFailed      = false;
   mTolerance   = aTrialTolerance;
   mIncrRatio   = aIncrementRatio;
   mBestSuccess = cSUCCESS_FAILURE_NOT_SET;
   mBestFailure = cSUCCESS_FAILURE_NOT_SET;
}

// inline protected
bool UtBinarySearch::HasAHit() const
{
   return mBestSuccess != cSUCCESS_FAILURE_NOT_SET;
}

// inline protected
bool UtBinarySearch::IsBracketed() const
{
   // Bests are initialized to cSUCCESS_FAILURE_NOT_SET, so if not that value, are set.
   return (mBestSuccess != cSUCCESS_FAILURE_NOT_SET) && (mBestFailure != cSUCCESS_FAILURE_NOT_SET);
}

// inline protected
bool UtBinarySearch::Continue() const
{
   return (!mFailed) && (!mConverged);
}

// protected
void UtBinarySearch::SaveBests(const std::pair<double, bool>& aNewValue)
{
   if (mFindMaximum)
   {
      // Trying to find the boundaries for largest value at which result is true.

      // Save off a larger true...
      if (aNewValue.second)
      {
         if ((mBestSuccess == cSUCCESS_FAILURE_NOT_SET) || (aNewValue.first > mBestSuccess))
         {
            mBestSuccess = aNewValue.first;
         }
      }
      // ... or save off a smaller false...
      else // if (! aNewValue.second)
      {
         // While increasing, do not save off a MISS
         // value if no HIT is encountered yet...
         if ((mBestSuccess != cSUCCESS_FAILURE_NOT_SET) &&
             ((mBestFailure == cSUCCESS_FAILURE_NOT_SET) || (aNewValue.first < mBestFailure)))
         {
            mBestFailure = aNewValue.first;
         }
      }
   }
   else // ! mFindMaximum
   {
      // Trying to find the boundaries for smallest value at which result is true.
      // Since the IV is initialized to -1, we must test for less than zero.

      // Save off a smaller true...
      if (aNewValue.second)
      {
         if ((mBestSuccess == cSUCCESS_FAILURE_NOT_SET) || (aNewValue.first < mBestSuccess))
         {
            mBestSuccess = aNewValue.first;
         }
      }

      // ... or save off a larger false...
      else // if (! aNewValue.second)
      {
         // While decreasing, do not save off a MISS
         // value if no HIT is encountered yet.
         if ((mBestSuccess != cSUCCESS_FAILURE_NOT_SET) &&
             ((mBestFailure == cSUCCESS_FAILURE_NOT_SET) || (aNewValue.first > mBestFailure)))
         {
            mBestFailure = aNewValue.first;
         }
      }
   }
}

void UtBinarySearch::Update(double  aTrialValue,
                            bool    aSuccessValue,
                            bool&   aSearchFailed,
                            bool&   aSearchConverged,
                            bool&   aValueIsBracketed,
                            double& aNextTrialValue)
{
   if (Continue())
   {
      mCurrent.first  = aTrialValue;
      mCurrent.second = aSuccessValue;

      SaveBests(mCurrent);

      if (IsBracketed())
      {
         // Have bracketed the desired IV.  Keep bisecting to a convergence.
         if (fabs(mBestSuccess - mBestFailure) < mTolerance)
         {
            aNextTrialValue = mBestSuccess;
            mConverged      = true;
         }
         else
         {
            aNextTrialValue = (mBestSuccess + mBestFailure) / 2.0;
         }
      }
      else if (mFindMaximum)
      {
         // Looking for a maximum IV value.  Keep increasing IV.
         aNextTrialValue = mIncrRatio * mCurrent.first;

         if (HasAHit())
         {
            // Increase the search step size to quickly reach the other
            // edge transition, since when we get the region bracketed, then
            // a binary search afterward becomes an efficient convergence method.
            aNextTrialValue *= mIncrRatio;
         }
      }
      else // if (! mFindMaximum)
      {
         // Looking for a minimum IV value.  Keep decreasing IV.
         aNextTrialValue = mCurrent.first / mIncrRatio;

         if (HasAHit())
         {
            // Increase the search step size to quickly reach the other
            // edge transition, since when we get the region bracketed, then
            // a binary search afterward becomes an efficient convergence method.
            aNextTrialValue /= mIncrRatio;
         }
      }

      mPrevious = mCurrent;

      ++mIterCount;

      // If the iteration count is too high, but we did bracket the desired value,
      // then use the best result, even if we did not fully converge.
      if (mIterCount > mIterLimit)
      {
         mFailed = true;
      }
   }
   else
   {
      aNextTrialValue = mBestSuccess;
   }

   aSearchFailed     = mFailed;
   aSearchConverged  = mConverged;
   aValueIsBracketed = IsBracketed();
}
