/**
 * @file FrequencyCharacteristicRepository.hpp
 * @brief Strict parameterized frequency characteristics with atomic revision replacement.
 */

#ifndef NRM_FREQUENCY_CHARACTERISTIC_REPOSITORY_HPP
#define NRM_FREQUENCY_CHARACTERISTIC_REPOSITORY_HPP

#include <cmath>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
struct FrequencyCharacteristic
{
   std::string characteristicId;
   NetworkType networkType = NetworkType::cUNKNOWN;
   double frequencyHz = 0.0;
   double maximumRangeM = 0.0;
   double capacityScale = 0.0;
   double addedDelayMs = 0.0;
   double candidatePdrPercent = 0.0;
   double interferenceThresholdDb = 0.0;
   double protectionBandwidthHz = 0.0;
   DataOrigin origin = DataOrigin::cPARAMETERIZED_MODEL;
   Confidence confidence = Confidence::cLOW;
};

enum class FrequencyValidationReason
{
   cNONE,
   cFILE_OPEN_FAILED,
   cPARSE_ERROR,
   cUNKNOWN_NETWORK_TYPE,
   cINVALID_NUMBER,
   cDUPLICATE_ID,
   cDUPLICATE_FREQUENCY,
   cEMPTY_SET
};

struct FrequencyCharacteristicValidation
{
   bool valid = false;
   FrequencyValidationReason reason = FrequencyValidationReason::cNONE;
   std::string field;
};

class FrequencyCharacteristicRepository
{
public:
   static FrequencyCharacteristicRepository BuiltInDemo()
   {
      FrequencyCharacteristicRepository repository;
      std::vector<FrequencyCharacteristic> values = {
         Value("l11-primary", NetworkType::cLINK11, 225000000.0, 300000.0, 1.0, 80.0, 90.0, 3.0, 2000000.0),
         Value("l11-backup", NetworkType::cLINK11, 235000000.0, 270000.0, 0.9, 90.0, 88.0, 4.0, 2000000.0),
         Value("l16-primary", NetworkType::cLINK16, 1000000000.0, 500000.0, 1.0, 4.0, 98.0, 3.0, 2000000.0),
         Value("l16-backup", NetworkType::cLINK16, 1050000000.0, 450000.0, 0.9, 6.0, 96.0, 4.0, 2000000.0),
         Value("sat-primary", NetworkType::cSATCOM, 20000000000.0, 45000000.0, 1.0, 40.0, 95.0, 2.0, 20000000.0),
         Value("sat-backup", NetworkType::cSATCOM, 20200000000.0, 45000000.0, 0.85, 50.0, 92.0, 3.0, 20000000.0),
         Value("cdl-primary", NetworkType::cCDL, 15000000000.0, 250000.0, 1.0, 2.0, 98.0, 5.0, 10000000.0),
         Value("cdl-backup", NetworkType::cCDL, 15200000000.0, 220000.0, 0.8, 4.0, 95.0, 6.0, 10000000.0)};
      FrequencyCharacteristicValidation validation;
      repository.Replace("demo-frequency-v1", "nrm-parameterized-frequency", values,
                         validation);
      return repository;
   }

   bool LoadFromFile(const std::string& aPath,
                     FrequencyCharacteristicValidation& aValidation)
   {
      std::ifstream input(aPath);
      if (!input) return Reject(aValidation, FrequencyValidationReason::cFILE_OPEN_FAILED, "path");
      std::string magic;
      std::string revision;
      std::string provider;
      if (!(input >> magic >> std::quoted(revision) >> std::quoted(provider)) ||
          magic != "NRM_FREQUENCY_CHARACTERISTICS_V1")
         return Reject(aValidation, FrequencyValidationReason::cPARSE_ERROR, "header");
      std::vector<FrequencyCharacteristic> values;
      std::string record;
      while (input >> record)
      {
         if (!record.empty() && record[0] == '#')
         {
            std::string ignored;
            std::getline(input, ignored);
            continue;
         }
         FrequencyCharacteristic value;
         std::string type;
         if (record != "FREQUENCY" ||
             !(input >> std::quoted(value.characteristicId) >> type >> value.frequencyHz >>
               value.maximumRangeM >> value.capacityScale >> value.addedDelayMs >>
               value.candidatePdrPercent >> value.interferenceThresholdDb >>
               value.protectionBandwidthHz))
            return Reject(aValidation, FrequencyValidationReason::cPARSE_ERROR, "record");
         value.networkType = ParseNetworkType(type);
         values.push_back(value);
      }
      return Replace(revision, provider, values, aValidation);
   }

