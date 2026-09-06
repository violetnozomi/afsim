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

#include "WsfMilEventPipe.hpp"

#include "UtPackReflect.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfCircularFieldOfView.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfFieldOfView.hpp"
#include "WsfMilEventPipeClasses.hpp"
#include "WsfMilEventPipeClassesRegister.hpp"
#include "WsfMilEventPipeSchema.hpp"
#include "WsfPolygonalFieldOfView.hpp"
#include "WsfQuantumTask.hpp"
#include "WsfQuantumTaskerObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfTask.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTrack.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

namespace
{
WsfPlatform* GetAssigner(const WsfSimulation* aSimulation, const WsfTask* aTask)
{
   if (aTask == nullptr || aSimulation == nullptr)
   {
      return nullptr;
   }
   return aSimulation->GetPlatformByIndex(aTask->GetAssignerPlatformIndex());
}
constexpr int cWEAPON_PART_ID{4};
} // namespace

// this is to update the sensor articulation at the sensor's frame-time
class WsfMilEventPipeWeaponUpdateEvent : public WsfEvent
{
public:
   WsfMilEventPipeWeaponUpdateEvent(WsfMilEventPipe* aInterfacePtr, WsfWeapon* aWeaponPtr, double aSimTime, double aUpdateRate)
      : WsfEvent(aSimTime)
      , mUpdateRate(aUpdateRate)
      , mInterfacePtr(aInterfacePtr)
      , mDeleteLater(false)
   {
      mPlatformId   = aWeaponPtr->GetPlatform()->GetIndex();
      mWeaponNameId = aWeaponPtr->GetNameId();
   }

   EventDisposition Execute() override
   {
      if (mDeleteLater)
      {
         return WsfEvent::cDELETE;
      }
      ExecutePrivate();
      SetTime(GetTime() + mUpdateRate);
      return WsfEvent::cRESCHEDULE;
   }

   void DeleteLater() { mDeleteLater = true; }

private:
   void ExecutePrivate() { mInterfacePtr->UpdateWeaponArticulation(GetTime(), mPlatformId, mWeaponNameId); }

   double           mUpdateRate;
   WsfMilEventPipe* mInterfacePtr;
   size_t           mPlatformId;
   WsfStringId      mWeaponNameId;
   bool             mDeleteLater;
};

void WsfMilEventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfMilEventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption("TASK", true);
   aEventPipeExtension.RegisterEventOption("QUANTUMTASK", true);
   aEventPipeExtension.RegisterEventOption("JAMMING", true);

   // Register the callback that gets invoked by WsfEventPipeInterface::AddedToSimulation to register
   // our message types with the serializer.
   aEventPipeExtension.AddCallback(
      aEventPipeExtension.RegisterExtensionMessages.Connect(&WsfMilEventPipe::RegisterMessages));
}

//! A callback from WsfEventPipeInterface::AddedToSimulation to register our message types with the serializer.
void WsfMilEventPipe::RegisterMessages(UtPackSerializer* aSerializerPtr)
{
   WsfEventPipe::UtPack_register_all_wsf_mil_events_types(*aSerializerPtr);
}

WsfMilEventPipe::WsfMilEventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipePtr(aEventPipeInterfacePtr)
{
   cTASK        = aEventPipeInterfacePtr->GetEventId("TASK");
   cQUANTUMTASK = aEventPipeInterfacePtr->GetEventId("QUANTUMTASK");
   cBASE_DATA   = aEventPipeInterfacePtr->GetEventId("BASE_DATA");
   cJAMMING     = aEventPipeInterfacePtr->GetEventId("JAMMING");
}

void WsfMilEventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                          const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   if (aNewCombinedOptions.IsOptionEnabled(cBASE_DATA))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cBASE_DATA))
      {
         // TODO: These events will need implemented
         //           mBaseCallbacks +=
         //           observer.DirectedEnergyWeaponBeginShot(mSimulationPtr).Connect(&WsfEventPipeInterface::DirectedEnergyWeaponBeginShot,
         //           this); mBaseCallbacks +=
         //           observer.DirectedEnergyWeaponUpdateShot(mSimulationPtr).Connect(&WsfEventPipeInterface::DirectedEnergyWeaponUpdateShot,
         //           this); mBaseCallbacks +=
         //           observer.DirectedEnergyWeaponAbortShot(mSimulationPtr).Connect(&WsfEventPipeInterface::DirectedEnergyWeaponAbortShot,
         //           this); mBaseCallbacks +=
         //           observer.DirectedEnergyWeaponCooldownComplete(mSimulationPtr).Connect(&WsfEventPipeInterface::DirectedEnergyWeaponCooldownComplete,
         //           this); mBaseCallbacks +=
         //           observer.DirectedEnergyWeaponEndShot(mSimulationPtr).Connect(&WsfEventPipeInterface::DirectedEnergyWeaponEndShot,
         //           this);
         // mBaseCallbacks +=
         // observer.ImplicitWeaponBeginEngagement(mSimulationPtr).Connect(&WsfEventPipeInterface::ImplicitWeaponBeginEngagement,
         // this); mBaseCallbacks +=
         // observer.ImplicitWeaponEndEngagement(mSimulationPtr).Connect(&WsfEventPipeInterface::ImplicitWeaponEndEngagement,
         // this); mBaseCallbacks +=
         // observer.WeaponFireAborted(mSimulationPtr).Connect(&WsfEventPipeInterface::WeaponFireAborted, this); mBaseCallbacks
         // += observer.WeaponFireRequested(mSimulationPtr).Connect(&WsfEventPipeInterface::WeaponFireRequested, this);

         // TODO: These events probably are not required
         //           mBaseCallbacks += observer.WeaponHit(mSimulationPtr).Connect(&WsfEventPipeInterface::WeaponHit,
         //           this); mBaseCallbacks +=
         //           observer.WeaponMissed(mSimulationPtr).Connect(&WsfEventPipeInterface::WeaponMissed, this);
         // mBaseCallbacks +=
         // observer.WeaponReloadStarted(mSimulationPtr).Connect(&WsfEventPipeInterface::WeaponReloadStarted, this);
         // mBaseCallbacks
         // += observer.WeaponReloadEnded(mSimulationPtr).Connect(&WsfEventPipeInterface::WeaponReloadEnded, this);

         mBaseCallbacks +=
            WsfObserver::WeaponModeActivated(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponModeActivated, this);
         mBaseCallbacks +=
            WsfObserver::WeaponModeDeactivated(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponModeDeactivated, this);
         mBaseCallbacks +=
            WsfObserver::WeaponNonOperational(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponNonOperational, this);
         mBaseCallbacks +=
            WsfObserver::WeaponOperational(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponOperational, this);
         mBaseCallbacks += WsfObserver::WeaponTurnedOff(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponTurnedOff, this);
         mBaseCallbacks += WsfObserver::WeaponTurnedOn(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponTurnedOn, this);
         mBaseCallbacks += WsfObserver::WeaponKilled(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponKilled, this);
         mBaseCallbacks += WsfObserver::WeaponTerminated(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponTerminated, this);
         mBaseCallbacks += WsfObserver::WeaponFired(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponFired, this);
         mBaseCallbacks +=
            WsfObserver::WeaponQuantityChanged(mSimulationPtr).Connect(&WsfMilEventPipe::WeaponQuantityChanged, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cBASE_DATA))
   {
      mBaseCallbacks.Clear();
   }

   if (aNewCombinedOptions.IsOptionEnabled(cTASK))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cTASK))
      {
         mTaskCallback += WsfObserver::TaskAssigned(mSimulationPtr).Connect(&WsfMilEventPipe::TaskAssigned, this);
         mTaskCallback += WsfObserver::TaskCanceled(mSimulationPtr).Connect(&WsfMilEventPipe::TaskCanceled, this);
         mTaskCallback += WsfObserver::TaskCompleted(mSimulationPtr).Connect(&WsfMilEventPipe::TaskCompleted, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cTASK))
   {
      mTaskCallback.Clear();
   }

   if (aNewCombinedOptions.IsOptionEnabled(cQUANTUMTASK))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cQUANTUMTASK))
      {
         mTaskCallback +=
            WsfObserver::QuantumTaskerUpdate(mSimulationPtr).Connect(&WsfMilEventPipe::QuantumTaskerUpdate, this);
      }
   }

   if (aNewCombinedOptions.IsOptionEnabled(cJAMMING))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cJAMMING))
      {
         mJammingCallback +=
            WsfObserver::JammingRequestInitiated(mSimulationPtr).Connect(&WsfMilEventPipe::JammingRequestInitiated, this);
         mJammingCallback +=
            WsfObserver::JammingRequestCanceled(mSimulationPtr).Connect(&WsfMilEventPipe::JammingRequestCanceled, this);
         mJammingCallback +=
            WsfObserver::JammingRequestUpdated(mSimulationPtr).Connect(&WsfMilEventPipe::JammingRequestUpdated, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cJAMMING))
   {
      mJammingCallback.Clear();
   }
}


