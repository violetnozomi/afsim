// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfSensorsManagerFovSensorScheduler.hpp"

#include "WsfSensorMode.hpp"
#include "WsfSimulation.hpp"

WsfSensorsManagerFovSensorScheduler::WsfSensorsManagerFovSensorScheduler()
   : WsfDefaultSensorScheduler()
{}

WsfSensorsManagerFovSensorScheduler::WsfSensorsManagerFovSensorScheduler(
   const WsfDefaultSensorScheduler& base)
   : WsfDefaultSensorScheduler(base)
{}

// virtual
WsfSensorsManagerFovSensorScheduler::~WsfSensorsManagerFovSensorScheduler()
{}

WsfSensorsManagerFovSensorScheduler::WsfSensorsManagerFovSensorScheduler(
   const WsfSensorsManagerFovSensorScheduler& src)
   : WsfDefaultSensorScheduler(src)
{}

// virtual
WsfSensorScheduler* WsfSensorsManagerFovSensorScheduler::Clone() const
{
   return new WsfSensorsManagerFovSensorScheduler(*this);
}

// Override TurnOn to change how it reschedules the request list.
// The remainder of the code is the same.
//virtual
void WsfSensorsManagerFovSensorScheduler::TurnOn(double simTime)
{
   CheckSearchModeAvailability();

   WsfSensor& sensor = *(this->mSensorPtr);
   WsfSimulation& sim = *(sensor.GetSimulation());

   // refresh the search list
   ResetSearchList();
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* target = sim.GetPlatformEntry(i);
      if (target != sensor.GetPlatform())
      {
         sensor.PlatformAdded(simTime, target);
      }
   }
   UpdateSearchChanceInterval();
   mNextSearchVisitTime = simTime;
   mCheckSearchList = false;

   // Redistribute the visit times for the request list. The base class version
   // of this code did not include a random factor, so it would often set the next
   // visit time for all requests to the exact same time.
   ut::Random& random = sim.GetRandom();
   double nextVisitTime = simTime;
   for (RequestListIndex i = 0; i < mRequestList.size(); ++i)
   {
      size_t modeIndex = mRequestList[i].mModeIndex;
      WsfSensorMode& mode = *(this->mModeList[modeIndex]);
      nextVisitTime += random.Uniform(0.0, mode.GetFrameTime());
      mRequestList[i].mNextVisitTime = nextVisitTime;
      nextVisitTime += mode.GetDwellTime();
   }
   UpdateNextTrackVisitTime();
   mDwellEndTime = simTime;
}

// virtual
void WsfSensorsManagerFovSensorScheduler::TurnOff(double simTime)
{
   // WsfDefaultSensorScheduler clears the request list when it
   // is turned off. The sensor manager FOV turns off the sensor
   // while it is slewing. We do not want to clear the request list
   // when this happens, or else the sensor will have nothing to
   // search for when it finishes slewing.

   // Clear the search list because the TurnOn function restores it.
   // TTRs should not be using the search feature, so clearing this
   // list should not make a difference, but we risk unexpected
   // behavior otherwise.
   ResetSearchList();
}
