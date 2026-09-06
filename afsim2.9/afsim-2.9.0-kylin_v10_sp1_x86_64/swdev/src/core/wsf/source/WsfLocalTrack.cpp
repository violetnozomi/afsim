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

#include "WsfLocalTrack.hpp"

#include "UtMath.hpp"
#include "WsfFilter.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
WsfLocalTrack::WsfLocalTrack()
   : WsfTrack()
   , mManagerPtr(nullptr)
   , mFusedTrackIds()
   , mUseCount(0)
   , mFilterPtr(nullptr)
   , mHighestQualityTrackId()
   , mMirrorTrackId()
{
}

// =================================================================================================
//! Create a local track from a WsfTrack.
WsfLocalTrack::WsfLocalTrack(const WsfTrack& aSrc)
   : WsfTrack(aSrc)
   , mManagerPtr(nullptr)
   , mFusedTrackIds()
   , mUseCount(0)
   , mFilterPtr(nullptr)
   , mHighestQualityTrackId()
   , mMirrorTrackId()
{
}

// =================================================================================================
WsfLocalTrack::WsfLocalTrack(const WsfLocalTrack& aSrc)
   : WsfTrack(aSrc)
   , mManagerPtr(nullptr)
   , mFusedTrackIds(aSrc.mFusedTrackIds)
   , mUseCount(aSrc.mUseCount)
   , mFilterPtr(nullptr)
   , mHighestQualityTrackId(aSrc.mHighestQualityTrackId)
   , mMirrorTrackId(aSrc.mMirrorTrackId)
{
   if (aSrc.mFilterPtr != nullptr)
   {
      mFilterPtr = aSrc.mFilterPtr->Clone();
   }
}

// =================================================================================================
// virtual
WsfLocalTrack::~WsfLocalTrack()
{
   delete mFilterPtr;
}

// =================================================================================================
// virtual
WsfLocalTrack* WsfLocalTrack::Clone() const
{
   return new WsfLocalTrack(*this);
}

// =================================================================================================
// virtual
//! Associate the given non-local track id with this local track.
//! @param aFusedTrackId The non-local track id to be associated with this local track.
//! @returns 'true' if the non-local track was added to the list of non-local tracks that are associated with
//! this local track.
//! @returns 'false' if the non-local track was already contained in the list of non-local tracks associated
//! with this local track.
bool WsfLocalTrack::Correlate(const WsfTrackId& aFusedTrackId)
{
   bool              added      = false;
   const WsfTrackId* trackIdPtr = mFusedTrackIds.Find(aFusedTrackId);
   if (trackIdPtr == nullptr)
   {
      mFusedTrackIds.Add(aFusedTrackId, aFusedTrackId);
      added = true;
   }
   return added;
}

// =================================================================================================
// virtual
//! Dis-associate the given non-local track id with this local track.
//! @param aFusedTrackId The non-local track id with which this track will be dis-associated.
//! @returns 'true' if the given non-local track was successfully dis-associated with the local track.
//! @returns 'false' if the given non-local track not dis-associated because it was not currently in the
//! local tracks list of associated non-local tracks.
bool WsfLocalTrack::Decorrelate(const WsfTrackId& aFusedTrackId)
{
   return mFusedTrackIds.Delete(aFusedTrackId);
}

// =================================================================================================
// virtual
//! Return whether this local track has been correlated with (fused with) this local track.
//! @param aTrackId A non-local track id.
bool WsfLocalTrack::IsCorrelatedWith(const WsfTrackId& aTrackId) const
{
   return (mFusedTrackIds.Find(aTrackId) != nullptr);
}

// =================================================================================================
void WsfLocalTrack::DecorrelateAll()
{
   mFusedTrackIds.DeleteAll();
}

// =================================================================================================
WsfLocalTrack& WsfLocalTrack::operator=(const WsfLocalTrack& aRhs)
{
   if (this != &aRhs)
   {
      WsfTrack::operator=(aRhs);
      mFusedTrackIds    = aRhs.mFusedTrackIds;
      mUseCount         = aRhs.mUseCount;
      if (aRhs.mFilterPtr != nullptr)
      {
         delete mFilterPtr;
         mFilterPtr = aRhs.mFilterPtr->Clone();
      }
   }
   return *this;
}

// =================================================================================================
// virtual
void WsfLocalTrack::ReplacementUpdate(const WsfTrack& aSource)
{
   // Let the base class do its thing...
   WsfTrack::ReplacementUpdate(aSource);

   // Handle derived class attributes
   mHighestQualityTrackId = aSource.GetTrackId();
}

// =================================================================================================
// virtual
void WsfLocalTrack::UpdateFromMeasurement(double aSimTime, const WsfTrack& aMeasurement)
{
   // The base class' UpdateMeasurement method simply copies the data.
   WsfTrack::UpdateFromMeasurement(aSimTime, aMeasurement);
   if (mFilterPtr != nullptr)
   {
      // Update the filter and copy filtered data back into the track
      // (Done in one step for efficiency).
      if (UpdateFilter(aSimTime, aMeasurement))
      {
         if ((mFilterPtr->GetStateCovariance() != nullptr) && mFilterPtr->IsStable())
         {
            // Must set the covariance in the track, as the fusion routines depend on track updates
            // to be WsfTracks, not WsfLocalTracks.
            SetStateCovariance(aSimTime, *mFilterPtr->GetStateCovariance());
            SetStateCovarianceValid(true);

            if (mFilterPtr->GetResidualCovariance() != nullptr)
            {
               SetResidualCovariance(*mFilterPtr->GetResidualCovariance());
            }
         }
      }
   }
}