void WsfMilEventPipe::PlatformKilled(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // In the case of a normal platform removal, we don't want to emit this event (it gets emitted in PlatformDeleted())
   if ((aPlatformPtr != nullptr) && (aPlatformPtr->IsBroken()))
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgPlatformStatus>();
      msgPtr->platformIndex(WsfEventPipeInterface::Platform(*aPlatformPtr));
      msgPtr->broken(true);
      Send(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfMilEventPipe::SendModeActive(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr, bool aActive)
{
   if (aActive)
   {
      WeaponModeIdentifier wmi(aWeaponPtr->GetUniqueId(), aModePtr->GetName());
      if (mModeSet.find(wmi) == mModeSet.end())
      {
         // if this is the first time we have encountered this mode, send a mode definition
         PublishMode(aSimTime, aWeaponPtr, aModePtr);
      }
   }
   if (aWeaponPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgWeaponModeChange>();
      msgPtr->platformIndex(WsfEventPipeInterface::Platform(*aWeaponPtr->GetPlatform()));
      msgPtr->weaponName(aWeaponPtr->GetName());
      msgPtr->modeName(aModePtr->GetName());
      msgPtr->activate(true);
      Send(aSimTime, *aWeaponPtr->GetPlatform(), std::move(msgPtr));
   }
   std::map<unsigned int, WsfMilEventPipeWeaponUpdateEvent*>::iterator it =
      mWeaponUpdateEvents.find(aWeaponPtr->GetUniqueId());
   if (it != mWeaponUpdateEvents.end())
   {
      it->second->DeleteLater();
   }
   if (aActive)
   {
      double frameTime = 1; // do weapons not have a frame time
      auto   eventPtr  = ut::make_unique<WsfMilEventPipeWeaponUpdateEvent>(this, aWeaponPtr, aSimTime, frameTime);
      mWeaponUpdateEvents[aWeaponPtr->GetUniqueId()] = eventPtr.get();
      mSimulationPtr->AddEvent(std::move(eventPtr));
   }
}

void WsfMilEventPipe::PublishMode(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr)
{
   int rc = static_cast<int>(aWeaponPtr->GetEM_XmtrCount());
   if (rc > 0)
   {
      // this (mModeSet.insert) used to be outside of the if statement, based on the assumption that the data we are
      // publishing in the mode is static.  It is not. by putting the line inside we will not publish a mode until we
      // have an active EM Xmtr.  This won't actually catch other variances in the number of spots/beams/xmtrs, and will
      // require a redesign.  This will need to be modified to change the beam definitions as they activate/deactivate,
      // rather than on mode change.
      mModeSet.insert(WeaponModeIdentifier(aWeaponPtr->GetUniqueId(), aModePtr->GetName()));
      if (aWeaponPtr->GetPlatform() != nullptr)
      {
         auto msgPtr = ut::make_unique<WsfEventPipe::MsgEmitterModeDefinition>();

         msgPtr->ownerIndex(WsfEventPipeInterface::Platform(*aWeaponPtr->GetPlatform()));
         msgPtr->partName(aWeaponPtr->GetName());
         msgPtr->partType(cWEAPON_PART_ID);
         msgPtr->modeName(aModePtr->GetName());
         msgPtr->slewLimit().minAzimuth(static_cast<float>(aWeaponPtr->GetMinAzSlew()));
         msgPtr->slewLimit().maxAzimuth(static_cast<float>(aWeaponPtr->GetMaxAzSlew()));
         msgPtr->slewLimit().minElevation(static_cast<float>(aWeaponPtr->GetMinElSlew()));
         msgPtr->slewLimit().maxElevation(static_cast<float>(aWeaponPtr->GetMaxElSlew()));
         // the following max/min stuff is from the script methods
         msgPtr->cueLimit().minAzimuth(static_cast<float>(std::max(aWeaponPtr->GetMinAzCue(), aWeaponPtr->GetMinAzSlew())));
         msgPtr->cueLimit().maxAzimuth(static_cast<float>(std::min(aWeaponPtr->GetMaxAzCue(), aWeaponPtr->GetMaxAzSlew())));
         msgPtr->cueLimit().minElevation(
            static_cast<float>(std::max(aWeaponPtr->GetMinElCue(), aWeaponPtr->GetMinElSlew())));
         msgPtr->cueLimit().maxElevation(
            static_cast<float>(std::min(aWeaponPtr->GetMaxElCue(), aWeaponPtr->GetMaxElSlew())));
         msgPtr->slewMode(aWeaponPtr->GetSlewMode());

         for (int i = 0; i < rc; ++i)
         {
            WsfEventPipe::BeamDefinition bd;

            bd.beamId(i);
            double             freq = aWeaponPtr->GetEM_Xmtr(i).GetFrequency();
            WsfAntennaPattern* patt =
               aWeaponPtr->GetEM_Xmtr(i).GetAntennaPattern(aWeaponPtr->GetEM_Xmtr(0).GetPolarization(), freq);
            WsfEM_Antenna* antennaPtr = aWeaponPtr->GetEM_Xmtr(i).GetAntenna(); // do I need to look at other Xmtrs?
            PublishAntennaData(bd, antennaPtr, aWeaponPtr, patt, freq);
            msgPtr->beamList().push_back(bd);
         }
         Send(aSimTime, *aWeaponPtr->GetPlatform(), std::move(msgPtr));
      }
   }
}

void WsfMilEventPipe::PublishAntennaData(WsfEventPipe::BeamDefinition& aBeamDefinition,
                                         WsfEM_Antenna*                aAntennaPtr,
                                         WsfArticulatedPart*           aPartPtr,
                                         WsfAntennaPattern*            aAntennaPatternPtr,
                                         double                        aFreq) const
{
   aBeamDefinition.minRange((float)aAntennaPtr->GetMinimumRange());
   aBeamDefinition.maxRange((float)aAntennaPtr->GetMaximumRange());
   aBeamDefinition.stabilized(aAntennaPtr->GetScanStabilization());
   aBeamDefinition.antennaTilt(static_cast<float>(aAntennaPtr->GetPitch()));
   aBeamDefinition.scanLimit().minAzimuth(static_cast<float>(aAntennaPtr->GetMinimumAzimuthScanLimit()));
   aBeamDefinition.scanLimit().maxAzimuth(static_cast<float>(aAntennaPtr->GetMaximumAzimuthScanLimit()));
   aBeamDefinition.scanLimit().minElevation(static_cast<float>(aAntennaPtr->GetMinimumElevationScanLimit()));
   aBeamDefinition.scanLimit().maxElevation(static_cast<float>(aAntennaPtr->GetMaximumElevationScanLimit()));
   aBeamDefinition.scanMode(aAntennaPtr->GetScanMode());
   aBeamDefinition.beamTilt(0.0F);
   if (aAntennaPatternPtr != nullptr)
   {
      aBeamDefinition.beamWidthAz(static_cast<float>(aAntennaPatternPtr->GetAzimuthBeamwidth(aFreq, 0.0, 0.0)));
      aBeamDefinition.beamWidthEl(static_cast<float>(aAntennaPatternPtr->GetElevationBeamwidth(aFreq, 0.0, 0.0)));
   }
   if (!aAntennaPtr->DefaultFieldOfView())
   {
      const WsfFieldOfView* fovPtr  = aAntennaPtr->GetFieldOfView();
      WsfStringId           fovtype = fovPtr->GetType();
      if (fovtype == "circular")
      {
         aBeamDefinition.fov().shape(WsfEventPipe::FovShape::circular);
         const WsfCircularFieldOfView* cfovPtr = dynamic_cast<const WsfCircularFieldOfView*>(fovPtr);
         WsfEventPipe::Vec2f           point;
         point.x(static_cast<float>(cfovPtr->GetHalfAngle()));
         point.y(0.0F);
         aBeamDefinition.fov().points().push_back(point);
      }
      else if (fovtype == "polygonal")
      {
         const WsfPolygonalFieldOfView* pFovPtr = dynamic_cast<const WsfPolygonalFieldOfView*>(fovPtr);

         const WsfZoneDefinition& zd =
            pFovPtr->GetPoints(); // I have been promised that this is only ever going to be a polygon,
                                  // so we will ignore everything else that zone-definition can do
         if (zd.GetShapeType() == WsfZoneDefinition::cPOLYGONAL)
         {
            aBeamDefinition.fov().shape(WsfEventPipe::FovShape::polygonal);
            const std::vector<WsfZone::Point>&          points = zd.GetPoints();
            std::vector<WsfZone::Point>::const_iterator it;
            for (it = points.begin(); it != points.end(); ++it)
            {
               WsfEventPipe::Vec2f point;
               point.x(static_cast<float>(it->mX));
               point.y(static_cast<float>(it->mY));
               aBeamDefinition.fov().points().push_back(point);
            }
         }
      }
      else
      {
         aBeamDefinition.fov().shape(WsfEventPipe::FovShape::rectangular);
         double minAzFOV;
         double maxAzFOV;
         aAntennaPtr->GetAzimuthFieldOfView(minAzFOV, maxAzFOV);
         double minElFOV;
         double maxElFOV;
         aAntennaPtr->GetElevationFieldOfView(minElFOV, maxElFOV);

         WsfEventPipe::Vec2f point;
         point.x(static_cast<float>(minAzFOV));
         point.y(static_cast<float>(minElFOV));
         aBeamDefinition.fov().points().push_back(point);
         point.x(static_cast<float>(maxAzFOV));
         point.y(static_cast<float>(maxElFOV));
         aBeamDefinition.fov().points().push_back(point);
      }
   }
}

void WsfMilEventPipe::UpdateWeaponArticulation(double aSimTime, size_t aPlatformId, WsfStringId& aSensorNameId)
{
   WsfPlatform* platform = mSimulationPtr->GetPlatformByIndex(aPlatformId);
   if (platform != nullptr)
   {
      WsfWeapon* weaponPtr = dynamic_cast<WsfWeapon*>(platform->FindComponent(aSensorNameId));
      if (weaponPtr != nullptr)
      {
         if (weaponPtr->IsTurnedOn())
         {
            double az = 0.0;
            double el = 0.0;
            if (weaponPtr->GetEM_XmtrCount() != 0)
            {
               if ((weaponPtr->GetSlewMode() != WsfArticulatedPart::cSLEW_FIXED) && (!weaponPtr->IsCued()) &&
                   (!weaponPtr->TransientCueActive())) // why is there data here if it should only be used conditionally?
               {
                  weaponPtr->GetCurrentCuedOrientation(az, el);
               }
            }
            mEventPipePtr->UpdateArticulation(aSimTime, weaponPtr, cWEAPON_PART_ID, az, el);
         }
      }
   }
}

void WsfMilEventPipe::WeaponModeActivated(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr)
{
   SendModeActive(aSimTime, aWeaponPtr, aModePtr, true);
}

void WsfMilEventPipe::WeaponModeDeactivated(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr)
{
   SendModeActive(aSimTime, aWeaponPtr, aModePtr, false);
}

void WsfMilEventPipe::WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr)
{
#ifdef JAM_CHANGE_WORKAROUND_NECESSARY
   auto it = mJamList.find(std::pair<size_t, std::string>(aWeaponPtr->GetPlatform()->GetIndex(), aWeaponPtr->GetName()));
   if (it != mJamList.end())
   {
      if (aWeaponPtr->GetPlatform() != nullptr)
      {
         for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
         {
            for (int i = 0; i < jt->second; ++i)
            {
               auto msgPtr = ut::make_unique<WsfEventPipe::MsgJammingRequestCanceled>();
               msgPtr->srcPlatform(WsfEventPipeInterface::Platform(*aWeaponPtr->GetPlatform()));
               msgPtr->weaponName(aWeaponPtr->GetName());
               msgPtr->target(WsfEventPipeInterface::Platform(jt->first));

               Send(aSimTime, *aWeaponPtr->GetPlatform(), std::move(msgPtr));
            }
         }
      }
      it->second.clear();
   }
#endif
   SendWeaponStatus(aSimTime, aWeaponPtr);

   auto kt = mWeaponUpdateEvents.find(aWeaponPtr->GetUniqueId());
   if (kt != mWeaponUpdateEvents.end())
   {
      kt->second->DeleteLater();
      mWeaponUpdateEvents.erase(kt);
   }
}

void WsfMilEventPipe::WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr)
{
   SendWeaponStatus(aSimTime, aWeaponPtr);

   auto it = mWeaponUpdateEvents.find(aWeaponPtr->GetUniqueId());
   if (it != mWeaponUpdateEvents.end())
   {
      it->second->DeleteLater();
   }
   double frameTime = 1; // do weapons not have a frame time
   auto   eventPtr  = ut::make_unique<WsfMilEventPipeWeaponUpdateEvent>(this, aWeaponPtr, aSimTime, frameTime);
   mWeaponUpdateEvents[aWeaponPtr->GetUniqueId()] = eventPtr.get();
   mSimulationPtr->AddEvent(std::move(eventPtr));
}

