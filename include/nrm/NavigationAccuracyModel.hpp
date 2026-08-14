#ifndef NRM_NAVIGATION_ACCURACY_MODEL_HPP
#define NRM_NAVIGATION_ACCURACY_MODEL_HPP

// Minimal parameterized navigation accuracy used only when direct AFSIM or
// customer-provided accuracy fields are absent.

#include <algorithm>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
struct NavigationAccuracyProfile
{
   std::string navigationType;
   double horizontalSigmaM = 0.0;
   double verticalSigmaM = 0.0;
   double headingSigmaDeg = 0.0;
};

class NavigationAccuracyModel
{
public:
   NavigationAccuracyModel()
      : mProfiles{{"GNSS", 5.0, 8.0, 1.0},
                  {"INS", 25.0, 15.0, 2.0},
                  {"INTEGRATED", 3.0, 5.0, 0.5}}
   {
   }

   bool FindProfile(const std::string& aNavigationType,
                    NavigationAccuracyProfile& aProfile) const
   {
      const auto iterator = std::find_if(
         mProfiles.begin(), mProfiles.end(),
         [&aNavigationType](const NavigationAccuracyProfile& aCandidate)
         { return aCandidate.navigationType == aNavigationType; });
      if (iterator == mProfiles.end()) return false;
      aProfile = *iterator;
      return true;
   }

   bool Enrich(NavigationSample& aSample,
               const std::string& aNavigationType) const
   {
      NavigationAccuracyProfile profile;
      if (!FindProfile(aNavigationType, profile)) return false;
      SetIfMissing(aSample.horizontalAccuracySigmaM,
                   profile.horizontalSigmaM, "m", aSample.sampleTime);
      SetIfMissing(aSample.verticalAccuracySigmaM,
                   profile.verticalSigmaM, "m", aSample.sampleTime);
      SetIfMissing(aSample.headingAccuracySigmaDeg,
                   profile.headingSigmaDeg, "deg", aSample.sampleTime);
      return true;
   }

   std::vector<NavigationAccuracyProfile> Profiles() const { return mProfiles; }

private:
   static void SetIfMissing(MetricValue<double>& aMetric, double aValue,
                            const char* aUnit, double aSampleTime)
   {
      if (aMetric.valid) return;
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = DataOrigin::cPARAMETERIZED_MODEL;
      aMetric.confidence = Confidence::cLOW;
      aMetric.sampleTime = aSampleTime;
      aMetric.reason = MetricReason::cNONE;
   }

   std::vector<NavigationAccuracyProfile> mProfiles;
};
} // namespace nrm

#endif
