#include "nrm/ResourceEventLedger.hpp"

#include <cmath>
#include <iostream>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

nrm::ResourceEvent StateEvent(const char* id, nrm::ResourceEventKind kind,
                              const char* entity, double time,
                              nrm::ResourceState previous,
                              nrm::ResourceState current)
{
   nrm::ResourceEvent event;
   event.eventId = id;
   event.kind = kind;
   event.simTime = time;
   event.previousState = previous;
   event.currentState = current;
   if (kind == nrm::ResourceEventKind::cENDPOINT_STATE)
      event.endpointId = entity;
   else
      event.linkId = entity;
   return event;
}

int main()
{
   nrm::ResourceEventLedger ledger;
   CHECK(ledger.Record(StateEvent("e0", nrm::ResourceEventKind::cENDPOINT_STATE,
                                  "endpoint", 0.0, nrm::ResourceState::cUNKNOWN,
                                  nrm::ResourceState::cONLINE)));
   CHECK(ledger.Record(StateEvent("e1", nrm::ResourceEventKind::cENDPOINT_STATE,
                                  "endpoint", 5.0, nrm::ResourceState::cONLINE,
                                  nrm::ResourceState::cOFFLINE)));
   CHECK(ledger.Record(StateEvent("e2", nrm::ResourceEventKind::cENDPOINT_STATE,
                                  "endpoint", 8.0, nrm::ResourceState::cOFFLINE,
                                  nrm::ResourceState::cONLINE)));
   CHECK(!ledger.Record(StateEvent("e2", nrm::ResourceEventKind::cENDPOINT_STATE,
                                   "endpoint", 8.0, nrm::ResourceState::cOFFLINE,
                                   nrm::ResourceState::cONLINE)));
   CHECK(ledger.DuplicateEventCount() == 1);

   const nrm::ResourceStateMetrics endpoint =
      ledger.EndpointMetrics("endpoint", 10.0, 10.0);
   CHECK(endpoint.currentOfflineDurationS.valid);
   CHECK(endpoint.currentOfflineDurationS.value == 0.0);
   CHECK(endpoint.windowOfflineDurationS.valid);
   CHECK(std::abs(endpoint.windowOfflineDurationS.value - 3.0) < 1.0e-9);
   CHECK(endpoint.endpointOnlineRatioPercent.valid);
   CHECK(std::abs(endpoint.endpointOnlineRatioPercent.value - 70.0) < 1.0e-9);
   CHECK(!endpoint.establishmentSuccessRatioPercent.valid);

   CHECK(ledger.Record(StateEvent("l0", nrm::ResourceEventKind::cLINK_STATE,
                                  "link", 0.0, nrm::ResourceState::cUNKNOWN,
                                  nrm::ResourceState::cONLINE)));
   nrm::ResourceEvent attempt1;
   attempt1.eventId = "a1";
   attempt1.kind = nrm::ResourceEventKind::cESTABLISHMENT_ATTEMPTED;
   attempt1.linkId = "link";
   attempt1.correlationId = "attempt-1";
   attempt1.simTime = 2.0;
   CHECK(ledger.Record(attempt1));
   nrm::ResourceEvent success = attempt1;
   success.eventId = "s1";
   success.kind = nrm::ResourceEventKind::cESTABLISHMENT_SUCCEEDED;
   success.simTime = 2.5;
   CHECK(ledger.Record(success));
   nrm::ResourceEvent attempt2 = attempt1;
   attempt2.eventId = "a2";
   attempt2.correlationId = "attempt-2";
   attempt2.simTime = 6.0;
   CHECK(ledger.Record(attempt2));
   nrm::ResourceEvent failed = attempt2;
   failed.eventId = "f2";
   failed.kind = nrm::ResourceEventKind::cESTABLISHMENT_FAILED;
   failed.simTime = 7.0;
   CHECK(ledger.Record(failed));

   const nrm::ResourceStateMetrics link = ledger.LinkMetrics("link", 10.0, 10.0);
   CHECK(link.serviceAvailabilityPercent.valid);
   CHECK(link.serviceAvailabilityPercent.value == 100.0);
   CHECK(link.establishmentAttempts == 2);
   CHECK(link.establishmentSuccesses == 1);
   CHECK(link.establishmentSuccessRatioPercent.value == 50.0);
   CHECK(std::abs(link.averageEstablishmentDelayMs.value - 500.0) < 1.0e-9);

   nrm::ResourceEventLedger outOfOrder;
   CHECK(outOfOrder.Record(StateEvent("later", nrm::ResourceEventKind::cLINK_STATE,
                                      "link", 5.0, nrm::ResourceState::cONLINE,
                                      nrm::ResourceState::cOFFLINE)));
   CHECK(outOfOrder.Record(StateEvent("earlier", nrm::ResourceEventKind::cLINK_STATE,
                                      "link", 0.0, nrm::ResourceState::cUNKNOWN,
                                      nrm::ResourceState::cONLINE)));
   CHECK(outOfOrder.OutOfOrderEventCount() == 1);
   CHECK(outOfOrder.LinkMetrics("link", 7.0, 7.0).currentOfflineDurationS.value == 2.0);
   return 0;
}