void WsfMilEventPipe::WeaponOperational(double aSimTime, WsfWeapon* aWeaponPtr)
{
   SendWeaponStatus(aSimTime, aWeaponPtr);
}

void WsfMilEventPipe::WeaponNonOperational(double aSimTime, WsfWeapon* aWeaponPtr)
{
   SendWeaponStatus(aSimTime, aWeaponPtr);
}

void WsfMilEventPipe::WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr)
{
   SendWeaponStatus(aSimTime, aWeaponPtr);
}

void WsfMilEventPipe::SendWeaponStatus(double aSimTime, WsfWeapon* aWeaponPtr)
{
   if (aWeaponPtr->IsTurnedOn())
   {
      WsfWeapon::WsfWeaponMode* modePtr = aWeaponPtr->GetCurrentMode();
      if (modePtr != nullptr)
      {
         WeaponModeIdentifier wmi(aWeaponPtr->GetUniqueId(), modePtr->GetName());
         if (mModeSet.find(wmi) == mModeSet.end())
         {
            SendModeActive(aSimTime, aWeaponPtr, modePtr, true);
            // if this is the first time we have encountered this mode, send a mode definition, and mode change
         }
      }
   }

   mEventPipePtr->SendPartStatus(aSimTime, aWeaponPtr, cWEAPON_PART_ID);
}

