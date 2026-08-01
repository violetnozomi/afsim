#ifndef NRM_NETWORK_PROFILE_REPOSITORY_HPP
#define NRM_NETWORK_PROFILE_REPOSITORY_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "nrm/NetworkProfile.hpp"

namespace nrm
{
class NetworkProfileRepository
{
public:
   static constexpr const char* cSCHEMA_VERSION = "nrm.network_profile.v1";

   NetworkProfileRepository() = default;

   static NetworkProfileRepository BuiltInDemo()
   {
      NetworkProfileRepository repository;
      std::vector<NetworkProfile> profiles;
      profiles.push_back(DemoProfile("demo-link11-v1", NetworkType::cLINK11, "LINK11_DEMO",
                                     225000000.0, 300000.0, 80.0, 90.0, 2400.0, 128));
      profiles.push_back(DemoProfile("demo-link16-v1", NetworkType::cLINK16, "LINK16_DEMO",
                                     1000000000.0, 500000.0, 20.0, 95.0, 238000.0, 128));
      profiles.push_back(DemoProfile("demo-satcom-v1", NetworkType::cSATCOM, "SATCOM_DEMO",
                                     20000000000.0, 45000000.0, 40.0, 92.0, 5000000.0, 64));
      profiles.push_back(DemoProfile("demo-cdl-v1", NetworkType::cCDL, "CDL_DEMO",
                                     15000000000.0, 250000.0, 10.0, 96.0, 45000000.0, 32));
      NetworkProfileValidation validation;
      repository.Replace("demo-0.7.0", "nrm-built-in-demo", profiles, validation);
      return repository;
   }

   bool Replace(const std::string& aConfigVersion,
                const std::string& aProviderId,
                const std::vector<NetworkProfile>& aProfiles,
                NetworkProfileValidation& aValidation)
   {
      mConfigVersion = aConfigVersion;
      mProviderId = aProviderId;
      if (aProviderId.empty())
      {
         return Fail(aValidation, NetworkProfileValidationReason::cMISSING_REQUIRED_FIELD,
                     "providerId", std::string());
      }
      if (aConfigVersion.empty())
      {
         return Fail(aValidation, NetworkProfileValidationReason::cMISSING_CONFIG_VERSION,
                     "configVersion", std::string());
      }
      if (aProfiles.empty())
      {
         return Fail(aValidation, NetworkProfileValidationReason::cMISSING_REQUIRED_FIELD,
                     "profiles", std::string());
      }
      std::set<std::string> profileIds;
      std::vector<NetworkProfile> validated = aProfiles;
      for (NetworkProfile& profile : validated)
      {
         profile.configVersion = aConfigVersion;
         if (profile.schemaVersion.empty())
         {
            return Fail(aValidation, NetworkProfileValidationReason::cMISSING_SCHEMA_VERSION,
                        "schemaVersion", profile.profileId);
         }
         if (profile.schemaVersion != cSCHEMA_VERSION)
         {
            return Fail(aValidation, NetworkProfileValidationReason::cUNSUPPORTED_SCHEMA_VERSION,
                        "schemaVersion", profile.profileId);
         }
         if (profile.profileId.empty())
         {
            return Fail(aValidation, NetworkProfileValidationReason::cMISSING_PROFILE_ID,
                        "profileId", std::string());
         }
         if (!profileIds.insert(profile.profileId).second)
         {
            return Fail(aValidation, NetworkProfileValidationReason::cDUPLICATE_PROFILE_ID,
                        "profileId", profile.profileId);
         }
         if (profile.networkType == NetworkType::cUNKNOWN)
         {
            return Fail(aValidation, NetworkProfileValidationReason::cUNKNOWN_NETWORK_TYPE,
                        "networkType", profile.profileId);
         }
         if (!std::isfinite(profile.maximumRangeM) ||
             !std::isfinite(profile.establishmentDelayMs) ||
             !std::isfinite(profile.serviceCapacityBps) ||
             profile.maximumRangeM < 0.0 || profile.establishmentDelayMs < 0.0 ||
             profile.serviceCapacityBps < 0.0)
         {
            return Fail(aValidation, NetworkProfileValidationReason::cNEGATIVE_VALUE,
                        "numericCapacity", profile.profileId);
         }
         if (!std::isfinite(profile.candidatePdrPercent) ||
             profile.candidatePdrPercent < 0.0 || profile.candidatePdrPercent > 100.0)
         {
            return Fail(aValidation, NetworkProfileValidationReason::cPDR_OUT_OF_RANGE,
                        "candidatePdrPercent", profile.profileId);
         }
         if (profile.protocolModel.empty() || profile.frequenciesHz.empty() ||
             profile.propagationModelId.empty() || profile.maximumMembers == 0 ||
             profile.supportedBusinessTypes.empty())
         {
            return Fail(aValidation, NetworkProfileValidationReason::cMISSING_REQUIRED_FIELD,
                        "profileFields", profile.profileId);
         }
         if (std::find_if(profile.frequenciesHz.begin(), profile.frequenciesHz.end(),
                          [](double aFrequency)
                          {
                             return !std::isfinite(aFrequency) || aFrequency <= 0.0;
                          }) !=
             profile.frequenciesHz.end())
         {
            return Fail(aValidation, NetworkProfileValidationReason::cNEGATIVE_VALUE,
                        "frequenciesHz", profile.profileId);
         }
         profile.providerId = aProviderId;
         profile.valid = true;
      }
      mConfigVersion = aConfigVersion;
      mProviderId = aProviderId;
      mProfiles.swap(validated);
      mValid = true;
      aValidation.valid = true;
      aValidation.reason = NetworkProfileValidationReason::cNONE;
      aValidation.field.clear();
      aValidation.profileId.clear();
      mLastValidation = aValidation;
      return true;
   }

