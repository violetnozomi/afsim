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

#ifndef WSFRF_REPEATER_HPP
#define WSFRF_REPEATER_HPP

#include "wsf_mil_export.h"

#include "UtCallbackHolder.hpp"
class UtInput;
class WsfEM_Interaction;
class WsfEM_Xmtr;
#include "WsfSensor.hpp"
#include "WsfTrack.hpp"
class WsfTrackId;
#include "WsfWeapon.hpp"

//! A repeater to implement....
//!
//! @note TODO.
class WSF_MIL_EXPORT WsfRF_Repeater
{
public:
   //! The operating mode of the repeater.
   enum OperatingMode
   {
      cOM_MANUAL, //!< Waits for assignments to be made manually.
      cOM_SEMIAUTO, //!< Manually starting of assignments with automatic stopping based on ESM/RWR . Requires a linked ESM/RWR.
      cOM_AUTO //!< Automatically starts and stops assignments based on ESM/RWR. Requires a linked ESM/RWR.
   };

   //! Construct a RF repeater jammer processor.
   //! @note Jammer routine for automatically starting, stopping and maintaining tracks as desired.
   WsfRF_Repeater();

   //! Copy construct a RF repeater jammer.
   //! @note Do not copy merged tracks and registered tracks.
   WsfRF_Repeater(const WsfRF_Repeater& aSrc);

   WsfRF_Repeater& operator=(const WsfRF_Repeater&) = delete;

   ~WsfRF_Repeater() = default;

   //! Creates and returns a pointer to a clone of this object.
   WsfRF_Repeater* Clone() const;

   //! Initialize the RF repeating jammer.
   //! @returns 'true' if successful or 'false' if an error was encountered.
   bool Initialize(double aSimTime, WsfWeapon* aWeaponPtr);

   //! Process input for a a generic source.
   //! @return 'true' if the input was processed successfully, 'false' if error(s) were found.
   bool ProcessInput(UtInput& aInput);

   //! A convenience method to check for and process a 'repeater/end_repeater' block.
   bool ProcessInputBlock(UtInput& aInput);

   void Reset(double aSimTime);

   OperatingMode GetOperatingMode() const { return mOperatingMode; }
   bool          IsSignalFollowing() const { return mSignalFollowing; }

   void       RegisterTrack(const WsfTrack* aTrackPtr);
   void       RegisterTrackId(WsfTrackId aTrackId, size_t aTargetIndex);
   void       RemoveTrackId(WsfTrackId aTrackId, size_t aTargetIndex);
   WsfTrackId MergedTrackId(WsfTrackId aProcessedTrackId);

   //! Checks and reports results of a detection attempt. Reports 'true' if the transmitter can be
   //! detected or the ESM/RWR sensor is invalid/null or 'false' if there is a failed detection attempt
   //! @param aSimTime           [input] The current simulation time.
   //! @param aXmtrPtr           [input] Pointer to the jammer transmitter.
   //! @param aResult            [output] A Result object that contains detailed information about the result detection
   //! attempt.
   //! @param aTargetInteraction [input] The interaction object representing the interaction to be jammed (radio, radar,
   //! ESM, etc.).
   //!                                   This input is used to get the pointer to the receiver and it's current pointing
   //!                                   information.
   //! @return                   Returns the result of the detection, 'true' if the transmitter can be detected or the
   //! ESM/RWR
   //!                           sensor is invalid/null or 'false' if there is a failed detection attempt
   bool CheckDetectAndUpdate(double                   aSimTime,
                             WsfEM_Xmtr*              aXmtrPtr,
                             WsfSensorResult&         aResult,
                             const WsfEM_Interaction& aTargetInteraction);

   //! Cease jammer operations for a given target
   //! @param aSimTime           [input] The current simulation time.
   //! @param aTrackId           [input] The assigned Track Id of the target.
   //! @param aTargetIndex       [Input] The platform index of the target to be jammed.
   void SensorTrackDropped(double aSimTime, WsfTrackId aTrackId, size_t aTargetIndex);

   //! Start jamming a given target
   //! @param aSimTime           [input] The current simulation time.
   //! @param aTrackPtr          [input] The assigned Track Pointer of the target to be jammed.
   void SensorTrackInitiated(double aSimTime, const WsfTrack* aTrackPtr);

   //! Currently jamming target and update sensor track
   //! @param aSimTime           [input] The current simulation time.
   //! @param aTrackPtr          [input] The assigned Track Pointer of the target to be jammed.
   void SensorTrackUpdated(double aSimTime, const WsfTrack* aTrackPtr);

private:
   // Monitor track signal changes and make jamming changes as required.
   using TrackIdtoSignalMap = std::map<WsfTrackId, std::vector<WsfTrack::Signal>>;

   using TrackIdToTrackIdMap = std::map<WsfTrackId, WsfTrackId>;

   using TargetIndexToSensorNameId = std::map<size_t, WsfStringId>;
   using TargetIndexToTrackIdMap   = std::map<size_t, WsfTrackId>;

   void AddUpdateSignalList(const WsfTrack* aTrackPtr);

   bool MergeTrack(const WsfTrack* aDetectedTrackPtr);

   void UpdateTrack(const WsfTrack* aTrackPtr);

   WsfWeapon* mWeaponPtr;

   OperatingMode mOperatingMode;
   bool          mSignalFollowing;
   bool          mDebug;

   TrackIdtoSignalMap        mTrackIdToSignalMap;
   TargetIndexToSensorNameId mTargetIndexToSensorNameId;

   TrackIdToTrackIdMap mMergedTrackIdMap;        // processed to raw track mappings
   TrackIdToTrackIdMap mReverseMergedTrackIdMap; // raw to processed track mappings
};

#endif
