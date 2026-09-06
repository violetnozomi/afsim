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

#ifndef WSFWEAPONENGAGEMENT_HPP
#define WSFWEAPONENGAGEMENT_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfEvent.hpp"
class WsfPlatform;
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"
class WsfWeapon;
#include "WsfWeaponEffects.hpp"

//! A class to assist in keeping track of an ongoing weapon engagement and process its termination.
//! While every simulated platform has a weapon engagement pointer, the pointer will be null except
//! when engaging a target.  An engagement is created and attached to a platform that either:
//! 1) contains an employed weapon, or 2) is intended to be a lethal weapon platform.  The
//! WsfWeaponEngagement object always instantiates a valid WsfWeaponEffects object.  For an
//! explicit weapon, termination of the engagement (via the Terminate() method) is externally
//! triggered by a WsfWeaponFuse.  For an implicit weapon, such as a directed-energy device,
//! the engagement may continuously damage platform(s) over time, so the owning implementation
//! must call Update() periodically, and then Terminate() when the engagement is considered to be
//! complete.  An engagement is inextricably tied to the concept of an intended target; the
//! WsfPlatform WsfTrackManager will supply the intended target to be considered by the
//! WsfWeaponEffects object.

class WSF_MIL_EXPORT WsfWeaponEngagement : public UtScriptAccessible
{
public:
   enum GeometryResult
   {
      cIN_PROGRESS,                    //!< Engagement not yet complete.
      cDUD,                            //!< No lethal effect due to no detonation (failed to Arm, etc.).
      cTARGET_IMPACT,                  //!< Direct impact upon a target.
      cFAR_AWAY_IN_AIR,                //!< Air burst detonated far from any target.
      cFAR_AWAY_ABOVE_GROUND,          //!< Above ground proximity burst not near any target.
      cFAR_AWAY_GROUND_IMPACT,         //!< Ground impact not near any target.
      cTARGET_PROXIMITY_AIR_BURST,     //!< Detonation was near a target, either in-air, or altitude unknown.
      cTARGET_PROXIMITY_ABOVE_GROUND,  //!< Above ground proximity burst near a target.
      cTARGET_PROXIMITY_GROUND_IMPACT, //!< Impacted ground near a target.
      cPART_DISABLED_OR_DESTROYED      //!< Temporary Lethal Effect on an attached platform part.
   };

   static WsfWeaponEngagement* Find(const WsfPlatform& aPlatform);

   static bool Assign(WsfPlatform& aPlatform, WsfWeaponEngagement*& aEngagementPtr);

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static bool EnumToString(std::string& aString, GeometryResult aGeometryResult);

   static bool StringToEnum(GeometryResult& aGeometryResult, const std::string& aString);

   WsfWeaponEngagement() = delete;

   WsfWeaponEngagement(double            aStartTime,
                       WsfSimulation*    aSimulationPtr,
                       WsfWeapon*        aWeaponSystemPtr,
                       const WsfTrackId& aTargetTrackId,
                       size_t            aIntendedTargetIndex,
                       WsfStringId       aWeaponEffectsType,
                       WsfStringId       aIntendedTargetOffsetName = WsfStringId());

   static void Delete(WsfWeaponEngagement* aEngagementPtr);

   const char* GetScriptClassName() const override { return "WsfWeaponEngagement"; }

   //! @name Engagement control methods.
   virtual bool Initialize(double aSimTime, WsfPlatform* aWeaponPlatformPtr);

   bool Initialize(WsfPlatform* aWeaponPlatformPtr);

   virtual void Update(double aSimTime);

   // NOTE: This is not virtual for a reason. See the implementation file.
   void Terminate(double aSimTime, GeometryResult aGeometryResult = cFAR_AWAY_IN_AIR);

   //! Defeat the engagement (it will have no target effect after this call).
   void Defeat()
   {
      if (mEffectPtr != nullptr)
      {
         mEffectPtr->Defeat();
      }
   }

   //! Set the optional 'extended result'.
   //! The weapon model may call this routine to pass along additional information as to why the weapon terminated.
   void SetExtendedResult(const std::string& aExtendedResult) { mExtendedResult = aExtendedResult; }

   //! Define the externally controlled attribute of the engagement.
   //! This is typically called by some interfaces to external simulations when it receives an engagement from
   //! the external simulation (e.g.: WsfDisFire/WsfDisDetonation).
   //!
   //! @param aIsExternallyControlled 'true' if the engagement is controlled by an external simulation.
   void SetIsExternallyControlled(bool aIsExternallyControlled) { mIsExternallyControlled = aIsExternallyControlled; }

   //! Set the weapon effect model to be used for this engagement.
   void SetWeaponEffects(WsfStringId aWeaponEffectsType);

   void SetInitialWeaponVelocityWCS(const double aInitialVelWCS[3]);