namespace
{
void PackTaskMessage(WsfEventPipe::MsgTaskUpdate* aMessage, const WsfTask* aTask)
{
   aMessage->taskId(aTask->GetTaskId());
   WsfEventPipeInterface::Pack(aMessage->trackId(), aTask->GetTrackId());
   aMessage->taskType(aTask->GetTaskType());
   aMessage->assignerPlatform(WsfEventPipeInterface::Platform(aTask->GetAssignerPlatformIndex()));
   aMessage->assignerProcessor(aTask->GetAssignerProcessorName());
   aMessage->assigneePlatform(WsfEventPipeInterface::Platform(aTask->GetAssigneePlatformIndex()));
   aMessage->assigneeProcessor(aTask->GetAssigneeProcessorName());
   aMessage->assignTime((float)aTask->GetAssignTime());
   if (!aTask->GetStatus().IsNull())
   {
      aMessage->status(aTask->GetStatus());
   }
   if (!aTask->GetSubStatus().IsNull())
   {
      aMessage->subStatus(aTask->GetSubStatus());
   }
}
} // namespace

void WsfMilEventPipe::TaskAssigned(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr)
{
   WsfPlatform* assigner = GetAssigner(mSimulationPtr, aTaskPtr);
   if (assigner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgTaskUpdate>();
      msgPtr->state(0);
      PackTaskMessage(msgPtr.get(), aTaskPtr);
      if ((aTrackPtr != nullptr) && (aTrackPtr->GetTargetIndex() != 0u))
      {
         msgPtr->targetIndex(WsfEventPipeInterface::Platform(aTrackPtr->GetTargetIndex()));
      }

      Send(aSimTime, *assigner, std::move(msgPtr));
   }
}

