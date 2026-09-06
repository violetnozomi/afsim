// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfAltLocSimulationExtension.hpp"

#include "UtRandom.hpp"
#include "WsfAltLocEventResults.hpp"
#include "WsfEventOutput.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
//! Add a static accessor for our observer to the WsfObserver namespace.
//! @param aSimulationPtr A pointer to the simulation object owning the observer.
WsfObserver::EventCallback& WsfObserver::Event(const WsfSimulation* aSimulationPtr)
{
   return wsf::altloc::SimulationExtension::Get(*aSimulationPtr).GetObserver();
}

namespace wsf
{
namespace altloc
{

void SimulationExtension::AddedToSimulation()
{
   // If the event_output extension is available, hook in the handlers for our events.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(GetSimulation());
   if (eventOutputPtr != nullptr)
   {
      eventOutputPtr->AddEvent<ComponentInitialized>("ALTERNATE_LOCATIONS_INITIALIZED", mAltLocEventCallback);
   }
}

SimulationExtension& SimulationExtension::Get(const WsfSimulation& aSimulation)
{
   return static_cast<SimulationExtension&>(aSimulation.GetExtension(cEXTENSION));
}

double SimulationExtension::GetDraw()
{
   if (mData.mGlobalDrawEnabled)
   {
      if (mData.mGlobalDraw == Component::cINVALID_DRAW)
      {
         mData.mGlobalDraw = GetSimulation().GetRandom().Uniform(0.0, 1.0);
      }

      return mData.mGlobalDraw;
   }

   return GetSimulation().GetRandom().Uniform(0.0, 1.0);
}

} // namespace altloc
} // namespace wsf
