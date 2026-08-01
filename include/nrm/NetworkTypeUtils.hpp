#ifndef NRM_NETWORK_TYPE_UTILS_HPP
#define NRM_NETWORK_TYPE_UTILS_HPP

#include <algorithm>
#include <cctype>
#include <string>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
inline std::string NormalizeNetworkToken(std::string aValue)
{
   std::transform(aValue.begin(),
                  aValue.end(),
                  aValue.begin(),
                  [](unsigned char aCharacter) { return static_cast<char>(std::tolower(aCharacter)); });
   return aValue;
}

inline NetworkType ClassifyNetwork(const std::string& aNetworkName, const std::string& aModelType)
{
   const std::string token = NormalizeNetworkToken(aNetworkName + " " + aModelType);
   if (token.find("link11") != std::string::npos || token.find("link_11") != std::string::npos ||
       token.find("nrm_l11") != std::string::npos)
   {
      return NetworkType::cLINK11;
   }
   if (token.find("link16") != std::string::npos || token.find("link_16") != std::string::npos ||
       token.find("jtids") != std::string::npos || token.find("nrm_l16") != std::string::npos)
   {
      return NetworkType::cLINK16;
   }
   if (token.find("satcom") != std::string::npos || token.find("satellite") != std::string::npos ||
       token.find("nrm_sat") != std::string::npos)
   {
      return NetworkType::cSATCOM;
   }
   if (token.find("cdl") != std::string::npos)
   {
      return NetworkType::cCDL;
   }
   return NetworkType::cUNKNOWN;
}

inline const char* ToString(NetworkType aType)
{
   switch (aType)
   {
   case NetworkType::cLINK11:
      return "LINK11";
   case NetworkType::cLINK16:
      return "LINK16";
   case NetworkType::cSATCOM:
      return "SATCOM";
   case NetworkType::cCDL:
      return "CDL";
   case NetworkType::cUNKNOWN:
   default:
      return "UNKNOWN";
   }
}

inline const char* ToString(ResourceState aState)
{
   switch (aState)
   {
   case ResourceState::cONLINE:
      return "ONLINE";
   case ResourceState::cOFFLINE:
      return "OFFLINE";
   case ResourceState::cDISABLED:
      return "DISABLED";
   case ResourceState::cFAILED:
      return "FAILED";
   case ResourceState::cUNKNOWN:
   default:
      return "UNKNOWN";
   }
}

inline const char* ToString(DataOrigin aOrigin)
{
   switch (aOrigin)
   {
   case DataOrigin::cCUSTOMER_MODULE:
      return "CUSTOMER_MODULE";
   case DataOrigin::cREPLAY:
      return "REPLAY";
   case DataOrigin::cPARAMETERIZED_MODEL:
      return "PARAMETERIZED_MODEL";
   case DataOrigin::cESTIMATED:
      return "ESTIMATED";
   case DataOrigin::cDERIVED:
      return "DERIVED";
   case DataOrigin::cAFSIM_INTERNAL:
   default:
      return "AFSIM_INTERNAL";
   }
}

inline const char* ToString(Confidence aConfidence)
{
   switch (aConfidence)
   {
   case Confidence::cHIGH:
      return "HIGH";
   case Confidence::cMEDIUM:
      return "MEDIUM";
   case Confidence::cLOW:
   default:
      return "LOW";
   }
}
} // namespace nrm

#endif
