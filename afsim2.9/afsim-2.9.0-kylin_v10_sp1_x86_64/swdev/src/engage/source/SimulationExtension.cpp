// ****************************************************************************
// CUI//REL TO USA ONLY
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

#include "SimulationExtension.hpp"

#include <iostream>
#include <string>

#include "Events.hpp"
#include "InputConfig.hpp"
#include "Platform.hpp"
#include "Simulation.hpp"
#include "Task.hpp"
#include "TaskOutput.hpp"
#include "WsfEvent.hpp"
#include "WsfEventOutput.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEngagement.hpp"
#include "dis/WsfDisInterface.hpp"
#include "observer/WsfPlatformObserver.hpp"
#include "observer/WsfSensorObserver.hpp"
#include "observer/WsfSimulationObserver.hpp"
#include "observer/WsfTrackObserver.hpp"
#include "observer/WsfWeaponObserver.hpp"

namespace engage
{

// =================================================================================================
SimulationExtension::SimulationExtension()
   : WsfSimulationExtension()
   , mCallbacks()
   , mTaskPtr(nullptr)
   , mTaskOutputPtr(nullptr)
   , mEventsPtr(nullptr)
   , mWeaponTerminated(false)
   , mSimulationComplete(false)
{
}

SimulationExtension::~SimulationExtension() {}

// =================================================================================================
// virtual
void SimulationExtension::AddedToSimulation()
{
   // The simulation to which we are attached
   Simulation& simulation = dynamic_cast<Simulation&>(GetSimulation());
   mTaskPtr               = &(simulation.GetTask());
   mTaskOutputPtr         = &(mTaskPtr->GetTaskOutput());
   mEventsPtr             = &(mTaskOutputPtr->GetEvents());

   // Attach our observers to the simulation....
   WsfSimulation* simPtr(&GetSimulation());

   mCallbacks.Add(WsfObserver::FrameComplete(simPtr).Connect(&SimulationExtension::FrameComplete, this));

   mCallbacks.Add(WsfObserver::PlatformAdded(simPtr).Connect(&SimulationExtension::PlatformAdded, this));
   mCallbacks.Add(
      WsfObserver::PlatformAppearanceChanged(simPtr).Connect(&SimulationExtension::PlatformAppearanceChanged, this));
   mCallbacks.Add(WsfObserver::PlatformDeleted(simPtr).Connect(&SimulationExtension::PlatformDeleted, this));
   mCallbacks.Add(WsfObserver::PlatformBroken(simPtr).Connect(&SimulationExtension::PlatformKilled, this));

   mCallbacks.Add(WsfObserver::SensorDetectionAttempt(simPtr).Connect(&SimulationExtension::SensorDetectionAttempt, this));
   mCallbacks.Add(WsfObserver::SensorDetectionChanged(simPtr).Connect(&SimulationExtension::SensorDetectionChanged, this));
   mCallbacks.Add(WsfObserver::SensorFrequencyChanged(simPtr).Connect(&SimulationExtension::SensorFrequencyChanged, this));
   mCallbacks.Add(WsfObserver::SensorModeActivated(simPtr).Connect(&SimulationExtension::SensorModeActivated, this));
   mCallbacks.Add(WsfObserver::SensorModeDeactivated(simPtr).Connect(&SimulationExtension::SensorModeDeactivated, this));
   mCallbacks.Add(WsfObserver::SensorNonOperational(simPtr).Connect(&SimulationExtension::SensorNonOperational, this));
   mCallbacks.Add(WsfObserver::SensorOperational(simPtr).Connect(&SimulationExtension::SensorOperational, this));
   mCallbacks.Add(WsfObserver::SensorRequestCanceled(simPtr).Connect(&SimulationExtension::SensorRequestCanceled, this));
   mCallbacks.Add(WsfObserver::SensorRequestInitiated(simPtr).Connect(&SimulationExtension::SensorRequestInitiated, this));
   mCallbacks.Add(WsfObserver::SensorRequestUpdated(simPtr).Connect(&SimulationExtension::SensorRequestUpdated, this));
   mCallbacks.Add(WsfObserver::SensorTrackCoasted(simPtr).Connect(&SimulationExtension::SensorTrackCoasted, this));
   mCallbacks.Add(WsfObserver::SensorTrackDropped(simPtr).Connect(&SimulationExtension::SensorTrackDropped, this));
   mCallbacks.Add(WsfObserver::SensorTrackUpdated(simPtr).Connect(&SimulationExtension::SensorTrackUpdated, this));
   mCallbacks.Add(WsfObserver::SensorTurnedOff(simPtr).Connect(&SimulationExtension::SensorTurnedOff, this));
   mCallbacks.Add(WsfObserver::SensorTurnedOn(simPtr).Connect(&SimulationExtension::SensorTurnedOn, this));
   mCallbacks.Add(WsfObserver::SensorRequestInitiated(simPtr).Connect(&SimulationExtension::SensorRequestInitiated, this));
   mCallbacks.Add(WsfObserver::SensorRequestInitiated(simPtr).Connect(&SimulationExtension::SensorRequestInitiated, this));
   mCallbacks.Add(WsfObserver::SimulationComplete(simPtr).Connect(&SimulationExtension::SimulationComplete, this));

   mCallbacks.Add(WsfObserver::WeaponFireAborted(simPtr).Connect(&SimulationExtension::WeaponFireAborted, this));
   mCallbacks.Add(WsfObserver::WeaponFireRequested(simPtr).Connect(&SimulationExtension::WeaponFireRequested, this));
   mCallbacks.Add(WsfObserver::WeaponFired(simPtr).Connect(&SimulationExtension::WeaponFired, this));
   mCallbacks.Add(WsfObserver::WeaponMissed(simPtr).Connect(&SimulationExtension::WeaponMissed, this));
   mCallbacks.Add(WsfObserver::WeaponHit(simPtr).Connect(&SimulationExtension::WeaponHit, this));
   mCallbacks.Add(WsfObserver::WeaponKilled(simPtr).Connect(&SimulationExtension::WeaponKilled, this));
   mCallbacks.Add(WsfObserver::WeaponTerminated(simPtr).Connect(&SimulationExtension::WeaponTerminated, this));

   mCallbacks.Add(WsfObserver::AdvanceTime(simPtr).Connect(&SimulationExtension::AdvanceTime, this));
   mCallbacks.Add(WsfObserver::FrameStarting(simPtr).Connect(&SimulationExtension::FrameStarting, this));
}

// =================================================================================================
// virtual
bool SimulationExtension::Initialize()
{
   bool ok = true;

   // Enable writing a DIS record file if specified.
   std::string recordFileName(mTaskPtr->GetRunConfig().GetRecordFileName(*mTaskPtr));
   if (!recordFileName.empty())
   {
      WsfDisInterface* disInterfacePtr = WsfDisInterface::Find(GetSimulation());
      if (disInterfacePtr == nullptr)
      {
         std::cout << "***** ERROR: Couldn't find DIS Interface!" << std::endl;
         ok = false;
      }
      else
      {
         WsfDisInterface::DeviceConfiguration device;
         device.mDeviceType = WsfDisDevice::cDEVICE_RECORD;
         device.mAddress    = recordFileName;
         disInterfacePtr->GetDeviceConfigurations().clear();
         disInterfacePtr->GetDeviceConfigurations().push_back(device);
      }
   }

   // Enable writing an event output file if specified.
   // TODO - probably need to check if something is enabled and if not turn everything on
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(GetSimulation());
   if (eventOutputPtr == nullptr)
   {
      std::cout << "***** ERROR: Couldn't find event_output" << std::endl;
      ok = false;
   }
   else
   {
      std::string eventOutputFileName(mTaskPtr->GetRunConfig().GetEventOutputFileName(*mTaskPtr));
      if (!eventOutputFileName.empty())
      {
         eventOutputPtr->OpenFile(eventOutputFileName);
      }
      else if (!eventOutputPtr->GetFileName().empty())
      {
         std::cout << "***** ERROR: Must set the 'event_output_file_base_name' in the run configuration" << std::endl;
         ok = false;
      }
   }

   return ok;
}

// =================================================================================================
//                                       Simulation Observers
// =================================================================================================

void SimulationExtension::AdvanceTime(double aSimTime)
{
   // Advance the platform if required
   Simulation& simulation = dynamic_cast<Simulation&>(GetSimulation());
   simulation.GetTarget()->Update(simulation.GetSimTime());
}

void SimulationExtension::FrameStarting(double aSimTime) {}

// =================================================================================================
void SimulationExtension::FrameComplete(double aSimTime)
{
   // cout << "T=" << aSimTime << " FC" << endl;
   if (!mSimulationComplete)
   {
      mTaskOutputPtr->Write(aSimTime);
      mTaskOutputPtr->WriteEvent(aSimTime);
      mSimulationComplete = mWeaponTerminated;
   }
}

// =================================================================================================
void SimulationExtension::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   std::cout << "T=" << aSimTime << " PFA " << aPlatformPtr->GetName() << std::endl;
   // mEventsPtr->PushEvent(new Events::PlatformAdded(aSimTime, aPlatformPtr));
}