   // External format is a strict, whitespace-delimited NRM profile grammar:
   // NRM_NETWORK_PROFILES_V1 "config" "provider"
   // PROFILE "id" TYPE "protocol" frequency range delay pdr capacity
   //         "propagation" maxMembers "business1,business2"
   bool LoadFromFile(const std::string& aPath, NetworkProfileValidation& aValidation)
   {
      std::ifstream input(aPath);
      if (!input)
      {
         return Fail(aValidation, NetworkProfileValidationReason::cFILE_OPEN_FAILED,
                     "path", std::string());
      }
      std::string magic;
      std::string configVersion;
      std::string providerId;
      if (!(input >> magic >> std::quoted(configVersion) >> std::quoted(providerId)) ||
          magic != "NRM_NETWORK_PROFILES_V1")
      {
         return Fail(aValidation, NetworkProfileValidationReason::cPARSE_ERROR,
                     "header", std::string());
      }
      std::vector<NetworkProfile> profiles;
      std::string recordType;
      while (input >> recordType)
      {
         if (recordType.empty() || recordType[0] == '#')
         {
            std::string ignored;
            std::getline(input, ignored);
            continue;
         }
         if (recordType != "PROFILE")
         {
            return Fail(aValidation, NetworkProfileValidationReason::cPARSE_ERROR,
                        "recordType", std::string());
         }
         NetworkProfile profile;
         std::string networkType;
         std::string businesses;
         double frequencyHz = 0.0;
         if (!(input >> std::quoted(profile.profileId) >> networkType >>
               std::quoted(profile.protocolModel) >> frequencyHz >>
               profile.maximumRangeM >> profile.establishmentDelayMs >>
               profile.candidatePdrPercent >> profile.serviceCapacityBps >>
               std::quoted(profile.propagationModelId) >> profile.maximumMembers >>
               std::quoted(businesses)))
         {
            return Fail(aValidation, NetworkProfileValidationReason::cPARSE_ERROR,
                        "profile", profile.profileId);
         }
         profile.schemaVersion = cSCHEMA_VERSION;
         profile.networkType = ParseNetworkType(networkType);
         profile.frequenciesHz.push_back(frequencyHz);
         profile.supportedBusinessTypes = SplitBusinesses(businesses);
         profile.source = DataOrigin::cPARAMETERIZED_MODEL;
         profile.confidence = Confidence::cLOW;
         profiles.push_back(profile);
      }
      return Replace(configVersion, providerId, profiles, aValidation);
   }

   const NetworkProfile* Find(NetworkType aNetworkType) const
   {
      if (!mValid)
      {
         return nullptr;
      }
      for (const NetworkProfile& profile : mProfiles)
      {
         if (profile.valid && profile.networkType == aNetworkType)
         {
            return &profile;
         }
      }
      return nullptr;
   }

   bool Valid() const { return mValid; }
   const std::string& ConfigVersion() const { return mConfigVersion; }
   const std::string& ProviderId() const { return mProviderId; }
   const std::vector<NetworkProfile>& Profiles() const { return mProfiles; }
   const NetworkProfileValidation& LastValidation() const { return mLastValidation; }

private:
   static NetworkProfile DemoProfile(const char* aId,
                                     NetworkType aType,
                                     const char* aProtocol,
                                     double aFrequencyHz,
                                     double aRangeM,
                                     double aDelayMs,
                                     double aPdr,
                                     double aCapacity,
                                     std::size_t aMaximumMembers)
   {
      NetworkProfile profile;
      profile.profileId = aId;
      profile.schemaVersion = cSCHEMA_VERSION;
      profile.networkType = aType;
      profile.protocolModel = aProtocol;
      profile.frequenciesHz.push_back(aFrequencyHz);
      profile.maximumRangeM = aRangeM;
      profile.establishmentDelayMs = aDelayMs;
      profile.candidatePdrPercent = aPdr;
      profile.serviceCapacityBps = aCapacity;
      profile.propagationModelId = "free-space-demo";
      profile.maximumMembers = aMaximumMembers;
      profile.supportedBusinessTypes.push_back("*");
      profile.source = DataOrigin::cPARAMETERIZED_MODEL;
      profile.confidence = Confidence::cLOW;
      return profile;
   }

   static NetworkType ParseNetworkType(const std::string& aValue)
   {
      if (aValue == "LINK11") return NetworkType::cLINK11;
      if (aValue == "LINK16") return NetworkType::cLINK16;
      if (aValue == "SATCOM") return NetworkType::cSATCOM;
      if (aValue == "CDL") return NetworkType::cCDL;
      return NetworkType::cUNKNOWN;
   }

   static std::vector<std::string> SplitBusinesses(const std::string& aValue)
   {
      std::vector<std::string> output;
      std::istringstream input(aValue);
      std::string token;
      while (std::getline(input, token, ','))
      {
         if (!token.empty())
         {
            output.push_back(token);
         }
      }
      return output;
   }

   bool Fail(NetworkProfileValidation& aValidation,
             NetworkProfileValidationReason aReason,
             const std::string& aField,
             const std::string& aProfileId)
   {
      aValidation.valid = false;
      aValidation.reason = aReason;
      aValidation.field = aField;
      aValidation.profileId = aProfileId;
      mLastValidation = aValidation;
      mValid = false;
      return false;
   }

   std::string mConfigVersion;
   std::string mProviderId;
   std::vector<NetworkProfile> mProfiles;
   NetworkProfileValidation mLastValidation;
   bool mValid = false;
};
} // namespace nrm

#endif