   void SetInitialTargetLocationWCS(const double aInitialLocWCS[3]);

   //@}

   //! @name Retrieve general information about the engagement.
   //@{

   //! Access the WsfWeaponEffects model used during this engagement.
   WsfWeaponEffects* GetWeaponEffects() const { return mEffectPtr; }

   //! Get the serial number assigned to this weapon event.
   //! Each weapon engagement has a unique serial number.
   unsigned int GetSerialNumber() const { return mSerialNumber; }

   //! Return the start time of the engagement.
   double GetStartTime() const { return mStartTime; }

   //! Return the elapsed time for this engagement.
   //! The return value will grow from zero while the engagement is in progress,
   //! and will be fixed after the engagement is complete.
   double GetElapsedTime(double aSimTime) const;

   //! Return the completion time of the engagement.
   //! @returns The simulation time at which the engagement was declared to be complete.
   //! This value will be less than zero if the engagement is not yet complete.
   double GetCompletionTime() const { return mCompletionTime; }

   //! Set the completion time of the engagement.
   void SetCompletionTime(double aCompletionTime) { mCompletionTime = aCompletionTime; }

   //! Return whether this engagement is complete.
   bool IsComplete() const { return mIsComplete; }

   //! Is the engagement externally controlled?
   bool IsExternallyControlled() const { return mIsExternallyControlled; }

   //@}

   //! @name Retrieve information about the participants in the engagement.
   //@{

   WsfPlatform* GetFiringPlatform() const;
   void         SetFiringPlatform(WsfPlatform* aPlatform);

   //! Return the platform index of the firing platform.
   size_t GetFiringPlatformIndex() const { return mFiringPlatformIndex; }

   WsfStringId GetFiringPlatformName() const;

   const WsfWeapon* GetWeaponSystem() const;

   //! Return the name of the firing weapon system (as a string ID).
   WsfStringId GetWeaponSystemName() const { return mWeaponSystemName; }

   //! Return the type of the firing weapon system (as a string ID).
   WsfStringId GetWeaponSystemType() const { return mWeaponSystemType; }

   WsfPlatform* GetWeaponPlatform() const;

   //! Return the platform index of the launched weapon platform.
   //! This will be 0 if this is an implicitly modeled weapon.
   size_t GetWeaponPlatformIndex() const { return mWeaponPlatformIndex; }

   WsfStringId GetWeaponPlatformName() const;

   //! Return the track ID of the target that was initially supplied on the Fire request.
   //! @note This will never change during the engagement, while the value returned by
   //! GetTargetTrackId may change.
   const WsfTrackId& GetInitialTargetTrackId() const { return mInitialTargetTrackId; }

   const WsfTrackId& GetTargetTrackId() const;

   size_t GetTargetPlatformIndex() const;

   WsfPlatform* GetTargetPlatform() const;

   WsfStringId GetIntendedTargetName() const;

   WsfStringId GetIntendedTargetOffsetName() const { return mIntendedTargetOffsetName; }

   //@}

   //! @name Retrieve information about the results of the engagement.
   //@{

   //! Access the GeometryResult for this engagement after it is complete.
   //! The external entity that calls Terminate() will specify the end-game
   //! geometric result for this engagement.
   GeometryResult GetGeometryResult() const { return mGeometryResult; }

   //! Access the WsfWeaponEffects::Result for this engagement after it is complete.
   //! This value of result applies only to the intended target of the engagement.
   WsfWeaponEffects::Result GetTargetResult() const { return mEffectPtr->GetTargetResult(); }

   //! Access the WsfWeaponEffects::Result for this engagement after it is complete.
   //! This value of result applies only to platforms damaged incidentally (not the primary target).
   //! @note For computational expediency, many implementations will explicitly ignore incidental effects.
   WsfWeaponEffects::Result GetIncidentalResult() const { return mEffectPtr->GetIncidentalResult(); }

   //! Get the optional 'extended result' as provided by the weapon model.
   //! The weapon model may provide additional information as to why the weapon terminated.
   std::string GetExtendedResult() const { return mExtendedResult; }

   void GetTargetLocationWCS(WsfPlatform* aTargetPtr, double aTargetLocWCS[3]) const;
   void SetTargetLocationWCS(WsfPlatform* aTargetPtr, const double aTargetLocWCS[3]);
   void GetWeaponLocationWCS(WsfPlatform* aWeaponPtr, double aWeaponLocWCS[3]) const;
   void SetWeaponLocationWCS(WsfPlatform* aWeaponPtr, const double aWeaponLocWCS[3]);

   void GetWeaponVelocityWCS(double aWeaponVelWCS[3]) const;
   void SetWeaponVelocityWCS(const double aWeaponVelWCS[3]);