// =================================================================================================
void SimulationExtension::PlatformAppearanceChanged(double                      aSimTime,
                                                    WsfPlatform*                aPlatformPtr,
                                                    WsfObserver::AppearanceType aAppearanceType)
{
   std::cout << "T=" << aSimTime << " PAC " << aPlatformPtr->GetName() << " appearance type " << aAppearanceType
             << std::endl;
   // mEventsPtr->PushEvent(new Events::PlatformAppearanceChanged(aSimTime, aPlatformPtr, aAppearanceType));
}

// =================================================================================================
void SimulationExtension::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   std::cout << "T=" << aSimTime << " PFD " << aPlatformPtr->GetName() << std::endl;
   // mEventsPtr->PushEvent(new Events::PlatformDeleted(aSimTime, aPlatformPtr));
}

// =================================================================================================
void SimulationExtension::PlatformKilled(double aSimTime, WsfPlatform* aPlatformPtr)
{
   std::cout << "T=" << aSimTime << " PFK " << aPlatformPtr->GetName() << std::endl;
   // mEventsPtr->PushEvent(new Events::PlatformKilled(aSimTime, aPlatformPtr));
}

// =================================================================================================
void SimulationExtension::SensorDetectionAttempt(double           aSimTime,
                                                 WsfSensor*       aSensorPtr,
                                                 WsfPlatform*     aTargetPtr,
                                                 WsfSensorResult& aResult)
{
   mEventsPtr->PushEvent(new Events::SensorDetectionAttempt(aSimTime, aSensorPtr, aTargetPtr, aResult));
}

