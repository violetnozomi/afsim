#include "nrm/EffectiveSnapshotAssembler.hpp"

#include <cassert>

namespace
{
nrm::NavigationSample Navigation(const char* aPlatformId, double aLatitude,
                                 nrm::DataOrigin aOrigin)
{
   nrm::NavigationSample sample;
   sample.platformId = aPlatformId;
   sample.platformName = aPlatformId;
   sample.sampleTime = aLatitude;
   sample.valid = true;
   sample.origin = aOrigin;
   sample.truthLatitudeDeg.value = aLatitude;
   sample.truthLatitudeDeg.valid = true;
   return sample;
}
}

int main()
{
   nrm::ResourceSnapshot customer;
   customer.snapshotVersion = 5;
   customer.simTime = 10.0;
   customer.providerId = "customer";
   customer.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   nrm::NetworkSnapshot customerNetwork;
   customerNetwork.networkId = "customer-network";
   customer.networks.push_back(customerNetwork);
   customer.navigation.valid = true;
   customer.navigation.providerId = "customer-navigation";
   nrm::NavigationSample navigation;
   navigation.platformId = "aircraft-1";
   navigation.valid = true;
   customer.navigation.platforms.push_back(navigation);
   customer.environment.valid = true;
   customer.environment.providerId = "customer-environment";

   nrm::ResourceSnapshot customerOnly =
      nrm::EffectiveSnapshotAssembler::Compose(nullptr, &customer);
   assert(customerOnly.networks.front().networkId == "customer-network");

   nrm::ResourceSnapshot afsim;
   afsim.snapshotVersion = 100;
   afsim.simTime = 12.0;
   afsim.providerId = "afsim-internal";
   nrm::NetworkSnapshot afsimNetwork;
   afsimNetwork.networkId = "afsim-network";
   afsim.networks.push_back(afsimNetwork);
   afsim.navigation.valid = true;
   afsim.navigation.providerId = "afsim-navigation";
   afsim.environment.valid = true;
   afsim.environment.providerId = "afsim-environment";

   const nrm::ResourceSnapshot effective =
      nrm::EffectiveSnapshotAssembler::Compose(&afsim, &customer);
   assert(effective.networks.size() == 1);
   assert(effective.networks.front().networkId == "afsim-network");
   assert(effective.providerId == "afsim-internal");
   assert(effective.navigation.providerId == "customer-navigation");
   assert(effective.navigation.platforms.front().platformId == "aircraft-1");
   assert(effective.environment.providerId == "customer-environment");
   assert(effective.simTime == 12.0);

   customer.networks.front().networkId = "stale-customer-network";
   customer.simTime = 11.0;
   const nrm::ResourceSnapshot staleCustomer =
      nrm::EffectiveSnapshotAssembler::Compose(&afsim, &customer);
   assert(staleCustomer.networks.front().networkId == "afsim-network");

   // Customer navigation is a per-platform overlay, not a replacement for
   // the complete AFSIM navigation domain.
   afsim.navigation.platforms = {
      Navigation("platform-x", 12.0, nrm::DataOrigin::cAFSIM_INTERNAL),
      Navigation("platform-y", 10.0, nrm::DataOrigin::cAFSIM_INTERNAL)};
   customer.navigation.platforms = {
      Navigation("platform-y", 11.0, nrm::DataOrigin::cCUSTOMER_MODULE)};
   const nrm::ResourceSnapshot navigationOverlay =
      nrm::EffectiveSnapshotAssembler::Compose(&afsim, &customer);
   assert(navigationOverlay.navigation.platforms.size() == 2);
   assert(navigationOverlay.navigation.platforms.at(0).platformId == "platform-x");
   assert(navigationOverlay.navigation.platforms.at(0).truthLatitudeDeg.value == 12.0);
   assert(navigationOverlay.navigation.platforms.at(1).platformId == "platform-y");
   assert(navigationOverlay.navigation.platforms.at(1).truthLatitudeDeg.value == 11.0);

   // Customer environment reports are partial by schema.  Missing customer
   // subdomains must retain the newest AFSIM values.
   afsim.environment.terrain.available = true;
   afsim.environment.terrain.enabled = true;
   afsim.environment.weather.available = true;
   afsim.environment.weather.rainRateMmPerHour.valid = true;
   afsim.environment.weather.rainRateMmPerHour.value = 8.0;
   customer.environment.terrain = nrm::TerrainEnvironmentState();
   customer.environment.weather = nrm::WeatherEnvironmentState();
   customer.environment.celestial = nrm::CelestialEnvironmentState();
   customer.environment.interference.available = true;
   customer.environment.interference.observedLinkCount = 1;
   const nrm::ResourceSnapshot environmentOverlay =
      nrm::EffectiveSnapshotAssembler::Compose(&afsim, &customer);
   assert(environmentOverlay.environment.terrain.available);
   assert(environmentOverlay.environment.terrain.enabled);
   assert(environmentOverlay.environment.weather.available);
   assert(environmentOverlay.environment.weather.rainRateMmPerHour.value == 8.0);
   assert(environmentOverlay.environment.interference.available);
   assert(environmentOverlay.environment.interference.observedLinkCount == 1);

   return 0;
}