void WsfMilEventPipe::TaskCanceled(double aSimTime, const WsfTask* aTaskPtr)
{
   WsfPlatform* assigner = GetAssigner(mSimulationPtr, aTaskPtr);
   if (assigner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgTaskUpdate>();
      msgPtr->state(2);
      PackTaskMessage(msgPtr.get(), aTaskPtr);

      Send(aSimTime, *assigner, std::move(msgPtr));
   }
}

void WsfMilEventPipe::TaskCompleted(double aSimTime, const WsfTask* aTaskPtr, WsfStringId aStatus)
{
   WsfPlatform* assigner = GetAssigner(mSimulationPtr, aTaskPtr);
   if (assigner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgTaskUpdate>();
      msgPtr->state(1);
      PackTaskMessage(msgPtr.get(), aTaskPtr);

      Send(aSimTime, *assigner, std::move(msgPtr));
   }
}

void WsfMilEventPipe::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   if (aEngagementPtr->GetFiringPlatform() != nullptr)
   {
      auto msgPtr     = ut::make_unique<WsfEventPipe::MsgWeaponTerminated>();
      int  geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::in_progress;
      switch (aEngagementPtr->GetGeometryResult())
      {
      case WsfWeaponEngagement::cIN_PROGRESS:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::in_progress;
         break;
      case WsfWeaponEngagement::cDUD:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::dud;
         break;
      case WsfWeaponEngagement::cTARGET_IMPACT:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::target_impact;
         break;
      case WsfWeaponEngagement::cFAR_AWAY_IN_AIR:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::far_away_in_air;
         break;
      case WsfWeaponEngagement::cFAR_AWAY_ABOVE_GROUND:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::far_away_above_ground;
         break;
      case WsfWeaponEngagement::cFAR_AWAY_GROUND_IMPACT:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::far_away_ground_impact;
         break;
      case WsfWeaponEngagement::cTARGET_PROXIMITY_AIR_BURST:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::target_proximity_air_burst;
         break;
      case WsfWeaponEngagement::cTARGET_PROXIMITY_ABOVE_GROUND:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::target_proximity_above_ground;
         break;
      case WsfWeaponEngagement::cTARGET_PROXIMITY_GROUND_IMPACT:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::target_proximity_ground_impact;
         break;
      case WsfWeaponEngagement::cPART_DISABLED_OR_DESTROYED:
         geomResult = WsfEventPipe::MsgWeaponTerminated_GeometryResult::part_disabled_or_destroyed;
         break;
      }
      msgPtr->geometryResult(geomResult);

      if (aEngagementPtr->GetFiringPlatformIndex() > 0)
      {
         msgPtr->firingPlatformIndex(WsfEventPipeInterface::Platform(aEngagementPtr->GetFiringPlatformIndex()));
      }

      if (aEngagementPtr->GetTargetPlatformIndex() > 0)
      {
         msgPtr->targetPlatformIndex(WsfEventPipeInterface::Platform(aEngagementPtr->GetTargetPlatformIndex()));
      }

      if (!aEngagementPtr->GetIntendedTargetName().Empty())
      {
         msgPtr->intendedTargetName(aEngagementPtr->GetIntendedTargetName());
      }

      if (aEngagementPtr->GetWeaponPlatformIndex() != 0u)
      {
         msgPtr->weaponPlatformIndex(WsfEventPipeInterface::Platform(aEngagementPtr->GetWeaponPlatformIndex()));
      }

      if (!aEngagementPtr->GetExtendedResult().empty())
      {
         msgPtr->extendedResult(aEngagementPtr->GetExtendedResult());
      }

      double missDistance = aEngagementPtr->GetMissDistance(nullptr);
      if (missDistance >= 0.0)
      {
         msgPtr->missDistance(static_cast<float>(missDistance));
      }

      WsfWeaponEffects* effects = aEngagementPtr->GetWeaponEffects();
      if (effects != nullptr)
      {
         msgPtr->interceptPk(static_cast<float>(effects->GetInterceptPk()));
         msgPtr->pkDrawn(static_cast<float>(effects->GetPkDrawn()));
      }
      Send(aSimTime, *aEngagementPtr->GetFiringPlatform(), std::move(msgPtr));
   }
}

