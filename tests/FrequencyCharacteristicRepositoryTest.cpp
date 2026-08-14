#include "nrm/FrequencyCharacteristicRepository.hpp"

#include <cassert>
#include <fstream>

int main()
{
   nrm::FrequencyCharacteristicRepository repository =
      nrm::FrequencyCharacteristicRepository::BuiltInDemo();
   assert(repository.Valid());
   assert(repository.ForNetwork(nrm::NetworkType::cLINK11).size() == 2);
   assert(repository.ForNetwork(nrm::NetworkType::cLINK16).size() == 2);
   assert(repository.ForNetwork(nrm::NetworkType::cSATCOM).size() == 2);
   assert(repository.ForNetwork(nrm::NetworkType::cCDL).size() == 2);
   const nrm::FrequencyCharacteristic* link16 =
      repository.Find(nrm::NetworkType::cLINK16, 1000000000.0);
   assert(link16 != nullptr);
   assert(link16->capacityScale == 1.0);
   assert(link16->addedDelayMs == 4.0);

   nrm::FrequencyCharacteristicValidation validation;
   assert(repository.LoadFromFile(
      std::string(NRM_SOURCE_DIR) + "/data/frequency_characteristics_v1.txt",
      validation));
   assert(validation.valid);
   const std::string retainedRevision = repository.Revision();

   const std::string invalidPath = "/tmp/nrm-frequency-invalid.txt";
   {
      std::ofstream output(invalidPath);
      output << "NRM_FREQUENCY_CHARACTERISTICS_V1 \"bad-v1\" \"test\"\n";
      output << "FREQUENCY \"a\" LINK16 1000000000 500000 1 -1 98 3 2000000\n";
   }
   assert(!repository.LoadFromFile(invalidPath, validation));
   assert(validation.reason == nrm::FrequencyValidationReason::cINVALID_NUMBER);
   assert(repository.Valid());
   assert(repository.Revision() == retainedRevision);

   const std::string duplicatePath = "/tmp/nrm-frequency-duplicate.txt";
   {
      std::ofstream output(duplicatePath);
      output << "NRM_FREQUENCY_CHARACTERISTICS_V1 \"bad-v2\" \"test\"\n";
      output << "FREQUENCY \"a\" LINK16 1000000000 500000 1 4 98 3 2000000\n";
      output << "FREQUENCY \"b\" LINK16 1000000000 400000 1 5 97 3 2000000\n";
   }
   assert(!repository.LoadFromFile(duplicatePath, validation));
   assert(validation.reason == nrm::FrequencyValidationReason::cDUPLICATE_FREQUENCY);
   assert(repository.Revision() == retainedRevision);
   return 0;
}