   bool Replace(const std::string& aRevision, const std::string& aProvider,
                const std::vector<FrequencyCharacteristic>& aValues,
                FrequencyCharacteristicValidation& aValidation)
   {
      if (aRevision.empty() || aProvider.empty() || aValues.empty())
         return Reject(aValidation, FrequencyValidationReason::cEMPTY_SET, "header");
      std::set<std::string> ids;
      std::set<std::pair<int, double>> frequencies;
      for (const FrequencyCharacteristic& value : aValues)
      {
         if (value.networkType == NetworkType::cUNKNOWN)
            return Reject(aValidation, FrequencyValidationReason::cUNKNOWN_NETWORK_TYPE,
                          value.characteristicId);
         if (value.characteristicId.empty() || !ids.insert(value.characteristicId).second)
            return Reject(aValidation, FrequencyValidationReason::cDUPLICATE_ID,
                          value.characteristicId);
         const auto key = std::make_pair(static_cast<int>(value.networkType), value.frequencyHz);
         if (!frequencies.insert(key).second)
            return Reject(aValidation, FrequencyValidationReason::cDUPLICATE_FREQUENCY,
                          value.characteristicId);
         if (!FinitePositive(value.frequencyHz) || !FinitePositive(value.maximumRangeM) ||
             !FinitePositive(value.capacityScale) || !FiniteNonNegative(value.addedDelayMs) ||
             !FiniteNonNegative(value.candidatePdrPercent) || value.candidatePdrPercent > 100.0 ||
             !FiniteNonNegative(value.interferenceThresholdDb) ||
             !FinitePositive(value.protectionBandwidthHz))
            return Reject(aValidation, FrequencyValidationReason::cINVALID_NUMBER,
                          value.characteristicId);
      }
      mRevision = aRevision;
      mProviderId = aProvider;
      mValues = aValues;
      mValid = true;
      aValidation.valid = true;
      aValidation.reason = FrequencyValidationReason::cNONE;
      aValidation.field.clear();
      return true;
   }

   std::vector<FrequencyCharacteristic> ForNetwork(NetworkType aType) const
   {
      std::vector<FrequencyCharacteristic> result;
      for (const FrequencyCharacteristic& value : mValues)
         if (value.networkType == aType) result.push_back(value);
      return result;
   }

   const FrequencyCharacteristic* Find(NetworkType aType, double aFrequencyHz) const
   {
      for (const FrequencyCharacteristic& value : mValues)
         if (value.networkType == aType && value.frequencyHz == aFrequencyHz) return &value;
      return nullptr;
   }

   bool Valid() const { return mValid; }
   const std::string& Revision() const { return mRevision; }
   const std::string& ProviderId() const { return mProviderId; }

private:
   static FrequencyCharacteristic Value(const char* aId, NetworkType aType,
                                        double aFrequencyHz, double aRangeM,
                                        double aCapacityScale, double aDelayMs,
                                        double aPdrPercent, double aThresholdDb,
                                        double aProtectionHz)
   {
      FrequencyCharacteristic value;
      value.characteristicId = aId;
      value.networkType = aType;
      value.frequencyHz = aFrequencyHz;
      value.maximumRangeM = aRangeM;
      value.capacityScale = aCapacityScale;
      value.addedDelayMs = aDelayMs;
      value.candidatePdrPercent = aPdrPercent;
      value.interferenceThresholdDb = aThresholdDb;
      value.protectionBandwidthHz = aProtectionHz;
      return value;
   }

   static NetworkType ParseNetworkType(const std::string& aValue)
   {
      if (aValue == "LINK11") return NetworkType::cLINK11;
      if (aValue == "LINK16") return NetworkType::cLINK16;
      if (aValue == "SATCOM") return NetworkType::cSATCOM;
      if (aValue == "CDL") return NetworkType::cCDL;
      return NetworkType::cUNKNOWN;
   }

   static bool FinitePositive(double aValue) { return std::isfinite(aValue) && aValue > 0.0; }
   static bool FiniteNonNegative(double aValue) { return std::isfinite(aValue) && aValue >= 0.0; }

   static bool Reject(FrequencyCharacteristicValidation& aValidation,
                      FrequencyValidationReason aReason, const std::string& aField)
   {
      aValidation.valid = false;
      aValidation.reason = aReason;
      aValidation.field = aField;
      return false;
   }

   std::string mRevision;
   std::string mProviderId;
   std::vector<FrequencyCharacteristic> mValues;
   bool mValid = false;
};
}

#endif