   void GetTargetLocationAtLaunchWCS(double aTargetLocWCS[3]) const;
   void GetWeaponLocationAtLaunchWCS(double aWeaponLocWCS[3]) const;
   void GetWeaponVelocityAtLaunchWCS(double aWeaponVelWCS[3]) const;

   void GetWeaponLocationECS(double aWeaponLocECS[3]) const;
   void GetWeaponVelocityECS(double aWeaponVelECS[3]) const;

   double GetMissDistance(WsfPlatform* aTargetPtr) const;

   //! Sets the miss distance as provided by the weapon model.
   //! @param aMissDistance Miss distance (meters).
   //! @see GetMissDistance for more information.
   void SetMissDistance(double aMissDistance) { mMissDistance = aMissDistance; }
   //@}

   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   static void CopySerialNumber(const WsfWeaponEngagement* aRealEngPtr, WsfWeaponEngagement* aTempEngPtr);

   //! Set to true is IFC is supported
   void SetIFCEnabled(bool aEnabled) { mIFCEnabled = aEnabled; }
   //@}

   //! Returns 'true' if the platform supports IFC Integrated Fire Control)
   bool GetIFCEnabled() const { return mIFCEnabled; }
   //@}

protected:
   void UpdateIntendedTarget() const;

   WsfWeaponEngagement(const WsfWeaponEngagement& aSrc) = delete;
   WsfWeaponEngagement& operator=(const WsfWeaponEngagement& aRhs) = delete;

   // NOTE: See the code as to why this is protected.
   virtual ~WsfWeaponEngagement();

private:
   class TerminateEvent;
   friend class TerminateEvent;

   class TerminateEvent : public WsfEvent
   {
   public:
      TerminateEvent(double aSimTime, WsfWeaponEngagement* aEngagementPtr);
      EventDisposition Execute() override;

      WsfWeaponEngagement* mEngagementPtr;
   };

   virtual void PrivateTerminate(double aSimTime);

   WsfSimulation*      mSimulationPtr;
   double              mStartTime;
   double              mCompletionTime;
   unsigned int        mSerialNumber;
   size_t              mFiringPlatformIndex;
   size_t              mWeaponPlatformIndex; // Cannot immediately determine mWeaponPlatformIndex, set later
   mutable size_t      mIntendedTargetIndex;
   mutable WsfStringId mIntendedTargetName; // Primarily used for output if the target is destroyed
   WsfStringId         mIntendedTargetOffsetName;
   WsfStringId         mWeaponSystemName;
   WsfStringId         mWeaponSystemType;

   //! The track ID of the target as initially supplied on the associated 'Fire' request.
   WsfTrackId mInitialTargetTrackId;

   //! The track ID of the current target.
   //! Initially this is the same as mInitialTargetTrackId (which is typically a track ID owned
   //! by the firing host). If the weapon is a separate platform then this will become a track ID
   //! owned by the weapon platform once the platform is created.
   mutable WsfTrackId mTargetTrackId;

   WsfWeaponEffects* mEffectPtr;

   //! The result of the engagement.
   GeometryResult mGeometryResult;

   //! An optional 'extended result' for termination as provided by the weapon model.
   std::string mExtendedResult;

   //! The location of the weapon at the time of launch.
   double mWeaponLocAtLaunchWCS[3];
   double mWeaponVelAtLaunchWCS[3];

   //! The velocity of the weapon at termination,
   //! used primarily for implicit engagements.
   double mWeaponVelWCS[3];

   //! The location of the target at the time of launch.
   double mTargetLocAtLaunchWCS[3];

   //! The location of the weapon at the time of engagement termination.
   //! If all fields are zero then the current location of the weapon platform should be used.
   double mComputedWeaponLocWCS[3];
   size_t mComputedWeaponIndex;

   //! The location of the target at the time of engagement termination.
   //! If all fields are zero then the current location of the target platform should be used.
   double mComputedTargetLocWCS[3];
   size_t mComputedTargetIndex;

   //! The miss distance as declared by the weapon model.
   //! This will be < 0.0 if the weapon model did not declare a miss distance.
   double mMissDistance;

   //! 'true' if the engagement originated and is controlled by an external source.
   bool mIsExternallyControlled;

   //! 'true' if initialize has already been called once.
   bool mIsInitialized;

   //! 'true' when the engagement has been completed.
   bool mIsComplete;

   //! 'true' if a TerminateEvent is pending.
   //! If true the object cannot be deleted.
   bool mTerminatePending;

   //! 'true' if Destroy() has been called.
   //! If the object must be deleted when the TerminateEvent executes.
   bool mDestroyPending;

   //! 'true' if the initial velocity is externally set
   bool mInitialWeaponVelocitySet;

   //! 'true' if the initial target location is externally set
   bool mInitialTargetLocationSet;

   //! 'true' if the weapon can be net enabled for integrated fire control
   bool mIFCEnabled;
};

#endif
