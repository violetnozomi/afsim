#ifndef NRM_INPUT_PROVIDER_HPP
#define NRM_INPUT_PROVIDER_HPP

// Stable extension seam for later customer AFSIM providers. The built-in
// collector does not instantiate this interface directly.

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
