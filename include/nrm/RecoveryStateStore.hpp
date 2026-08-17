#ifndef NRM_RECOVERY_STATE_STORE_HPP
#define NRM_RECOVERY_STATE_STORE_HPP

// Atomic persistence for small, validated restart metadata. This deliberately
// does not restore simulation time or unfinished communication tasks. The
// component is independently tested; runtime startup wiring is deferred until
// the customer confirms the target persistence location and recovery policy.

#include <cmath>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "nrm/TemporaryPathGuard.hpp"

namespace nrm
{
struct UnacknowledgedPackageState
{
   std::string packageId;
   std::string fingerprint;
   std::uint64_t planRevision = 0;
};

struct RecoveryState
{
   std::string configPath;
   std::string configVersion;
   std::string planId;
   std::uint64_t planRevision = 0;
   std::string demandSetId;
   std::uint64_t demandRevision = 0;
   double savedAt = 0.0;
   std::vector<UnacknowledgedPackageState> unacknowledgedPackages;
   bool valid = false;
};

class RecoveryStateStore
{
public:
   bool Save(const std::string& aPath, const RecoveryState& aState) const
   {
      if (!Valid(aState) || aPath.empty()) return false;
      const std::string temporaryPath = aPath + ".tmp";
      TemporaryPathGuard temporaryGuard(temporaryPath);
      {
         std::ofstream output(temporaryPath.c_str(),
                              std::ios::out | std::ios::trunc);
         if (!output) return false;
         output << "NRM_RECOVERY_V1\n"
                << "CONFIG_PATH " << std::quoted(aState.configPath) << "\n"
                << "CONFIG_VERSION " << std::quoted(aState.configVersion) << "\n"
                << "PLAN " << std::quoted(aState.planId) << ' '
                << aState.planRevision << "\n"
                << "DEMAND " << std::quoted(aState.demandSetId) << ' '
                << aState.demandRevision << "\n"
                << "SAVED_AT " << std::setprecision(17) << aState.savedAt << "\n"
                << "PACKAGE_COUNT " << aState.unacknowledgedPackages.size()
                << "\n";
         for (const UnacknowledgedPackageState& package :
              aState.unacknowledgedPackages)
         {
            output << "PACKAGE " << std::quoted(package.packageId) << ' '
                   << std::quoted(package.fingerprint) << ' '
                   << package.planRevision << "\n";
         }
         output << "END\n";
         output.flush();
         if (!output)
         {
            return false;
         }
      }
      if (std::rename(temporaryPath.c_str(), aPath.c_str()) != 0)
      {
         return false;
      }
      temporaryGuard.Commit();
      return true;
   }

   bool Load(const std::string& aPath, RecoveryState& aState) const
   {
      std::ifstream input(aPath.c_str());
      if (!input) return false;
      RecoveryState candidate;
      std::string token;
      std::size_t packageCount = 0;
      if (!(input >> token) || token != "NRM_RECOVERY_V1" ||
          !(input >> token) || token != "CONFIG_PATH" ||
          !(input >> std::quoted(candidate.configPath)) ||
          !(input >> token) || token != "CONFIG_VERSION" ||
          !(input >> std::quoted(candidate.configVersion)) ||
          !(input >> token) || token != "PLAN" ||
          !(input >> std::quoted(candidate.planId) >> candidate.planRevision) ||
          !(input >> token) || token != "DEMAND" ||
          !(input >> std::quoted(candidate.demandSetId) >>
            candidate.demandRevision) ||
          !(input >> token) || token != "SAVED_AT" ||
          !(input >> candidate.savedAt) ||
          !(input >> token) || token != "PACKAGE_COUNT" ||
          !(input >> packageCount) || packageCount > 4096)
         return false;
      for (std::size_t index = 0; index < packageCount; ++index)
      {
         UnacknowledgedPackageState package;
         if (!(input >> token) || token != "PACKAGE" ||
             !(input >> std::quoted(package.packageId) >>
               std::quoted(package.fingerprint) >> package.planRevision))
            return false;
         candidate.unacknowledgedPackages.push_back(package);
      }
      if (!(input >> token) || token != "END" || (input >> token)) return false;
      candidate.valid = true;
      if (!Valid(candidate)) return false;
      aState = candidate;
      return true;
   }

private:
   static bool Valid(const RecoveryState& aState)
   {
      if (!aState.valid || aState.configPath.empty() ||
          aState.configVersion.empty() || !std::isfinite(aState.savedAt) ||
          aState.savedAt < 0.0)
         return false;
      for (const UnacknowledgedPackageState& package :
           aState.unacknowledgedPackages)
      {
         if (package.packageId.empty() || package.fingerprint.empty())
            return false;
      }
      return true;
   }
};
} // namespace nrm

#endif
