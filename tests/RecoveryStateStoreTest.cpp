#include "nrm/RecoveryStateStore.hpp"

#include <cassert>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
   const std::string root =
      "/tmp/nrm-recovery-state-" + std::to_string(getpid());
   const std::string path = root + "/recovery.state";
   assert(::mkdir(root.c_str(), 0700) == 0);

   nrm::RecoveryState state;
   state.configPath = "data/resource_manager_defaults.txt";
   state.configVersion = "demo-0.8.0";
   state.planId = "plan-001";
   state.planRevision = 4;
   state.demandSetId = "demand-001";
   state.demandRevision = 7;
   state.savedAt = 18.5;
   state.valid = true;
   state.unacknowledgedPackages.push_back(
      {"package-01", "fingerprint-01", 4});

   nrm::RecoveryStateStore store;
   assert(store.Save(path, state));
   nrm::RecoveryState loaded;
   assert(store.Load(path, loaded));
   assert(loaded.configPath == state.configPath);
   assert(loaded.planRevision == 4);
   assert(loaded.demandRevision == 7);
   assert(loaded.unacknowledgedPackages.size() == 1);
   assert(loaded.unacknowledgedPackages.front().fingerprint ==
          "fingerprint-01");
   assert(::access((path + ".tmp").c_str(), F_OK) != 0);

   nrm::RecoveryState preserved = loaded;
   assert(!store.Load(root + "/missing.state", preserved));
   assert(preserved.planId == "plan-001");
   assert(preserved.planRevision == 4);

   {
      std::ofstream output(path.c_str(), std::ios::trunc);
      output << "NRM_RECOVERY_V1\nCONFIG_PATH only\n";
   }
   assert(!store.Load(path, preserved));
   assert(preserved.demandRevision == 7);

   {
      std::ofstream output(path.c_str(), std::ios::trunc);
      output << "NRM_RECOVERY_V1\n"
             << "CONFIG_PATH config.txt\nCONFIG_VERSION v1\n"
             << "PLAN plan 1\nDEMAND demand 2\nSAVED_AT nan\n"
             << "PACKAGE_COUNT 0\nEND\n";
   }
   assert(!store.Load(path, preserved));
   assert(preserved.configVersion == "demo-0.8.0");
   return 0;
}
