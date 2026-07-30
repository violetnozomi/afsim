#include "WsfNetworkResourceManagerPlugin.hpp"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"

namespace
{
const char* cEXTENSION_NAME = "wsf_network_resource_manager";

class NetworkResourceManagerApplicationExtension : public WsfApplicationExtension
{
};
}

void Register_wsf_network_resource_manager(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered(cEXTENSION_NAME))
   {
      aApplication.RegisterFeature("network_resource_manager", cEXTENSION_NAME);
      aApplication.RegisterExtension(cEXTENSION_NAME,
                                     ut::make_unique<NetworkResourceManagerApplicationExtension>());
   }
}

extern "C"
{
UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
{
   aVersion =
      UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
}

UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
{
   Register_wsf_network_resource_manager(aApplication);
}
}
