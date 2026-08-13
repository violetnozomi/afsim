#ifndef NRM_ENVIRONMENT_CONFIG_REPOSITORY_HPP
#define NRM_ENVIRONMENT_CONFIG_REPOSITORY_HPP

// Strict, dependency-free configuration repository for the parameterized
// environment effects used only by candidate communication links.

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "nrm/CommunicationCapabilityTypes.hpp"

namespace nrm
{
struct EnvironmentAdjustment
{
   std::string adjustmentId;
   EnvironmentDomain domain = EnvironmentDomain::cTERRAIN;
   NetworkType networkType = NetworkType::cUNKNOWN;
   double pathLossDeltaDb = 0.0;
   double capacityScale = 1.0;
   double packetLossDeltaPercent = 0.0;
   double delayDeltaMs = 0.0;
   bool hardBlocked = false;
   bool valid = false;
};

struct EnvironmentConfigValidation
{
   bool valid = false;
   std::size_t line = 0;
   std::string reason;
};

class EnvironmentConfigRepository
{
public:
   static EnvironmentConfigRepository BuiltInDemo()
   {
      EnvironmentConfigRepository result;
      result.mConfigVersion = "environment-demo-v1";
      result.mProviderId = "nrm-parameterized-environment";
      result.mAdjustments = {
         Make("weather-link11", EnvironmentDomain::cWEATHER, NetworkType::cLINK11, 0.5, 0.98, 0.2, 1.0, false),
         Make("weather-link16", EnvironmentDomain::cWEATHER, NetworkType::cLINK16, 1.0, 0.95, 0.5, 2.0, false),
         Make("weather-satcom", EnvironmentDomain::cWEATHER, NetworkType::cSATCOM, 2.5, 0.85, 1.5, 20.0, false),
         Make("weather-cdl", EnvironmentDomain::cWEATHER, NetworkType::cCDL, 3.0, 0.75, 3.0, 5.0, false),
         Make("interference-link11", EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE, NetworkType::cLINK11, 3.0, 0.75, 4.0, 10.0, false),
         Make("interference-link16", EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE, NetworkType::cLINK16, 3.0, 0.75, 4.0, 10.0, false),
         Make("interference-satcom", EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE, NetworkType::cSATCOM, 2.0, 0.85, 2.0, 15.0, false),
         Make("interference-cdl", EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE, NetworkType::cCDL, 4.0, 0.65, 6.0, 8.0, false)};
      return result;
   }

   bool LoadFromFile(const std::string& aPath, EnvironmentConfigValidation& aValidation)
   {
      std::ifstream input(aPath.c_str());
      if (!input)
      {
         aValidation = {false, 0, "FILE_OPEN_FAILED"};
         return false;
      }
      return Load(input, aValidation);
   }

   bool Load(std::istream& aInput, EnvironmentConfigValidation& aValidation)
   {
      std::string configVersion;
      std::string providerId;
      std::vector<EnvironmentAdjustment> adjustments;
      std::string line;
      std::size_t lineNumber = 0;
      bool headerSeen = false;
      while (std::getline(aInput, line))
      {
         ++lineNumber;
         const std::size_t first = line.find_first_not_of(" \t\r");
         if (first == std::string::npos || line[first] == '#') continue;
         std::istringstream stream(line);
         if (!headerSeen)
         {
            std::string magic;
            if (!(stream >> magic >> std::quoted(configVersion) >> std::quoted(providerId)) ||
                magic != "NRM_ENVIRONMENT_V1" || configVersion.empty() || providerId.empty() ||
                HasTrailing(stream))
            {
               aValidation = {false, lineNumber, "INVALID_HEADER"};
               return false;
            }
            headerSeen = true;
            continue;
         }
         std::string keyword;
         std::string id;
         std::string domainText;
         std::string networkText;
         int hardBlocked = 0;
         EnvironmentAdjustment item;
         if (!(stream >> keyword >> std::quoted(id) >> domainText >> networkText >>
               item.pathLossDeltaDb >> item.capacityScale >>
               item.packetLossDeltaPercent >> item.delayDeltaMs >> hardBlocked) ||
             keyword != "ADJUSTMENT" || id.empty() || HasTrailing(stream) ||
             !ParseDomain(domainText, item.domain) ||
             !ParseNetwork(networkText, item.networkType) ||
             !ValidateNumbers(item) || (hardBlocked != 0 && hardBlocked != 1))
         {
            aValidation = {false, lineNumber, "INVALID_ADJUSTMENT"};
            return false;
         }
         item.adjustmentId = id;
         item.hardBlocked = hardBlocked != 0;
         item.valid = true;
         adjustments.push_back(item);
      }
      if (!headerSeen || adjustments.empty())
      {
         aValidation = {false, lineNumber, "CONFIG_INCOMPLETE"};
         return false;
      }
      mConfigVersion = configVersion;
      mProviderId = providerId;
      mAdjustments.swap(adjustments);
      aValidation = {true, 0, "NONE"};
      return true;
   }

