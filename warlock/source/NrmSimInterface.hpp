#ifndef NRM_SIM_INTERFACE_HPP
#define NRM_SIM_INTERFACE_HPP

#include <map>
#include <string>

#include "NrmSimEvents.hpp"
#include "UtCallbackHolder.hpp"
#include "WkSimInterface.hpp"
#include "nrm/MessageLifecycleTracker.hpp"
#include "nrm/EnvironmentConfigRepository.hpp"
#include "nrm/ResourceEventLedger.hpp"
#include "nrm/RollingMetrics.hpp"

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
   struct LinkRadioState
   {
      nrm::MetricValue<double> bandwidthBps;
      nrm::MetricValue<double> rssiDbm;
      nrm::MetricValue<double> snrDb;
      nrm::MetricValue<double> ber;
      nrm::MetricValue<double> interferencePowerDbm;
      nrm::MetricValue<double> interferenceFactorPercent;
      nrm::MetricValue<double> atmosphericTransmittancePercent;
   };

   void PublishSnapshot(const WsfSimulation& aSimulation, nrm::RuntimeState aState);
   void BuildResourceState(const WsfSimulation& aSimulation);
   void RegisterCallbacks(const WsfSimulation& aSimulation);
   void CountMessage(wsf::comm::Comm* aCommPtr, std::uint64_t nrm::MessageStatistics::*aCounter);
   void PruneCorrelations(double aSimTime);
   static nrm::NetworkType GetNetworkType(const wsf::comm::Comm* aCommPtr);
   nrm::MessageLifecycleTracker                  mLifecycleTracker;
   nrm::ResourceEventLedger                      mEventLedger;
   nrm::EnvironmentConfigRepository              mEnvironmentConfig;

   UtCallbackHolder                              mCallbacks;
   nrm::ResourceSnapshot                         mSnapshot;
   std::map<std::string, nrm::MessageStatistics> mMessagesByNetwork;
   std::map<std::string, nrm::RollingMetrics>     mMetricsByNetwork;
   std::map<std::string, nrm::RollingMetrics>     mMetricsByLink;
   std::map<std::string, LinkRadioState>          mRadioByLink;
   std::map<std::string, nrm::ResourceState>      mLastEndpointStates;
   std::map<std::string, nrm::ResourceState>      mLastLinkStates;
   std::map<unsigned int, double>                 mQueuedTimes;
   std::map<unsigned int, double>                 mTransmittedTimes;
   double                                        mLastPublishTime = -1.0;
};
} // namespace WkNrm

#endif