void WsfMilEventPipe::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   if (aEngagementPtr->GetFiringPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgWeaponFired>();
      if (aEngagementPtr->GetFiringPlatformIndex() != 0u)
      {
         msgPtr->firingPlatformIndex(WsfEventPipeInterface::Platform(aEngagementPtr->GetFiringPlatformIndex()));
      }

      if (aEngagementPtr->GetTargetPlatformIndex() != 0u)
      {
         msgPtr->targetPlatformIndex(WsfEventPipeInterface::Platform(aEngagementPtr->GetTargetPlatformIndex()));
      }

      if (!aEngagementPtr->GetIntendedTargetName().Empty())
      {
         msgPtr->intendedTargetName(aEngagementPtr->GetIntendedTargetName());
      }

      if (aEngagementPtr->GetWeaponPlatformIndex() != 0u)
      {
         msgPtr->weaponPlatformIndex(WsfEventPipeInterface::Platform(aEngagementPtr->GetWeaponPlatformIndex()));
      }

      if (aTargetTrackPtr != nullptr)
      {
         msgPtr->targetTrackIdValid(true);
         WsfEventPipeInterface::Pack(msgPtr->targetTrackId(), aTargetTrackPtr->GetTrackId());
      }
      Send(aSimTime, *aEngagementPtr->GetFiringPlatform(), std::move(msgPtr));
   }
}

void WsfMilEventPipe::WeaponQuantityChanged(double aSimTime, WsfWeapon* aWeaponPtr)
{
   if (aWeaponPtr != nullptr)
   {
      WsfPlatform* platform = aWeaponPtr->GetPlatform();
      if (platform != nullptr)
      {
         unsigned int index  = WsfEventPipeInterface::Platform(*platform);
         auto         msgPtr = ut::make_unique<WsfEventPipe::MsgWeaponQuantityChange>();
         msgPtr->weaponName(aWeaponPtr->GetName());
         msgPtr->platformIndex(index);
         msgPtr->weaponQuantity(aWeaponPtr->GetQuantityRemaining());
         Send(aSimTime, *platform, std::move(msgPtr));
      }
   }
}