// =================================================================================================
//! Update the filter, using the current values in a measurement.
//! @note If there is no filter, there will be no update.
bool WsfLocalTrack::UpdateFilter(double aSimTime, const WsfTrack& aMeasurement)
{
   WsfMeasurement measurement = aMeasurement;
   mFilterPtr->SetFrameTime(aSimTime - measurement.GetUpdateTime());

   double newLocationWCS[3];
   bool   doUpdateFilter = aMeasurement.GetExtrapolatedLocationWCS(aSimTime, newLocationWCS);

   if (doUpdateFilter)
   {
      measurement.SetLocationWCS(newLocationWCS);

      // Use the originator location of the sensor that produced the measurement.
      // ... the measurement contains the originator
      WsfMeasurement estimate;
      mFilterPtr->Update(aSimTime, measurement, estimate);
      if (estimate.mLocationWCS.IsValid())
      {
         mLocationWCS = estimate.mLocationWCS;
      }
      if (estimate.mVelocityWCS.IsValid())
      {
         mVelocityWCS = estimate.mVelocityWCS;
      }
   }

   return doUpdateFilter;
}

// =================================================================================================
//! Set the filter.
//! @param aFilterPtr The pointer to the filter or 0 if the use of filtering is being eliminated.
//! Any existing filter is deleted and the track assumes ownership of the new filter.
void WsfLocalTrack::SetFilter(WsfFilter* aFilterPtr)
{
   delete mFilterPtr;
   mFilterPtr = aFilterPtr;
}

// =================================================================================================
//! Set whether this track can be purged.
void WsfLocalTrack::SetPurgeable(bool aIsPurgeable)
{
   if (aIsPurgeable && (!IsPurgeable()))
   {
      // Transition to make this track purgeable when it currently is not.
      assert(mUseCount > 0); // we should have incremented this when we set it non-purgeable before.
      DecrementUseCount();
   }
   else if (IsPurgeable() && (!aIsPurgeable))
   {
      // Transition to make this track non-purgeable when it currently is.
      IncrementUseCount();
   }

   WsfTrack::SetPurgeable(aIsPurgeable);
}

// =================================================================================================
//! Is the supplied track the mirror source track.
bool WsfLocalTrack::IsMirrorTrack(const WsfTrack& aTrack) const
{
   return (aTrack.GetTrackId() == mMirrorTrackId);
}

// =================================================================================================
//! Indicate that the specified track is the mirror source track.
void WsfLocalTrack::SetMirrorTrack(const WsfTrack& aTrack)
{
   mMirrorTrackId = aTrack.GetTrackId();
}

// =================================================================================================
//! Indicate the track is no longer a mirror of a single contributor.
void WsfLocalTrack::ClearMirrorTrack()
{
   mMirrorTrackId.Null();
}

// =================================================================================================
//! Update the signal list in the local track from the signal lists of the contributing raw tracks
void WsfLocalTrack::UpdateSignalList()
{
   unsigned int rtCount = mFusedTrackIds.GetCount();
   if ((rtCount == 0) || (mManagerPtr == nullptr))
   {
      // No contributing tracks. Clear the signal list.
      delete mSignalListPtr.Get();
      mSignalListPtr.Get() = nullptr;
      delete mEmitterTypeIdListPtr.Get();
      mEmitterTypeIdListPtr.Get() = nullptr;
   }
   else
   {
      // One or more contributing tracks.
      // First, clear the signal and emitter lists.
      if (mSignalListPtr.Get() != nullptr)
      {
         mSignalListPtr.Get()->mSignalList.clear();
      }
      if (mEmitterTypeIdListPtr.Get() != nullptr)
      {
         mEmitterTypeIdListPtr.Get()->mEmitterTypeIdList.clear();
      }

      // Build the signal and emitter lists from the associated raw tracks.
      for (unsigned int rtIndex = 0; rtIndex < rtCount; ++rtIndex)
      {
         const WsfTrack* trackPtr = mManagerPtr->FindRawTrack(*mFusedTrackIds.GetEntry(rtIndex));
         if (trackPtr != nullptr)
         {
            MergeSignalLists(*trackPtr);
            MergeEmitterTypeIdLists(*trackPtr);
         }
      }
      if ((mSignalListPtr.Get() != nullptr) && (mSignalListPtr.Get()->mSignalList.empty()))
      {
         delete mSignalListPtr.Get();
         mSignalListPtr.Get() = nullptr;
      }
      if ((mEmitterTypeIdListPtr.Get() != nullptr) && (mEmitterTypeIdListPtr.Get()->mEmitterTypeIdList.empty()))
      {
         delete mEmitterTypeIdListPtr.Get();
         mEmitterTypeIdListPtr.Get() = nullptr;
      }
   }
   mSignalListPtr.SetValid((mSignalListPtr.Get() != nullptr) && (!mSignalListPtr.Get()->mSignalList.empty()));
   mEmitterTypeIdListPtr.SetValid((mEmitterTypeIdListPtr.Get() != nullptr) &&
                                  (!mEmitterTypeIdListPtr.Get()->mEmitterTypeIdList.empty()));
}

const char* WsfLocalTrack::GetScriptClassName() const
{
   return "WsfLocalTrack";
}
