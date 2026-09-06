// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERTRACKMANAGER_HPP
#define WSFCYBERTRACKMANAGER_HPP

#include "wsf_cyber_export.h"

#include <memory>

#include "WsfTrackManager.hpp"

class UtScript;
class WsfScriptContext;

namespace wsf
{
namespace cyber
{

//! WsfCyberTrackManager is a derived track manager object that takes the place
//! of a typical track manager. When used in conjunction with the cyber track
//! effects, this track manager is substituted for the normally operating track
//! manager. The original track manager is still held by this object for
//! proper initialization and restoration of capabilities upon removal of active
//! cyber effects (for potential multiple effects simultaneously).
class WSF_CYBER_EXPORT TrackManager : public WsfTrackManager
{
public:
   TrackManager(std::unique_ptr<WsfTrackManager>  aTrackManager,
                std::shared_ptr<WsfScriptContext> aContextPtr,
                UtScript*                         aOnEntry,
                UtScript*                         aOnExit,
                UtScript*                         aOnTrack,
                UtScript*                         aOnRawTrack,
                UtScript*                         aOnTrackDrop,
                bool                              aTrackPurging,
                bool                              aRawTrackPurging,
                bool                              aLocalHistoryPurging);

   TrackManager()           = default;
   ~TrackManager() override = default;
   TrackManager& operator=(const TrackManager& aRhs) = delete;
   TrackManager& operator=(TrackManager&& aRhs) = delete;

   TrackManager* Clone() const override;

   virtual void ExecuteOnEntry(double aSimTime);
   virtual void ExecuteOnExit(double aSimTime);

   void PurgeInactiveRawTracks(double aSimTime, double aInactivityTimeLimit, double aStaticInactivityTimeLimit) override;

   void PurgeLocalTrackHistory(double aSimTime, double aKeepTimeInterval) override;

   void PurgeInactiveTracks(double aSimTime, double aInactivityTimeLimit) override;

   //! These are the calls important for modifying the behavior of the track manager. They consist of allowing
   //! the user to inject their logic via defined script method implementations prior to the normal call of the
   //! overloaded method. In the case of a track attempting to be introduced to, or dropped from, the track manager,
   //! the user can indicate via the corresponding script method call return value (boolean) if they want this method
   //! run at all - essentially preventing the track update or preventing the track drop altogether.
   //@{
   WsfLocalTrack* AddTrackReport(double aSimTime, const WsfTrack& aTrackUpdate) override;

   void DropTrack(double aSimTime, const WsfTrackId& aRawTrackId) override;

   WsfTrack* AddRawTrackReport(double aSimTime, const WsfTrack& aRawTrack) override;
   //@}

   //! @name Activate/Deactivate Methods
   //@{
   void Activate() { mActive = true; }
   void Deactivate() { mActive = false; }
   //@}

   WsfTrackManager* GetTrackManager() const { return mTrackManager.get(); }

   void StartFrame() override;
   void EndFrame() override;

protected:
   //! These methods query the user defined logic related to their associated
   //! virtual method implementations. These have to be separated, as the signature
   //! for some of these methods do not allow for feedback of the cyber functionality
   //! via the return type of these functions (some of them being void). In addition,
   //! this also means that the nested track manager must be cast to determine its
   //! interface. This is unfortunate, but necessary. The OnEntry and OnExit calls are
   //! exceptions, since these only occur once, and are executed directly from the effect.
   //@{
   virtual bool CyberAddTrack(double aSimTime, const WsfTrack& aTrackUpdate);
   virtual bool CyberDropTrack(double aSimTime, const WsfTrackId& aRawTrackId);
   virtual bool CyberAddRawTrack(double aSimTime, const WsfTrack& aRawTrack);
   //@}

   TrackManager(const TrackManager& aSrc);
   TrackManager(TrackManager&& aSrc);

   //! A convenience function for handling callbacks. Copies the callbacks
   //! from the decorated track manager instance to this, and then disconnects
   //! the decorated instance callbacks.
   void SwapCallbacks();

private:
   std::unique_ptr<WsfTrackManager>  mTrackManager{nullptr};
   std::shared_ptr<WsfScriptContext> mContextPtr{nullptr};
   UtScript*                         mOnEntry{nullptr};
   UtScript*                         mOnExit{nullptr};
   UtScript*                         mOnTrack{nullptr};
   UtScript*                         mOnRawTrack{nullptr};
   UtScript*                         mOnTrackDrop{nullptr};
   bool                              mActive{true};
   bool                              mInactiveTrackPurging{true};
   bool                              mInactiveRawTrackPurging{true};
   bool                              mLocalTrackHistoryPurging{true};
};

} // namespace cyber
} // namespace wsf

#endif