void WsfMilEventPipe::QuantumTaskerUpdate(double aSimTime, WsfQuantumTaskerProcessor* aProcessor)
{
   if (aProcessor->GetPlatform() != nullptr)
   {
      // loop through the QuantumTaskerProcessor's matrix and create a message for each one
      std::vector<WsfAssetPerception*> assets    = aProcessor->Assets();
      size_t                           numAssets = aProcessor->Assets().size();
      std::vector<WsfQuantumTask*>     tasks     = aProcessor->Tasks();

      for (size_t taskIdx = 0; taskIdx < tasks.size(); ++taskIdx)
      {
         WsfQuantumTask* curTask          = tasks[taskIdx];
         std::string     taskName         = curTask->GetTaskType().GetString();
         WsfStringId     assigneeResource = curTask->GetResourceName();
         std::string     assignee         = curTask->GetAssigneePlatformName().GetString();

         std::string targetName = curTask->GetTargetName().GetString();
         if (!targetName.empty())
         {
            taskName.append(".");
            taskName.append(targetName);
         }


         for (size_t assetIdx = 0; assetIdx < numAssets; ++assetIdx)
         {
            auto                msgPtr    = ut::make_unique<WsfEventPipe::MsgQuantumTaskerUpdate>();
            WsfAssetPerception* curAsset  = assets[assetIdx];
            std::string         assetName = curAsset->GetName();
            bool                winner    = false;
            if (assignee == assetName)
            {
               // now check that we are looking at the winning system
               for (auto curSystem : curAsset->mSystems)
               {
                  if (curSystem.mSystemName == assigneeResource)
                  {
                     winner = true;
                  }
               }
            }
            else
            {
               // check the assigned tasks for this task....
               std::set<WsfAssetPerception*> assignees = aProcessor->AssetAssigneesForTask(curTask);
               for (auto curAssignee : assignees)
               {
                  if (curAssignee->GetNameId().GetString() == assetName)
                  {
                     // now check that we are looking at the winning system
                     for (auto curSystem : curAssignee->mSystems)
                     {
                        if (curSystem.mSystemName == assigneeResource)
                        {
                           winner = true;
                        }
                     }
                  }
               }
            }

            if (!assigneeResource.Empty())
            {
               assetName.append(".");
               assetName.append(assigneeResource);
            }

            msgPtr->column((int)assetIdx);
            msgPtr->row((int)taskIdx);
            msgPtr->value((float)aProcessor->Value(taskIdx, assetIdx));
            msgPtr->taskName(taskName);
            msgPtr->assetName(assetName);
            msgPtr->resourceName(assigneeResource);
            msgPtr->taskWinner(winner);
            Send(aSimTime, *aProcessor->GetPlatform(), std::move(msgPtr));
         }
      }
   }
}

void WsfMilEventPipe::JammingRequestCanceled(double      aSimTime,
                                             WsfWeapon*  aWeaponPtr,
                                             double      aFrequency,
                                             double      aBandwidth,
                                             std::size_t aTargetIndex)
{
   if (aWeaponPtr->GetPlatform() != nullptr)
   {
#ifdef JAM_CHANGE_WORKAROUND_NECESSARY
      mJamList[std::pair<size_t, std::string>(aWeaponPtr->GetPlatform()->GetIndex(), aWeaponPtr->GetName())][aTargetIndex]--;
#endif
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgJammingRequestCanceled>();
      if (aWeaponPtr->GetPlatform() != nullptr)
      {
         msgPtr->srcPlatform(WsfEventPipeInterface::Platform(*aWeaponPtr->GetPlatform()));
      }
      msgPtr->weaponName(aWeaponPtr->GetName());
      msgPtr->frequency(static_cast<float>(aFrequency));
      msgPtr->bandwidth(static_cast<float>(aBandwidth));
      msgPtr->target(WsfEventPipeInterface::Platform(aTargetIndex));

      Send(aSimTime, *aWeaponPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfMilEventPipe::JammingRequestInitiated(double      aSimTime,
                                              WsfWeapon*  aWeaponPtr,
                                              double      aFrequency,
                                              double      aBandwidth,
                                              WsfStringId aTechniqueId,
                                              std::size_t aTargetIndex)
{
   if (aWeaponPtr->GetPlatform() != nullptr)
   {
#ifdef JAM_CHANGE_WORKAROUND_NECESSARY
      mJamList[std::pair<size_t, std::string>(aWeaponPtr->GetPlatform()->GetIndex(), aWeaponPtr->GetName())][aTargetIndex]++;
#endif
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgJammingRequestInitiated>();
      msgPtr->srcPlatform(WsfEventPipeInterface::Platform(*aWeaponPtr->GetPlatform()));
      msgPtr->weaponName(aWeaponPtr->GetName());
      msgPtr->frequency(static_cast<float>(aFrequency));
      msgPtr->bandwidth(static_cast<float>(aBandwidth));
      msgPtr->technique(aTechniqueId);
      msgPtr->target(WsfEventPipeInterface::Platform(aTargetIndex));

      Send(aSimTime, *aWeaponPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfMilEventPipe::JammingRequestUpdated(double      aSimTime,
                                            WsfWeapon*  aWeaponPtr,
                                            double      aFrequency,
                                            double      aBandwidth,
                                            WsfStringId aTechniqueId,
                                            std::size_t aTargetIndex)
{
   if (aWeaponPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgJammingRequestUpdated>();
      if (aWeaponPtr->GetPlatform() != nullptr)
      {
         msgPtr->srcPlatform(WsfEventPipeInterface::Platform(*aWeaponPtr->GetPlatform()));
      }
      msgPtr->weaponName(aWeaponPtr->GetName());
      msgPtr->frequency(static_cast<float>(aFrequency));
      msgPtr->bandwidth(static_cast<float>(aBandwidth));
      msgPtr->technique(aTechniqueId);
      msgPtr->target(WsfEventPipeInterface::Platform(aTargetIndex));

      Send(aSimTime, *aWeaponPtr->GetPlatform(), std::move(msgPtr));
   }
}
