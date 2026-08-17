#include "nrm/TemporaryPathGuard.hpp"

#include <cassert>
#include <fstream>
#include <string>
#include <unistd.h>

namespace
{
bool Exists(const std::string& aPath)
{
   std::ifstream input(aPath);
   return input.good();
}
}

int main()
{
   const std::string base =
      "/tmp/nrm-temporary-path-guard-" + std::to_string(getpid());
   {
      std::ofstream output(base);
      output << "temporary";
      assert(output.good());
   }
   {
      nrm::TemporaryPathGuard guard(base);
      assert(guard.Active());
   }
   assert(!Exists(base));

   {
      std::ofstream output(base);
      output << "committed";
   }
   {
      nrm::TemporaryPathGuard guard(base);
      guard.Commit();
      assert(!guard.Active());
   }
   assert(Exists(base));
   std::remove(base.c_str());

   bool customCleanupCalled = false;
   {
      nrm::TemporaryPathGuard guard(
         "virtual-staging-path",
         [&customCleanupCalled](const std::string& aPath)
         {
            assert(aPath == "virtual-staging-path");
            customCleanupCalled = true;
         });
   }
   assert(customCleanupCalled);
   return 0;
}
