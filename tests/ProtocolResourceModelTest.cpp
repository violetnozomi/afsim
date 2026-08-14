#include "nrm/ProtocolResourceModel.hpp"

#include <cassert>

int main()
{
   nrm::ProtocolResourceModel model;
   const auto link11 = model.Evaluate(nrm::NetworkType::cLINK11, 0, 64000.0);
   assert(link11.available);
   assert(link11.state.kind == "POLLING_UNIT");
   assert(link11.state.capacity == 8);
   assert(link11.state.used == 1);
   assert(link11.state.remaining == 7);
   assert(link11.addedDelayMs == 125.0);

   const auto link16 = model.Evaluate(nrm::NetworkType::cLINK16, 0, 64000.0);
   assert(link16.state.kind == "TIMESLOT");
   assert(link16.state.capacity == 16);
   assert(link16.addedDelayMs == 7.8125);
   assert(model.Evaluate(nrm::NetworkType::cLINK16, 15, 1.0).available);
   const auto exhausted = model.Evaluate(nrm::NetworkType::cLINK16, 16, 1.0);
   assert(!exhausted.available);
   assert(exhausted.reason == nrm::ProtocolResourceReason::cTIMESLOT_EXHAUSTED);
   assert(exhausted.state.used == 17);
   assert(exhausted.state.remaining == 0);

   assert(model.Evaluate(nrm::NetworkType::cSATCOM, 0, 1.0).state.capacity == 8);
   assert(model.Evaluate(nrm::NetworkType::cCDL, 0, 1.0).state.capacity == 4);
   assert(!model.Evaluate(nrm::NetworkType::cUNKNOWN, 0, 1.0).available);
   return 0;
}
