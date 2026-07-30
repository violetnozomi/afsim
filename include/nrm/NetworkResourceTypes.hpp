#ifndef NRM_NETWORK_RESOURCE_TYPES_HPP
#define NRM_NETWORK_RESOURCE_TYPES_HPP

#include <cstddef>
#include <cstdint>

namespace nrm
{
enum class NetworkType
{
   cUNKNOWN,
   cLINK11,
   cLINK16,
   cSATCOM,
   cCDL
};

enum class DataOrigin
{
   cAFSIM_INTERNAL,
   cCUSTOMER_MODULE,
   cPARAMETERIZED_MODEL,
   cDERIVED
};

enum class RuntimeState
{
   cIDLE,
   cINITIALIZING,
   cRUNNING,
   cCOMPLETE
};

struct FrameworkSnapshot
{
   std::uint64_t snapshotVersion = 0;
   double        simTime         = 0.0;
   RuntimeState  runtimeState    = RuntimeState::cIDLE;
   std::size_t   networkCount    = 0;
   std::size_t   endpointCount   = 0;
   std::uint64_t transmitted     = 0;
   std::uint64_t received        = 0;
   std::uint64_t hops            = 0;
};
} // namespace nrm

#endif

