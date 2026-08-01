#ifndef NRM_MESSAGE_LIFECYCLE_TYPES_HPP
#define NRM_MESSAGE_LIFECYCLE_TYPES_HPP

#include <cstdint>
#include <string>

namespace nrm
{
enum class MessageTerminalState
{
   cPENDING,
   cDELIVERED,
   cDISCARDED,
   cROUTING_FAILED,
   cEXPIRED
};

inline const char* ToString(MessageTerminalState aState)
{
   switch (aState)
   {
   case MessageTerminalState::cPENDING:
      return "PENDING";
   case MessageTerminalState::cDELIVERED:
      return "DELIVERED";
   case MessageTerminalState::cDISCARDED:
      return "DISCARDED";
   case MessageTerminalState::cROUTING_FAILED:
      return "ROUTING_FAILED";
   case MessageTerminalState::cEXPIRED:
      return "EXPIRED";
   }
   return "PENDING";
}

struct MessageLifecycleRecord
{
   std::uint64_t messageId = 0;
   std::string networkId;
   std::string sourceEndpointId;
   std::string destinationEndpointId;
   std::string businessType;
   std::uint64_t bits = 0;
   double queuedTime = -1.0;
   double transmittedTime = -1.0;
   double terminalTime = -1.0;
   MessageTerminalState terminalState = MessageTerminalState::cPENDING;
};
} // namespace nrm

#endif
