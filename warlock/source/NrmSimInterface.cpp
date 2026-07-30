#include "NrmSimInterface.hpp"

#include "UtMemory.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfSimulation.hpp"

WkNrm::SimInterface::SimInterface(const QString& aPluginName)
   : warlock::SimInterfaceT<SimEvent>(aPluginName)
{
}

void WkNrm::SimInterface::SimulationInitializing(const WsfSimulation& aSimulation)
{
   mCallbacks.Clear();
   mSnapshot = nrm::FrameworkSnapshot();

   mCallbacks.Add(
      WsfObserver::MessageTransmitted(&aSimulation)
         .Connect([this](double, wsf::comm::Comm*, const WsfMessage&) { ++mSnapshot.transmitted; }));

   mCallbacks.Add(WsfObserver::MessageReceived(&aSimulation)
                     .Connect([this](double,
                                     wsf::comm::Comm*,
                                     wsf::comm::Comm*,
                                     const WsfMessage&,
                                     wsf::comm::Result&) { ++mSnapshot.received; }));

   mCallbacks.Add(WsfObserver::MessageHop(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm*, wsf::comm::Comm*, const WsfMessage&)
                              { ++mSnapshot.hops; }));

   PublishSnapshot(aSimulation, nrm::RuntimeState::cINITIALIZING);
}

void WkNrm::SimInterface::SimulationStarting(const WsfSimulation& aSimulation)
{
   PublishSnapshot(aSimulation, nrm::RuntimeState::cRUNNING);
}

void WkNrm::SimInterface::SimulationClockRead(const WsfSimulation& aSimulation)
{
   PublishSnapshot(aSimulation, nrm::RuntimeState::cRUNNING);
}

void WkNrm::SimInterface::SimulationComplete(const WsfSimulation& aSimulation)
{
   PublishSnapshot(aSimulation, nrm::RuntimeState::cCOMPLETE);
   mCallbacks.Clear();
}

void WkNrm::SimInterface::PublishSnapshot(const WsfSimulation& aSimulation, nrm::RuntimeState aState)
{
   ++mSnapshot.snapshotVersion;
   mSnapshot.simTime      = aSimulation.GetSimTime();
   mSnapshot.runtimeState = aState;

   const wsf::comm::NetworkManager* networkManagerPtr = aSimulation.GetCommNetworkManager();
   if (networkManagerPtr != nullptr)
   {
      mSnapshot.networkCount  = networkManagerPtr->GetManagedNetworks().size();
      mSnapshot.endpointCount = networkManagerPtr->GetComms().size();
   }
   else
   {
      mSnapshot.networkCount  = 0;
      mSnapshot.endpointCount = 0;
   }

   AddSimEvent(ut::make_unique<SnapshotEvent>(mSnapshot));
}