// =================================================================================================
void SimulationExtension::SensorDetectionChanged(double           aSimTime,
                                                 WsfSensor*       aSensorPtr,
                                                 size_t           aTargetIndex,
                                                 WsfSensorResult& aResult)
{
   mEventsPtr->PushEvent(new Events::SensorDetectionChanged(aSimTime, aSensorPtr, aTargetIndex, aResult));
}

// =================================================================================================
void SimulationExtension::SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mEventsPtr->PushEvent(new Events::SensorFrequencyChanged(aSimTime, aSensorPtr, aModePtr));
}

// =================================================================================================
void SimulationExtension::SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mEventsPtr->PushEvent(new Events::SensorModeActivated(aSimTime, aSensorPtr, aModePtr));
}

// =================================================================================================
void SimulationExtension::SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mEventsPtr->PushEvent(new Events::SensorModeDeactivated(aSimTime, aSensorPtr, aModePtr));
}

// =================================================================================================
void SimulationExtension::SensorNonOperational(double aSimTime, WsfSensor* aSensorPtr)
{
   mEventsPtr->PushEvent(new Events::SensorNonOperational(aSimTime, aSensorPtr));
}

// =================================================================================================
void SimulationExtension::SensorOperational(double aSimTime, WsfSensor* aSensorPtr)
{
   mEventsPtr->PushEvent(new Events::SensorOperational(aSimTime, aSensorPtr));
}

