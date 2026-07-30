#ifndef NRM_SIM_EVENTS_HPP
#define NRM_SIM_EVENTS_HPP

#include "NrmDataContainer.hpp"
#include "WkSimInterface.hpp"

namespace WkNrm
{
class SimEvent : public warlock::SimEvent
{
public:
   explicit SimEvent(bool aRecurring = false)
      : warlock::SimEvent(aRecurring)
   {
   }

   virtual void Process(DataContainer& aData) = 0;
};

class SnapshotEvent : public SimEvent
{
public:
   explicit SnapshotEvent(const nrm::FrameworkSnapshot& aSnapshot)
      : SimEvent(true)
      , mSnapshot(aSnapshot)
   {
   }

   void Process(DataContainer& aData) override { aData.SetSnapshot(mSnapshot); }

private:
   nrm::FrameworkSnapshot mSnapshot;
};
} // namespace WkNrm

#endif

