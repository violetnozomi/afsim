#ifndef NRM_SIM_INTERFACE_HPP
#define NRM_SIM_INTERFACE_HPP

#include "NrmSimEvents.hpp"
#include "UtCallbackHolder.hpp"
#include "WkSimInterface.hpp"

class WsfSimulation;

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

   UtCallbackHolder       mCallbacks;
   nrm::FrameworkSnapshot mSnapshot;
};
} // namespace WkNrm

#endif

