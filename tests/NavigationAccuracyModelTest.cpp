#include "nrm/NavigationAccuracyModel.hpp"

#include <cassert>
#include <cmath>

namespace
{
bool Near(double aLeft, double aRight)
{
   return std::abs(aLeft - aRight) < 1.0e-9;
}
}

int main()
{
   const nrm::NavigationAccuracyModel model;

   nrm::NavigationAccuracyProfile profile;
   assert(model.FindProfile("GNSS", profile));
   assert(Near(profile.horizontalSigmaM, 5.0));
   assert(Near(profile.verticalSigmaM, 8.0));
   assert(Near(profile.headingSigmaDeg, 1.0));
   assert(model.FindProfile("INS", profile));
   assert(Near(profile.horizontalSigmaM, 25.0));
   assert(Near(profile.verticalSigmaM, 15.0));
   assert(Near(profile.headingSigmaDeg, 2.0));
   assert(model.FindProfile("INTEGRATED", profile));
   assert(Near(profile.horizontalSigmaM, 3.0));
   assert(Near(profile.verticalSigmaM, 5.0));
   assert(Near(profile.headingSigmaDeg, 0.5));

   nrm::NavigationSample sample;
   sample.sampleTime = 12.0;
   assert(model.Enrich(sample, "GNSS"));
   assert(sample.horizontalAccuracySigmaM.valid);
   assert(Near(sample.horizontalAccuracySigmaM.value, 5.0));
   assert(sample.horizontalAccuracySigmaM.origin ==
          nrm::DataOrigin::cPARAMETERIZED_MODEL);
   assert(sample.horizontalAccuracySigmaM.confidence == nrm::Confidence::cLOW);

   sample.totalPositionErrorM.value = 7.25;
   sample.totalPositionErrorM.valid = true;
   sample.totalPositionErrorM.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   assert(model.Enrich(sample, "INTEGRATED"));
   assert(Near(sample.totalPositionErrorM.value, 7.25));
   assert(sample.totalPositionErrorM.origin ==
          nrm::DataOrigin::cCUSTOMER_MODULE);

   nrm::NavigationSample unknown;
   assert(!model.Enrich(unknown, "UNKNOWN"));
   assert(!unknown.horizontalAccuracySigmaM.valid);
   assert(!unknown.verticalAccuracySigmaM.valid);
   assert(!unknown.headingAccuracySigmaDeg.valid);
   return 0;
}