// =================================================================================================
void SimulationExtension::SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorRequestCanceled(aSimTime, aSensorPtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorRequestInitiated(double          aSimTime,
                                                 WsfSensor*      aSensorPtr,
                                                 WsfSensorMode*  aModePtr,
                                                 const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorRequestInitiated(aSimTime, aSensorPtr, aModePtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorRequestUpdated(double          aSimTime,
                                               WsfSensor*      aSensorPtr,
                                               WsfSensorMode*  aModePtr,
                                               const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorRequestUpdated(aSimTime, aSensorPtr, aModePtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorTrackCoasted(aSimTime, aSensorPtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorTrackDropped(aSimTime, aSensorPtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorTrackInitiated(aSimTime, aSensorPtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   mEventsPtr->PushEvent(new Events::SensorTrackUpdated(aSimTime, aSensorPtr, aTrackPtr));
}

// =================================================================================================
void SimulationExtension::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
   mEventsPtr->PushEvent(new Events::SensorTurnedOff(aSimTime, aSensorPtr));
}

// =================================================================================================
void SimulationExtension::SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr)
{
   mEventsPtr->PushEvent(new Events::SensorTurnedOn(aSimTime, aSensorPtr));
}

// =================================================================================================
void SimulationExtension::SimulationComplete(double aSimTime)
{
   mEventsPtr->PushEvent(new Events::SimulationComplete(aSimTime));
}

// =================================================================================================
void SimulationExtension::WeaponFireAborted(double          aSimTime,
                                            WsfWeapon*      aWeaponPtr,
                                            const WsfTrack* aTargetTrackPtr,
                                            double          aQuantity)
{
   std::cout << "T=" << aSimTime << " WFA " << aWeaponPtr->GetPlatform()->GetName() << std::endl;
   mEventsPtr->PushEvent(new Events::WeaponFireAborted(aSimTime, aWeaponPtr, aTargetTrackPtr, aQuantity));
}

// =================================================================================================
void SimulationExtension::WeaponFireRequested(double          aSimTime,
                                              WsfWeapon*      aWeaponPtr,
                                              const WsfTrack* aTargetTrackPtr,
                                              double          aQuantity)
{
   std::cout << "T=" << aSimTime << " WFR " << aWeaponPtr->GetPlatform()->GetName() << std::endl;
   mEventsPtr->PushEvent(new Events::WeaponFireRequested(aSimTime, aWeaponPtr, aTargetTrackPtr, aQuantity));
}

// =================================================================================================
void SimulationExtension::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   std::cout << "T=" << aSimTime << " WF " << aEngagementPtr->GetWeaponPlatform()->GetName() << std::endl;

   // Indicate the simulation is in the 'flying' phase.
   Simulation& simulation = dynamic_cast<Simulation&>(GetSimulation());
   simulation.SetPhase(cFLYING, aSimTime);

   // Create the our interface platform that is called by the output module to extract variables.
   WsfPlatform* wpnPlatformPtr(aEngagementPtr->GetWeaponPlatform());
   if (wpnPlatformPtr != nullptr)
   {
      simulation.SetWeapon(new Platform(simulation.GetTask().GetRunConfig(), wpnPlatformPtr));
   }
   mEventsPtr->PushEvent(new Events::WeaponFired(aSimTime, aEngagementPtr, aTargetTrackPtr));
}

// =================================================================================================
void SimulationExtension::WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr)
{
   std::cout << "T=" << aSimTime << " WH " << aEngagementPtr->GetWeaponPlatform()->GetName() << std::endl;
   mEventsPtr->PushEvent(new Events::WeaponHit(aSimTime, aEngagementPtr, aTargetPtr));
}

// =================================================================================================
void SimulationExtension::WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr)
{
   std::cout << "T=" << aSimTime << " WK " << aWeaponPtr->GetName() << std::endl;
   mEventsPtr->PushEvent(new Events::WeaponKilled(aSimTime, aWeaponPtr));
}

// =================================================================================================
void SimulationExtension::WeaponMissed(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr)
{
   std::cout << "T=" << aSimTime << " WM " << aEngagementPtr->GetWeaponPlatform()->GetName() << std::endl;
   mEventsPtr->PushEvent(new Events::WeaponMissed(aSimTime, aEngagementPtr, aTargetPtr));
}

// =================================================================================================
void SimulationExtension::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   // This observer is called sometime during the execution of a frame to indicate that weapon engagement has
   // terminated. Note that FrameComplete will be called after this in the frame.
   std::cout << "T=" << aSimTime << " WT " << aEngagementPtr->GetWeaponPlatform()->GetName() << std::endl;

   // This sets the end time and also schedules the end-time event. We only care about the former.
   GetSimulation().SetEndTime(aSimTime);

   // This tells the simulation to quit after the current frame.
   // We don't HAVE to do this but it is a little cleaner.  If we don't then the simulation will run one frame beyond.
   GetSimulation().RequestTermination();

   // Tell the 'FrameComplete' callback that the weapon has terminated and this is the logical termination of the
   // simulation. There may actually be a frame or two after this to allow things to clean up, but we don't want
   // any further output.
   mWeaponTerminated = true;

   mEventsPtr->PushEvent(new Events::WeaponTerminated(aSimTime, aEngagementPtr));
   mTaskOutputPtr->GetTaskSummary().EngagementResult(aSimTime, aEngagementPtr);
   TaskOutput::GetSummaryData(mTaskPtr->GetRunConfig().GetRunIndex(), mTaskPtr->GetSiteIndex())
      .EngagementResult(aSimTime, aEngagementPtr);
   TaskOutput::ReleaseRunSummary();
}

} // namespace engage
