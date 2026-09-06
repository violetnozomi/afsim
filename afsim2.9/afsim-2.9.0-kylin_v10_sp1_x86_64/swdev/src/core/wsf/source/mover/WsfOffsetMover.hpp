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

#ifndef WSFOFFSETMOVER_HPP
#define WSFOFFSETMOVER_HPP

#include "wsf_export.h"

#include "WsfMover.hpp"
#include "WsfSinglePlatformObserver.hpp"

//! A mover that is tethered to another platform.
//!
//! The current implementation is attached to a designated platform at some specified
//! offset from the origin of that platform. The tether is currently 'rigid' in that the
//! following platform EXACTLY mimics the motion of its leader.

class WSF_EXPORT WsfOffsetMover : public WsfMover, public WsfSinglePlatformObserver
{
public:
   //! The type of attachment between the attached platforms
   enum AttachmentType
   {
      cAT_TETHER, //!< The platforms appear to be tethered
      cAT_RIGID   //!< The platforms are rigidly attached to each other
   };

   //! What to do if the reference platform is removed.
   enum OrphanAction
   {
      cOA_STOP,        //!< Stop moving
      cOA_EXTRAPOLATE, //!< Extrapolate from last known position.
      cOA_REMOVE       //!< Remove from simulation.
   };

   WsfOffsetMover() = delete;
   WsfOffsetMover(WsfScenario& aScenario);
   WsfOffsetMover& operator=(const WsfOffsetMover&) = delete;
   ~WsfOffsetMover() override;

   //! @name Common infrastructure methods.
   //@{
   WsfSpatialDomain GetSpatialDomain() const override;
   WsfMover*        Clone() const override;
   bool             Initialize(double aSimTime) override;
   bool             ProcessInput(UtInput& aInput) override;
   void             Update(double aSimTime) override;
   void             SetPlatform(WsfPlatform* aPlatformPtr) override;
   void             PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void             PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   //@}

   //! @name Overrides of WsfSinglePlatformObserver.
   //@{
   void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) override;
   //@}

   virtual void SetReference(double aSimTime, const std::string& aReferenceName);

   virtual void SetOffsetFromReference(double aSimTime, const double aOffsetFromReferenceECS[3]);

protected:
   WsfOffsetMover(const WsfOffsetMover& aSrc);

   void CopyStateFromReference(double aSimTime, WsfPlatform* aReferencePtr);

   void ExtrapolateState(double aSimTime);

   void UpdateTetheredState(double aSimTime);

   //! Pointer to the reference platform
   WsfPlatform* mReferencePtr;

   //! The name (ID) of the reference platform.
   WsfStringId mReferenceNameId;

   //! The means by which the platforms are attached to each other
   AttachmentType mAttachmentType;

   //! The simulation time at which we attached to the reference platform.
   //! This will be less than zero until an attachment is made.
   double mReferenceAttachmentTime;

   //! The ECS offset from the reference platform.
   double mOffsetFromReferenceECS[3];

   //! The magnitude of the offset vector from the reference platform.
   double mOffsetMagnitude;

private:
   //! What to do if the reference platform is deleted.
   OrphanAction mOrphanAction;

   //! The damage factor of the reference platform at the time we detached.
   double mReferenceDamageFactor;

   //! 'true' when the local platforms' Update method is active.
   bool mLocalUpdateActive;

   //! 'true' when the reference platforms' Update method is being explicitly called from our Update method.
   bool mReferenceUpdateActive;

   //! 'true' if the follower is 'locked on' to the reference.
   bool mIsLockedOn;

   double mOldRefTime;
   double mOldRefLocWCS[3];
   double mOldRefVelWCS[3];
};

#endif
