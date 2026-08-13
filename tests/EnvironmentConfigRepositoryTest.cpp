#include "nrm/EnvironmentConfigRepository.hpp"

#include <cassert>
#include <sstream>

int main()
{
   nrm::EnvironmentConfigRepository repository =
      nrm::EnvironmentConfigRepository::BuiltInDemo();
   assert(repository.Find(nrm::EnvironmentDomain::cWEATHER,
                          nrm::NetworkType::cSATCOM) != nullptr);

   std::istringstream valid(
      "NRM_ENVIRONMENT_V1 \"test-v1\" \"test-provider\"\n"
      "ADJUSTMENT \"rain-cdl\" WEATHER CDL 2.0 0.8 3.0 4.0 0\n"
      "ADJUSTMENT \"jam-l16\" INTERFERENCE LINK16 5.0 0.5 8.0 12.0 0\n");
   nrm::EnvironmentConfigValidation validation;
   assert(repository.Load(valid, validation));
   assert(validation.valid);
   assert(repository.ConfigVersion() == "test-v1");
   const nrm::EnvironmentAdjustment* weather = repository.Find(
      nrm::EnvironmentDomain::cWEATHER, nrm::NetworkType::cCDL);
   assert(weather != nullptr);
   assert(weather->capacityScale == 0.8);

   std::istringstream invalid(
      "NRM_ENVIRONMENT_V1 \"bad-v1\" \"bad-provider\"\n"
      "ADJUSTMENT \"bad\" WEATHER CDL 2.0 1.5 3.0 4.0 0\n");
   assert(!repository.Load(invalid, validation));
   assert(!validation.valid);
   // Failed loads are atomic and preserve the last known-good configuration.
   assert(repository.ConfigVersion() == "test-v1");
   assert(repository.Find(nrm::EnvironmentDomain::cWEATHER,
                          nrm::NetworkType::cCDL) != nullptr);
   return 0;
}
