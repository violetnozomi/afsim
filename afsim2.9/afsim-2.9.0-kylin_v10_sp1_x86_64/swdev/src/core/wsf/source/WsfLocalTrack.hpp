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

#ifndef WSFLOCALTRACK_HPP
#define WSFLOCALTRACK_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

#include "UtGenericMappedList.hpp"
class UtScriptClass;
class UtScriptTypes;
class UtInput;
class WsfFilter;
#include "WsfTrack.hpp"
class WsfTrackManager;

//! WsfLocalTrack is a track containing fusion information.
//! @see WsfTrack

class WSF_EXPORT WsfLocalTrack : public WsfTrack
{
public:
   using RawTrackIdList = UtMappedList<WsfTrackId, WsfTrackId>;

   WsfLocalTrack();
   WsfLocalTrack(const WsfLocalTrack& aTrack);
   WsfLocalTrack(const WsfTrack& aTrack);
   ~WsfLocalTrack() override;

   //! Create a copy (clone) of "this" object.
   WsfLocalTrack* Clone() const override;

   const char* GetScriptClassName() const override;

   void ReplacementUpdate(const WsfTrack& aSource) override;

   void UpdateFromMeasurement(double aSimTime, const WsfTrack& aMeasurement) override;

   //! Return the pointer to the track manager that is managing this local track.
   //! @returns The pointer to the track manager, or zero if the local track is
   //! not being managed.
   WsfTrackManager* GetTrackManager() const { return mManagerPtr; }

   //! Set the pointer to the track manager that is managing this local track.
   //! @param aManagerPtr The pointer to the track manager that is managing the local track,
   //! or zero if the track is no longer being considered 'managed'.
   void SetTrackManager(WsfTrackManager* aManagerPtr) { mManagerPtr = aManagerPtr; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   //! @name Correlation methods.
   //@{

   virtual bool Correlate(const WsfTrackId& aFusedTrackId);
   virtual bool Decorrelate(const WsfTrackId& aFusedTrackId);
   virtual bool IsCorrelatedWith(const WsfTrackId& aTrackId) const;

   void DecorrelateAll();

   //! Return whether this track is correlated with any non-local track.
   bool IsCorrelated() const { return (mFusedTrackIds.GetCount() > 0); }

   //! Get the list of track ID's that went into forming this track.
   const RawTrackIdList& GetRawTrackIds() const { return mFusedTrackIds; }

   //@}

   //! @name Use count methods.
   //! The 'use count' is essentially the number of tasks that are currently assigned against the track.
   //! The use count is typically incremented when a task is assigned and decremented when it is
   //! complete. A track manager should not purge (drop) a track when the use count is greater than zero.
   //@{

   void SetPurgeable(bool aIsPurgeable) override;

   //! Increment the usage count.
   void IncrementUseCount() { ++mUseCount; }

   //! Return the current value of the usage count.
   int GetUseCount() const { return mUseCount; }

   //! Decrement the usage count.
   void DecrementUseCount()
   {
      --mUseCount;
      if (mUseCount < 0)
      {
         mUseCount = 0;
      }
   }

   //! Reset the usage count back to zero.
   void ResetUseCount() { mUseCount = 0; }

   //@}

   //! @name Filter methods.
   //@{

   // Return a pointer to the filter.  If no filter is present, the pointer will be zero.
   WsfFilter* GetFilter() override { return mFilterPtr; }

   //! Return a pointer to the filter (const version).  If no filter is present, the pointer will be zero.
   const WsfFilter* GetFilter() const override { return mFilterPtr; }

   void SetFilter(WsfFilter* aFilterPtr);

   virtual bool UpdateFilter(double aSimTime, const WsfTrack& aMeasurement);
   //
   //@}

   //! @name Misc. methods.
   //@{
   //! Return this same object as a WsfTrack.
   WsfTrack* GetRawTrack() { return static_cast<WsfTrack*>(this); }

   //! Return the contributing track id with the highest quality, if set.
   //! If not set, calling the IsNull() method on the returned track id will return true.
   const WsfTrackId& GetHighestTrackQualityId() const { return mHighestQualityTrackId; }

   //! Set the contributing track id with the highest quality.
   void SetHighestTrackQualityId(const WsfTrackId& aTrackId) { mHighestQualityTrackId = aTrackId; }

   bool IsMirrorTrack(const WsfTrack& aTrack) const;
   void SetMirrorTrack(const WsfTrack& aTrack);
   void ClearMirrorTrack();

   void UpdateSignalList();

   WsfLocalTrack& operator=(const WsfLocalTrack& aRhs);
   //@}

   // ================================= WARNING: New member variables =================================
   //
   //    This class is transmitted over the network.  If you add a new member variable, it is
   //    YOUR RESPONSIBILITY to verify that the member is serialized correctly.
   //    Please see the implementation of WsfLocalTrack::SerializeP in xio/WsfXIO_SerializeTypes.cpp
   //
   // =================================================================================================

   //! For XIO (de)serialization.
   template<typename T>
   void SerializeP(T& aBuff);

private:
   WsfLocalTrack& operator=(const WsfTrack& aRhs); // Not Allowed.

   //! The pointer to the track manager that is managing this local track.
   //! This will be zero if the track is not being 'managed'.
   WsfTrackManager* mManagerPtr;

   RawTrackIdList mFusedTrackIds;
   int            mUseCount;
   WsfFilter*     mFilterPtr;
   WsfTrackId     mHighestQualityTrackId;

   //! The track ID of the 'mirror' track (the source track that was INITIALLY responsible for the creation of this
   //! track). It will retain the ID as long as no other tracks contribute to the track. It will be cleared once another
   //! track contributes and will never be reset. See WsfDefaultFusion::UpdateLocalTrackFromNonLocalTrack.
   WsfTrackId mMirrorTrackId;
};
#endif
