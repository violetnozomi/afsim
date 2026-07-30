#ifndef NRM_SIM_INTERFACE_HPP
#define NRM_SIM_INTERFACE_HPP

#include <map>
#include <string>

#include "NrmSimEvents.hpp"
#include "UtCallbackHolder.hpp"
#include "WkSimInterface.hpp"

class WsfSimulation;
class WsfMessage;

namespace wsf
{
namespace comm
{
class Comm;
class NetworkManager;
} // namespace comm
} // namespace wsf

namespace WkNrm
{
class SimInterface : public warlock::SimInterfaceT<SimEvent>
{
   Q_OBJECT

public:
   explicit SimInterface(const QString& aPluginName);

protected:
   void SimulationInitializing(const WsfSimulation& aSimulation) override;
   void SimulationStarting(const WsfSimulation& aSimulation) override;
   void SimulationClockRead(const WsfSimulation& aSimulation) override;
   void SimulationComplete(const WsfSimulation& aSimulation) override;

private:
   void PublishSnapshot(const WsfSimulation& aSimulation, nrm::RuntimeState aState);
   void BuildResourceState(const WsfSimulation& aSimulation);
   void RegisterCallbacks(const WsfSimulation& aSimulation);
   void CountMessage(wsf::comm::Comm* aCommPtr, std::uint64_t nrm::MessageStatistics::*aCounter);
   static nrm::NetworkType GetNetworkType(const wsf::comm::Comm* aCommPtr);

   UtCallbackHolder                           mCallbacks;
   nrm::ResourceSnapshot                      mSnapshot;
   std::map<std::string, nrm::MessageStatistics> mMessagesByNetwork;
   double                                     mLastPublishTime = -1.0;
};
} // namespace WkNrm

#endif