   const EnvironmentAdjustment* Find(EnvironmentDomain aDomain,
                                     NetworkType aNetworkType) const
   {
      for (const EnvironmentAdjustment& item : mAdjustments)
      {
         if (item.valid && item.domain == aDomain &&
             item.networkType == aNetworkType) return &item;
      }
      return nullptr;
   }

   const std::string& ConfigVersion() const { return mConfigVersion; }
   const std::string& ProviderId() const { return mProviderId; }
   const std::vector<EnvironmentAdjustment>& Adjustments() const
   {
      return mAdjustments;
   }

private:
   static EnvironmentAdjustment Make(const char* aId, EnvironmentDomain aDomain,
                                     NetworkType aNetwork, double aLoss,
                                     double aScale, double aPacketLoss,
                                     double aDelay, bool aBlocked)
   {
      EnvironmentAdjustment item;
      item.adjustmentId = aId;
      item.domain = aDomain;
      item.networkType = aNetwork;
      item.pathLossDeltaDb = aLoss;
      item.capacityScale = aScale;
      item.packetLossDeltaPercent = aPacketLoss;
      item.delayDeltaMs = aDelay;
      item.hardBlocked = aBlocked;
      item.valid = true;
      return item;
   }

   static bool HasTrailing(std::istringstream& aStream)
   {
      std::string extra;
      return static_cast<bool>(aStream >> extra);
   }

   static bool ParseDomain(const std::string& aText, EnvironmentDomain& aDomain)
   {
      if (aText == "TERRAIN") aDomain = EnvironmentDomain::cTERRAIN;
      else if (aText == "WEATHER") aDomain = EnvironmentDomain::cWEATHER;
      else if (aText == "CELESTIAL") aDomain = EnvironmentDomain::cCELESTIAL;
      else if (aText == "INTERFERENCE")
         aDomain = EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE;
      else return false;
      return true;
   }

   static bool ParseNetwork(const std::string& aText, NetworkType& aNetwork)
   {
      if (aText == "LINK11") aNetwork = NetworkType::cLINK11;
      else if (aText == "LINK16") aNetwork = NetworkType::cLINK16;
      else if (aText == "SATCOM") aNetwork = NetworkType::cSATCOM;
      else if (aText == "CDL") aNetwork = NetworkType::cCDL;
      else return false;
      return true;
   }

   static bool ValidateNumbers(const EnvironmentAdjustment& aItem)
   {
      return std::isfinite(aItem.pathLossDeltaDb) && aItem.pathLossDeltaDb >= 0.0 &&
             std::isfinite(aItem.capacityScale) && aItem.capacityScale >= 0.0 &&
             aItem.capacityScale <= 1.0 &&
             std::isfinite(aItem.packetLossDeltaPercent) &&
             aItem.packetLossDeltaPercent >= 0.0 &&
             aItem.packetLossDeltaPercent <= 100.0 &&
             std::isfinite(aItem.delayDeltaMs) && aItem.delayDeltaMs >= 0.0;
   }

   std::string mConfigVersion;
   std::string mProviderId;
   std::vector<EnvironmentAdjustment> mAdjustments;
};
} // namespace nrm

#endif
