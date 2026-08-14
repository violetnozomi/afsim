#include "nrm/OperationalEnvironmentEvaluator.hpp"

#include <cassert>

int main()
{
   nrm::OperationalArea area;
   area.areaId = "square";
   area.validFrom = 10.0;
   area.validUntil = 30.0;
   area.points = {{0.0, 0.0, 0.0}, {0.0, 10.0, 0.0},
                  {10.0, 10.0, 0.0}, {10.0, 0.0, 0.0}};
   assert(nrm::OperationalEnvironmentEvaluator::Contains(area, {5.0, 5.0, 0.0}));
   assert(!nrm::OperationalEnvironmentEvaluator::Contains(area, {11.0, 5.0, 0.0}));
   assert(nrm::OperationalEnvironmentEvaluator::Contains(area, {0.0, 5.0, 0.0}));
   assert(nrm::OperationalEnvironmentEvaluator::Covers(10.0, 30.0, 12.0, 20.0));
   assert(!nrm::OperationalEnvironmentEvaluator::Covers(10.0, 30.0, 5.0, 20.0));

   assert(nrm::OperationalEnvironmentEvaluator::HeadingWithin(350.0, 10.0, 30.0));
   assert(!nrm::OperationalEnvironmentEvaluator::HeadingWithin(350.0, 21.0, 30.0));
   assert(nrm::OperationalEnvironmentEvaluator::BandsOverlap(
      1000000000.0, 4000000.0, 1000000000.0, 2000000.0));
   assert(!nrm::OperationalEnvironmentEvaluator::BandsOverlap(
      1010000000.0, 1000000.0, 1000000000.0, 2000000.0));

   nrm::GeoPoint source{0.0, 0.0, 0.0};
   nrm::GeoPoint east{0.0, 1.0, 0.0};
   assert(nrm::OperationalEnvironmentEvaluator::BearingDeg(source, east) == 90.0);
   return 0;
}
