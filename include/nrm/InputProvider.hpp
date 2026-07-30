#ifndef NRM_INPUT_PROVIDER_HPP
#define NRM_INPUT_PROVIDER_HPP

#include <string>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class InputProvider
{
public:
   virtual ~InputProvider() = default;

   virtual const std::string& GetProviderId() const = 0;
   virtual DataOrigin GetOrigin() const = 0;
   virtual bool ReadSnapshot(double aSimTime, ResourceSnapshot& aSnapshot, std::string& aError) = 0;
};
} // namespace nrm

#endif
