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

#ifndef WSFMILEVENTPIPE_HPP
#define WSFMILEVENTPIPE_HPP

class WsfDirectedEnergyWeapon;
class WsfEM_Interaction;
#include "WsfEventPipe.hpp"
class WsfEventPipeInterface;
class WsfQuantumTaskerProcessor;
class WsfSensor;
class WsfTask;
class WsfTrack;
#include "WsfWeapon.hpp"
class WsfWeaponEffects;
class WsfWeaponEngagement;

class WsfMilEventPipeWeaponUpdateEvent;

#define JAM_CHANGE_WORKAROUND_NECESSARY 1

class WsfMilEventPipe : public WsfEventPipeLogger
{
public:
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);
   static void RegisterMessages(UtPackSerializer* aSerializer);
   WsfMilEventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr);

   void GetAdditionalDE_Info(WsfDirectedEnergyWeapon* aWeaponPtr, WsfPlatform* aTargetPtr, std::string& aInfo);

   void PlatformKilled(double aSimTime, WsfPlatform* aPlatformPtr);

   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override;

   void WeaponFinalOutgoingCheck(double aSimTime, WsfWeaponEngagement* aEngagementPtr);

   void WeaponFireAborted(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);

   void WeaponFireRequested(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   //       void WeaponHit(double                     aSimTime,
   //                      const WsfWeaponEngagement* aEngagementPtr,
   //                      WsfPlatform*               aTargetPtr);

   void WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponModeActivated(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr);

   void WeaponModeDeactivated(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr);

   //       void WeaponMissed(double                     aSimTime,
   //                         const WsfWeaponEngagement* aEngagementPtr,
   //                         WsfPlatform*               aTargetPtr);

   void WeaponNonOperational(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponOperational(double aSimTime, WsfWeapon* aWeaponPtr);

   //      void WeaponReloadStarted(double     aSimTime,
   //                               WsfWeapon* aWeaponPtr);
   //
   //      void WeaponReloadEnded(double     aSimTime,
   //                             WsfWeapon* aWeaponPtr);

   void WeaponQuantityChanged(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponSelected(double aSimTime, WsfWeapon* aWeaponPtr, WsfTrack* aTargetTrackPtr = nullptr, int aStoreId = 0);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   void WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr);

   void SendModeActive(double aSimTime, WsfWeapon* aSensorPtr, WsfWeapon::WsfWeaponMode* aModePtr, bool aActive);

   void TaskAssigned(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr);

   void TaskCanceled(double aSimTime, const WsfTask* aTaskPtr);

   void TaskCompleted(double aSimTime, const WsfTask* aTaskPtr, WsfStringId aStatus);

   void QuantumTaskerUpdate(double aSimTime, WsfQuantumTaskerProcessor* aProcessor);


   //       void ImplicitWeaponBeginEngagement(double                     aSimTime,
   //                                          WsfImplicitWeapon*         aWeaponPtr,
   //                                          const WsfWeaponEngagement* aEngagementPtr);
   //
   //       void ImplicitWeaponEndEngagement(double                     aSimTime,
   //                                        WsfImplicitWeapon*         aWeaponPtr,
   //                                        const WsfWeaponEngagement* aEngagementPtr);

   void JammingAttempt(double aSimTime, WsfEM_Xmtr* aXmtrPtr, WsfEM_Rcvr* aRcvrPtr, WsfEM_Interaction& aResult);

   void JammingRequestCanceled(double      aSimTime,
                               WsfWeapon*  aWeaponPtr,
                               double      aFrequency,
                               double      aBandwidth,
                               std::size_t aTargetIndex);

   void JammingRequestInitiated(double      aSimTime,
                                WsfWeapon*  aWeaponPtr,
                                double      aFrequency,
                                double      aBandwidth,
                                WsfStringId aTechniqueId,
                                std::size_t aTargetIndex);

   void JammingRequestUpdated(double      aSimTime,
                              WsfWeapon*  aWeaponPtr,
                              double      aFrequency,
                              double      aBandwidth,
                              WsfStringId aTechniqueId,
                              std::size_t aTargetIndex);
   void UpdateWeaponArticulation(double aSimTime, size_t aPlatformId, WsfStringId& aSensorNameId);

private:
   void SendWeaponStatus(double aSimTime, WsfWeapon* aWeaponPtr);
   void PublishMode(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr);

   void PublishAntennaData(WsfEventPipe::BeamDefinition& bd,
                           WsfEM_Antenna*                aAntennaPtr,
                           WsfArticulatedPart*           aWeaponPtr,
                           WsfAntennaPattern*            aAntennaPatternPtr,
                           double                        aFreq) const;

   void Send(double aSimTime, const WsfPlatform& aPlatform, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipePtr->SendDependent(aSimTime, aPlatform, std::move(aMessagePtr));
   }

   WsfSimulation*         mSimulationPtr;
   UtCallbackHolder       mBaseCallbacks;
   WsfEventPipeInterface* mEventPipePtr;

   UtCallbackHolder mTaskCallback;
   UtCallbackHolder mJammingCallback;

   int cBASE_DATA;
   int cTASK;
   int cQUANTUMTASK;
   int cJAMMING;

   using WeaponModeIdentifier =
      std::pair<unsigned int, std::string>; // identify a sensor mode with sensor unique id and mode name
   std::set<WeaponModeIdentifier>                            mModeSet;
   std::map<unsigned int, WsfMilEventPipeWeaponUpdateEvent*> mWeaponUpdateEvents;
#ifdef JAM_CHANGE_WORKAROUND_NECESSARY
   std::map<std::pair<size_t, std::string>, std::map<size_t, int>> mJamList;
#endif
};
#endif
