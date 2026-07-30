#include "nrm/NetworkResourceTypes.hpp"

#include <cassert>

int main()
{
   nrm::FrameworkSnapshot snapshot;
   assert(snapshot.snapshotVersion == 0);
   assert(snapshot.runtimeState == nrm::RuntimeState::cIDLE);
   assert(snapshot.networkCount == 0);
   assert(snapshot.endpointCount == 0);
   assert(snapshot.transmitted == 0);
   assert(snapshot.received == 0);
   assert(snapshot.hops == 0);

   snapshot.runtimeState = nrm::RuntimeState::cRUNNING;
   snapshot.networkCount = 4;
   assert(snapshot.runtimeState == nrm::RuntimeState::cRUNNING);
   assert(snapshot.networkCount == 4);
   return 0;
}

